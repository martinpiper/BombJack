//
// usbconfigparser.c
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
#include <uspi/usbconfigparser.h>
#include <uspios.h>
#include <uspi/assert.h>

#define SKIP_BYTES(pDesc, nBytes)	((TUSBDescriptor *) ((u8 *) (pDesc) + (nBytes)))

void USBConfigurationParser (TUSBConfigurationParser *pThis, const void *pBuffer, unsigned nBufLen)
{
	assert (pThis != 0);
	pThis->m_pBuffer = (TUSBDescriptor *) pBuffer;
	pThis->m_nBufLen = nBufLen;
	pThis->m_bValid = FALSE;
	pThis->m_pEndPosition = SKIP_BYTES (pThis->m_pBuffer, nBufLen);
	pThis->m_pNextPosition = pThis->m_pBuffer;
	pThis->m_pCurrentDescriptor = 0;
	pThis->m_pErrorPosition = pThis->m_pBuffer;

	assert (pThis->m_pBuffer != 0);
	
	if (   pThis->m_nBufLen < 4		// wTotalLength must exist
	    || pThis->m_nBufLen > 512)		// best guess
	{
		return;
	}

	if (   pThis->m_pBuffer->Configuration.bLength         != sizeof (TUSBConfigurationDescriptor)
	    || pThis->m_pBuffer->Configuration.bDescriptorType != DESCRIPTOR_CONFIGURATION
	    || pThis->m_pBuffer->Configuration.wTotalLength    >  nBufLen)
	{
		return;
	}

	if (pThis->m_pBuffer->Configuration.wTotalLength < nBufLen)
	{
		pThis->m_pEndPosition = SKIP_BYTES (pThis->m_pBuffer, pThis->m_pBuffer->Configuration.wTotalLength);
	}

	const TUSBDescriptor *pCurrentPosition = pThis->m_pBuffer;
	u8 ucLastDescType = 0;
	boolean bInAudioInterface = FALSE;
	while (SKIP_BYTES (pCurrentPosition, 2) < pThis->m_pEndPosition)
	{
		u8 ucDescLen  = pCurrentPosition->Header.bLength;
		u8 ucDescType = pCurrentPosition->Header.bDescriptorType;

		TUSBDescriptor *pDescEnd = SKIP_BYTES (pCurrentPosition, ucDescLen);
		if (pDescEnd > pThis->m_pEndPosition)
		{
			pThis->m_pErrorPosition = pCurrentPosition;
			return;
		}

		u8 ucExpectedLen = 0;
		switch (ucDescType)
		{
		case DESCRIPTOR_CONFIGURATION:
			if (ucLastDescType != 0)
			{
				pThis->m_pErrorPosition = pCurrentPosition;
				return;
			}
			ucExpectedLen = sizeof (TUSBConfigurationDescriptor);
			break;

		case DESCRIPTOR_INTERFACE:
			if (ucLastDescType == 0)
			{
				pThis->m_pErrorPosition = pCurrentPosition;
				return;
			}
			ucExpectedLen = sizeof (TUSBInterfaceDescriptor);
			bInAudioInterface = pCurrentPosition->Interface.bInterfaceClass == 0x01; // Audio class
			break;

		case DESCRIPTOR_ENDPOINT:
			if (   ucLastDescType == 0
			    || ucLastDescType == DESCRIPTOR_CONFIGURATION)
			{
				pThis->m_pErrorPosition = pCurrentPosition;
				return;
			}
			ucExpectedLen = bInAudioInterface ? sizeof (TUSBAudioEndpointDescriptor) : sizeof (TUSBEndpointDescriptor);
			break;

		default:
			break;
		}

		if (   ucExpectedLen != 0
		    && ucDescLen != ucExpectedLen)
		{
			pThis->m_pErrorPosition = pCurrentPosition;
			return;
		}

		ucLastDescType = ucDescType;
		pCurrentPosition = pDescEnd;
	}

	if (pCurrentPosition != pThis->m_pEndPosition)
	{
		pThis->m_pErrorPosition = pCurrentPosition;
		return;
	}

	pThis->m_bValid = TRUE;
}

void USBConfigurationParserCopy (TUSBConfigurationParser *pThis, TUSBConfigurationParser *pParser)
{
	assert (pThis != 0);
	assert (pParser != 0);

	pThis->m_pBuffer	    = pParser->m_pBuffer;
	pThis->m_nBufLen	    = pParser->m_nBufLen;
	pThis->m_bValid	     	    = pParser->m_bValid;
	pThis->m_pEndPosition	    = pParser->m_pEndPosition;
	pThis->m_pNextPosition	    = pParser->m_pNextPosition;
	pThis->m_pCurrentDescriptor = pParser->m_pCurrentDescriptor;
	pThis->m_pErrorPosition     = pParser->m_pErrorPosition;
}

void _USBConfigurationParser (TUSBConfigurationParser *pThis)
{
	assert (pThis != 0);
	pThis->m_pBuffer = 0;
}

boolean USBConfigurationParserIsValid (TUSBConfigurationParser *pThis)
{
	assert (pThis != 0);
	return pThis->m_bValid;
}

const TUSBDescriptor *USBConfigurationParserGetDescriptor (TUSBConfigurationParser *pThis, u8 ucType)
{
	assert (pThis != 0);
	assert (pThis->m_bValid);

	const TUSBDescriptor *pResult = 0;
	
	while (pThis->m_pNextPosition < pThis->m_pEndPosition)
	{
		u8 ucDescLen  = pThis->m_pNextPosition->Header.bLength;
		u8 ucDescType = pThis->m_pNextPosition->Header.bDescriptorType;

		TUSBDescriptor *pDescEnd = SKIP_BYTES (pThis->m_pNextPosition, ucDescLen);
		assert (pDescEnd <= pThis->m_pEndPosition);

		if (   ucType     == DESCRIPTOR_ENDPOINT
		    && ucDescType == DESCRIPTOR_INTERFACE)
		{
			break;
		}

		if (ucDescType == ucType)
		{
			pResult = pThis->m_pNextPosition;
			pThis->m_pNextPosition = pDescEnd;
			break;
		}

		pThis->m_pNextPosition = pDescEnd;
	}

	if (pResult != 0)
	{
		pThis->m_pErrorPosition = pResult;
	}

	pThis->m_pCurrentDescriptor = pResult;

	return pResult;
}

const TUSBDescriptor *USBConfigurationParserGetCurrentDescriptor (TUSBConfigurationParser *pThis)
{
	assert (pThis->m_bValid);
	assert (pThis->m_pCurrentDescriptor != 0);

	return pThis->m_pCurrentDescriptor;
}

void USBConfigurationParserError (TUSBConfigurationParser *pThis, const char *pSource)
{
	assert (pThis != 0);
	assert (pSource != 0);
	LogWrite (pSource, LOG_ERROR,
		     "Invalid configuration descriptor (offset 0x%X)",
		     (unsigned) ((uintptr) pThis->m_pErrorPosition - (uintptr) pThis->m_pBuffer));
#ifndef NDEBUG
	DebugHexdump (pThis->m_pBuffer, pThis->m_nBufLen, pSource);
#endif
}
