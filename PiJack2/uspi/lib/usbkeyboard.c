//
// usbkeyboard.c
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014-2018  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <uspi/usbkeyboard.h>
#include <uspi/usbhid.h>
#include <uspi/usbhostcontroller.h>
#include <uspi/devicenameservice.h>
#include <uspi/macros.h>
#include <uspi/assert.h>
#include <uspios.h>

//#define REPEAT_ENABLE					// does not work well with any Keyboard

#define MSEC2HZ(msec)		((msec) * HZ / 1000)

#define REPEAT_DELAY		MSEC2HZ (400)
#define REPEAT_RATE		MSEC2HZ (80)

static unsigned s_nDeviceNumber = 1;

static const char FromUSBKbd[] = "usbkbd";

static void USBKeyboardDeviceGenerateKeyEvent (TUSBKeyboardDevice *pThis, u8 ucPhyCode);
static boolean USBKeyboardDeviceStartRequest (TUSBKeyboardDevice *pThis);
static void USBKeyboardDeviceCompletionRoutine (TUSBRequest *pURB, void *pParam, void *pContext);
static u8 USBKeyboardDeviceGetModifiers (TUSBKeyboardDevice *pThis);
static u8 USBKeyboardDeviceGetKeyCode (TUSBKeyboardDevice *pThis);
#ifdef REPEAT_ENABLE
static void USBKeyboardDeviceTimerHandler (unsigned hTimer, void *pParam, void *pContext);
#endif

void USBKeyboardDevice (TUSBKeyboardDevice *pThis, TUSBFunction *pDevice)
{
	assert (pThis != 0);

	USBFunctionCopy (&pThis->m_USBFunction, pDevice);
	pThis->m_USBFunction.Configure = USBKeyboardDeviceConfigure;

	pThis->m_pReportEndpoint = 0;
	pThis->m_pKeyPressedHandler = 0;
	pThis->m_pSelectConsoleHandler = 0;
	pThis->m_pShutdownHandler = 0;
	pThis->m_pKeyStatusHandlerRaw = 0;
	pThis->m_pReportBuffer = 0;
	pThis->m_ucLastPhyCode = 0;
	pThis->m_hTimer = 0;
	pThis->m_ucLastLEDStatus = 0;

	KeyMap (&pThis->m_KeyMap);

	pThis->m_pReportBuffer = malloc (BOOT_REPORT_SIZE);
	assert (pThis->m_pReportBuffer != 0);
}

void _CUSBKeyboardDevice (TUSBKeyboardDevice *pThis)
{
	assert (pThis != 0);

	if (pThis->m_pReportBuffer != 0)
	{
		free (pThis->m_pReportBuffer);
		pThis->m_pReportBuffer = 0;
	}

	if (pThis->m_pReportEndpoint != 0)
	{
		_USBEndpoint (pThis->m_pReportEndpoint);
		free (pThis->m_pReportEndpoint);
		pThis->m_pReportEndpoint = 0;
	}

	_KeyMap (&pThis->m_KeyMap);
	_USBFunction (&pThis->m_USBFunction);
}

boolean USBKeyboardDeviceConfigure (TUSBFunction *pUSBFunction)
{
	TUSBKeyboardDevice *pThis = (TUSBKeyboardDevice *) pUSBFunction;
	assert (pThis != 0);

	if (USBFunctionGetNumEndpoints (&pThis->m_USBFunction) < 1)
	{
		USBFunctionConfigurationError (&pThis->m_USBFunction, FromUSBKbd);

		return FALSE;
	}

	TUSBEndpointDescriptor *pEndpointDesc;
	while ((pEndpointDesc = (TUSBEndpointDescriptor *)
			USBFunctionGetDescriptor (&pThis->m_USBFunction, DESCRIPTOR_ENDPOINT)) != 0)
	{
		if (   (pEndpointDesc->bEndpointAddress & 0x80) != 0x80		// Input EP
		    || (pEndpointDesc->bmAttributes     & 0x3F)	!= 0x03)	// Interrupt EP
		{
			continue;
		}

		assert (pThis->m_pReportEndpoint == 0);
		pThis->m_pReportEndpoint = malloc (sizeof (TUSBEndpoint));
		assert (pThis->m_pReportEndpoint != 0);
		USBEndpoint2 (pThis->m_pReportEndpoint, USBFunctionGetDevice (&pThis->m_USBFunction), pEndpointDesc);

		break;
	}

	if (pThis->m_pReportEndpoint == 0)
	{
		USBFunctionConfigurationError (&pThis->m_USBFunction, FromUSBKbd);

		return FALSE;
	}
	
	if (!USBFunctionConfigure (&pThis->m_USBFunction))
	{
		LogWrite (FromUSBKbd, LOG_ERROR, "Cannot set interface");

		return FALSE;
	}

	if (DWHCIDeviceControlMessage (USBFunctionGetHost (&pThis->m_USBFunction),
				       USBFunctionGetEndpoint0 (&pThis->m_USBFunction),
				       REQUEST_OUT | REQUEST_CLASS | REQUEST_TO_INTERFACE,
				       SET_PROTOCOL, BOOT_PROTOCOL,
				       USBFunctionGetInterfaceNumber (&pThis->m_USBFunction), 0, 0) < 0)
	{
		LogWrite (FromUSBKbd, LOG_ERROR, "Cannot set boot protocol");

		return FALSE;
	}

	// setting the LED status forces some keyboard adapters to work
	USBKeyboardDeviceSetLEDs (pThis, pThis->m_ucLastLEDStatus);

	TString DeviceName;
	String (&DeviceName);
	StringFormat (&DeviceName, "ukbd%u", s_nDeviceNumber++);
	DeviceNameServiceAddDevice (DeviceNameServiceGet (), StringGet (&DeviceName), pThis, FALSE);

	_String (&DeviceName);

	return USBKeyboardDeviceStartRequest (pThis);
}

void USBKeyboardDeviceRegisterKeyPressedHandler (TUSBKeyboardDevice *pThis, TKeyPressedHandler *pKeyPressedHandler)
{
	assert (pThis != 0);
	assert (pKeyPressedHandler != 0);
	pThis->m_pKeyPressedHandler = pKeyPressedHandler;
}

void USBKeyboardDeviceRegisterSelectConsoleHandler (TUSBKeyboardDevice *pThis, TSelectConsoleHandler *pSelectConsoleHandler)
{
	assert (pThis != 0);
	assert (pSelectConsoleHandler != 0);
	pThis->m_pSelectConsoleHandler = pSelectConsoleHandler;
}

void USBKeyboardDeviceRegisterShutdownHandler (TUSBKeyboardDevice *pThis, TShutdownHandler *pShutdownHandler)
{
	assert (pThis != 0);
	assert (pShutdownHandler != 0);
	pThis->m_pShutdownHandler = pShutdownHandler;
}

void USBKeyboardDeviceUpdateLEDs (TUSBKeyboardDevice *pThis)
{
	assert (pThis != 0);
	u8 ucLEDStatus = KeyMapGetLEDStatus (&pThis->m_KeyMap);
	if (ucLEDStatus != pThis->m_ucLastLEDStatus)
	{
		USBKeyboardDeviceSetLEDs (pThis, ucLEDStatus);

		pThis->m_ucLastLEDStatus = ucLEDStatus;
	}
}

void USBKeyboardDeviceRegisterKeyStatusHandlerRaw (TUSBKeyboardDevice *pThis, TKeyStatusHandlerRaw *pKeyStatusHandlerRaw)
{
	assert (pThis != 0);
	assert (pKeyStatusHandlerRaw != 0);
	pThis->m_pKeyStatusHandlerRaw = pKeyStatusHandlerRaw;
}

void USBKeyboardDeviceSetLEDs (TUSBKeyboardDevice *pThis, u8 ucLEDMask)
{
	assert (pThis != 0);

	u8 LEDs[1] ALIGN(4) = {ucLEDMask};	// DMA buffer

	if (DWHCIDeviceControlMessage (USBFunctionGetHost (&pThis->m_USBFunction),
				       USBFunctionGetEndpoint0 (&pThis->m_USBFunction),
				       REQUEST_OUT | REQUEST_CLASS | REQUEST_TO_INTERFACE,
				       SET_REPORT, (REPORT_TYPE_OUTPUT << 8) | 0,
				       USBFunctionGetInterfaceNumber (&pThis->m_USBFunction),
				       LEDs, sizeof LEDs) < 0)
	{
		LogWrite (FromUSBKbd, LOG_WARNING, "Cannot set LEDs");
	}
}

void USBKeyboardDeviceGenerateKeyEvent (TUSBKeyboardDevice *pThis, u8 ucPhyCode)
{
	assert (pThis != 0);

	const char *pKeyString;
	char Buffer[2];

	u8 ucModifiers = USBKeyboardDeviceGetModifiers (pThis);
	u16 usLogCode = KeyMapTranslate (&pThis->m_KeyMap, ucPhyCode, ucModifiers);

	switch (usLogCode)
	{
	case ActionSwitchCapsLock:
	case ActionSwitchNumLock:
	case ActionSwitchScrollLock:
		break;

	case ActionSelectConsole1:
	case ActionSelectConsole2:
	case ActionSelectConsole3:
	case ActionSelectConsole4:
	case ActionSelectConsole5:
	case ActionSelectConsole6:
	case ActionSelectConsole7:
	case ActionSelectConsole8:
	case ActionSelectConsole9:
	case ActionSelectConsole10:
	case ActionSelectConsole11:
	case ActionSelectConsole12:
		if (pThis->m_pSelectConsoleHandler != 0)
		{
			unsigned nConsole = usLogCode - ActionSelectConsole1;
			assert (nConsole < 12);

			(*pThis->m_pSelectConsoleHandler) (nConsole);
		}
		break;

	case ActionShutdown:
		if (pThis->m_pShutdownHandler != 0)
		{
			(*pThis->m_pShutdownHandler) ();
		}
		break;

	default:
		pKeyString = KeyMapGetString (&pThis->m_KeyMap, usLogCode, ucModifiers, Buffer);
		if (pKeyString != 0)
		{
			if (pThis->m_pKeyPressedHandler != 0)
			{
				(*pThis->m_pKeyPressedHandler) (pKeyString);
			}
		}
		break;
	}
}

boolean USBKeyboardDeviceStartRequest (TUSBKeyboardDevice *pThis)
{
	assert (pThis != 0);

	assert (pThis->m_pReportEndpoint != 0);
	assert (pThis->m_pReportBuffer != 0);
	
	USBRequest (&pThis->m_URB, pThis->m_pReportEndpoint, pThis->m_pReportBuffer, BOOT_REPORT_SIZE, 0);
	USBRequestSetCompletionRoutine (&pThis->m_URB, USBKeyboardDeviceCompletionRoutine, 0, pThis);
	
	return DWHCIDeviceSubmitAsyncRequest (USBFunctionGetHost (&pThis->m_USBFunction), &pThis->m_URB);
}

void USBKeyboardDeviceCompletionRoutine (TUSBRequest *pURB, void *pParam, void *pContext)
{
	TUSBKeyboardDevice *pThis = (TUSBKeyboardDevice *) pContext;
	assert (pThis != 0);
	
	assert (pURB != 0);
	assert (&pThis->m_URB == pURB);

	if (   USBRequestGetStatus (pURB) != 0
	    && USBRequestGetResultLength (pURB) == BOOT_REPORT_SIZE)
	{
		if (pThis->m_pKeyStatusHandlerRaw != 0)
		{
			(*pThis->m_pKeyStatusHandlerRaw) (USBKeyboardDeviceGetModifiers (pThis), pThis->m_pReportBuffer+2);
		}
		else
		{
			u8 ucPhyCode = USBKeyboardDeviceGetKeyCode (pThis);

			if (ucPhyCode == pThis->m_ucLastPhyCode)
			{
				ucPhyCode = 0;
			}
			else
			{
				pThis->m_ucLastPhyCode = ucPhyCode;
			}
			
			if (ucPhyCode != 0)
			{
				USBKeyboardDeviceGenerateKeyEvent (pThis, ucPhyCode);
#ifdef REPEAT_ENABLE
				if (pThis->m_hTimer != 0)
				{
					CancelKernelTimer (pThis->m_hTimer);
				}

				pThis->m_hTimer = StartKernelTimer (REPEAT_DELAY, USBKeyboardDeviceTimerHandler, 0, pThis);
				assert (pThis->m_hTimer != 0);
#endif
			}
			else if (pThis->m_hTimer != 0)
			{
				CancelKernelTimer (pThis->m_hTimer);
				pThis->m_hTimer = 0;
			}
		}
	}

	_USBRequest (&pThis->m_URB);
	
	USBKeyboardDeviceStartRequest (pThis);
}

u8 USBKeyboardDeviceGetModifiers (TUSBKeyboardDevice *pThis)
{
	assert (pThis != 0);
	return pThis->m_pReportBuffer[0];
}

u8 USBKeyboardDeviceGetKeyCode (TUSBKeyboardDevice *pThis)
{
	assert (pThis != 0);

	for (unsigned i = 7; i >= 2; i--)
	{
		u8 ucKeyCode = pThis->m_pReportBuffer[i];
		if (ucKeyCode != 0)
		{
			return ucKeyCode;
		}
	}
	
	return 0;
}

#ifdef REPEAT_ENABLE

void USBKeyboardDeviceTimerHandler (unsigned hTimer, void *pParam, void *pContext)
{
	TUSBKeyboardDevice *pThis = (TUSBKeyboardDevice *) pContext;
	assert (pThis != 0);

	assert (hTimer == pThis->m_hTimer);

	if (pThis->m_ucLastPhyCode != 0)
	{
		USBKeyboardDeviceGenerateKeyEvent (pThis, pThis->m_ucLastPhyCode);

		pThis->m_hTimer = StartKernelTimer (REPEAT_RATE, USBKeyboardDeviceTimerHandler, 0, pThis);
		assert (pThis->m_hTimer != 0);
	}
}

#endif
