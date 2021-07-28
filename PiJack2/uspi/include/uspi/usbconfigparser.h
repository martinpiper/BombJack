//
// usbconfigparser.h
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
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
#ifndef _uspi_usbconfigparser_h
#define _uspi_usbconfigparser_h

#include <uspi/usb.h>
#include <uspi/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TUSBConfigurationParser
{
	const TUSBDescriptor	*m_pBuffer;
	unsigned		 m_nBufLen;
	boolean			 m_bValid;
	const TUSBDescriptor	*m_pEndPosition;
	const TUSBDescriptor	*m_pNextPosition;
	const TUSBDescriptor	*m_pCurrentDescriptor;
	const TUSBDescriptor	*m_pErrorPosition;
}
TUSBConfigurationParser;

void USBConfigurationParser (TUSBConfigurationParser *pThis, const void *pBuffer, unsigned nBufLen);
void USBConfigurationParserCopy (TUSBConfigurationParser *pThis, TUSBConfigurationParser *pParser);
void _USBConfigurationParser (TUSBConfigurationParser *pThis);

boolean USBConfigurationParserIsValid (TUSBConfigurationParser *pThis);

const TUSBDescriptor *USBConfigurationParserGetDescriptor (TUSBConfigurationParser *pThis, u8 ucType);	// returns 0 if not found

const TUSBDescriptor *USBConfigurationParserGetCurrentDescriptor (TUSBConfigurationParser *pThis);

void USBConfigurationParserError (TUSBConfigurationParser *pThis, const char *pSource);

#ifdef __cplusplus
}
#endif

#endif
