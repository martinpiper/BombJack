//
// usbmouse.h
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
#ifndef _uspi_usbmouse_h
#define _uspi_usbmouse_h

#include <uspi/usbfunction.h>
#include <uspi/usbendpoint.h>
#include <uspi/usbrequest.h>
#include <uspi/types.h>

#define MOUSE_BOOT_REPORT_SIZE	3

typedef void TMouseStatusHandler (unsigned nButtons, int nDisplacementX, int nDisplacementY);

typedef struct TUSBMouseDevice
{
	TUSBFunction m_USBFunction;

	TUSBEndpoint *m_pReportEndpoint;

	TMouseStatusHandler *m_pStatusHandler;

	u16 m_usReportDescriptorLength;
	u8 *m_pHIDReportDescriptor;

	TUSBRequest m_URB;
	u8 *m_pReportBuffer;
}
TUSBMouseDevice;

void USBMouseDevice (TUSBMouseDevice *pThis, TUSBFunction *pFunction);
void _CUSBMouseDevice (TUSBMouseDevice *pThis);

boolean USBMouseDeviceConfigure (TUSBFunction *pUSBFunction);

void USBMouseDeviceRegisterStatusHandler (TUSBMouseDevice *pThis, TMouseStatusHandler *pStatusHandler);

#endif
