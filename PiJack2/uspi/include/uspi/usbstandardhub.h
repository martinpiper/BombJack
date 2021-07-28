//
// usbstandardhub.h
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
#ifndef _uspi_usbstandardhub_h
#define _uspi_usbstandardhub_h

#include <uspi/usb.h>
#include <uspi/usbhub.h>
#include <uspi/usbfunction.h>
#include <uspi/usbhostcontroller.h>
#include <uspi/string.h>
#include <uspi/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TUSBStandardHub
{
	TUSBFunction m_USBFunction;

	TUSBHubDescriptor *m_pHubDesc;

	unsigned m_nPorts;
	TUSBDevice *m_pDevice[USB_HUB_MAX_PORTS];
	TUSBPortStatus *m_pStatus[USB_HUB_MAX_PORTS];
}
TUSBStandardHub;

void USBStandardHub (TUSBStandardHub *pThis, TUSBFunction *pFunction);
void _USBStandardHub (TUSBStandardHub *pThis);

boolean USBStandardHubInitialize (TUSBStandardHub *pThis);
boolean USBStandardHubConfigure (TUSBFunction *pUSBFunction);

#ifdef __cplusplus
}
#endif

#endif
