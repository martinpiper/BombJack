//
// bcmpropertytags.h
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
#ifndef _bcmpropertytags_h
#define _bcmpropertytags_h

#include <uspienv/bcmmailbox.h>
#include <uspienv/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PROPTAG_END			0x00000000

#define PROPTAG_GET_FIRMWARE_REVISION	0x00000001
#define PROPTAG_GET_BOARD_MODEL		0x00010001
#define PROPTAG_GET_BOARD_REVISION	0x00010002
#define PROPTAG_GET_MAC_ADDRESS		0x00010003
#define PROPTAG_GET_BOARD_SERIAL	0x00010004
#define PROPTAG_GET_ARM_MEMORY		0x00010005
#define PROPTAG_GET_VC_MEMORY		0x00010006
#define PROPTAG_SET_POWER_STATE		0x00028001
#define PROPTAG_GET_CLOCK_RATE		0x00030002
#define PROPTAG_GET_TEMPERATURE		0x00030006
#define PROPTAG_GET_EDID_BLOCK		0x00030020
#define PROPTAG_GET_DISPLAY_DIMENSIONS	0x00040003
#define PROPTAG_GET_COMMAND_LINE	0x00050001

typedef struct TPropertyTag
{
	u32	nTagId;
	u32	nValueBufSize;			// bytes, multiple of 4
	u32	nValueLength;			// bytes
	#define VALUE_LENGTH_RESPONSE	(1 << 31)
	//u8	ValueBuffer[0];			// must be padded to be 4 byte aligned
}
TPropertyTag;

typedef struct TPropertyTagSimple
{
	TPropertyTag	Tag;
	u32		nValue;
}
TPropertyTagSimple;

typedef struct TPropertyTagMACAddress
{
	TPropertyTag	Tag;
	u8		Address[6];
	u8		Padding[2];
}
TPropertyTagMACAddress;

typedef struct TPropertyTagSerial
{
	TPropertyTag	Tag;
	u32		Serial[2];
}
TPropertyTagSerial;

typedef struct TPropertyTagMemory
{
	TPropertyTag	Tag;
	u32		nBaseAddress;
	u32		nSize;			// bytes
}
TPropertyTagMemory;

typedef struct TPropertyTagPowerState
{
	TPropertyTag	Tag;
	u32		nDeviceId;
	#define DEVICE_ID_SD_CARD	0
	#define DEVICE_ID_USB_HCD	3
	u32		nState;
	#define POWER_STATE_OFF		(0 << 0)
	#define POWER_STATE_ON		(1 << 0)
	#define POWER_STATE_WAIT	(1 << 1)
	#define POWER_STATE_NO_DEVICE	(1 << 1)	// in response
}
TPropertyTagPowerState;

typedef struct TPropertyTagClockRate
{
	TPropertyTag	Tag;
	u32		nClockId;
	#define CLOCK_ID_EMMC		1
	#define CLOCK_ID_UART		2
	u32		nRate;			// Hz
}
TPropertyTagClockRate;

typedef struct TPropertyTemperature
{
	TPropertyTag	Tag;
	u32		nTemperatureId;
	#define TEMPERATURE_ID		0
	u32		nValue;			// degree Celsius * 1000
}
TPropertyTemperature;

typedef struct TPropertyTagEDIDBlock
{
	TPropertyTag	Tag;
	u32		nBlockNumber;
	#define EDID_FIRST_BLOCK	0
	u32		nStatus;
	#define EDID_STATUS_SUCCESS	0
	u8		Block[128];
}
TPropertyTagEDIDBlock;

typedef struct TPropertyTagDisplayDimensions
{
	TPropertyTag	Tag;
	u32		nWidth;
	u32		nHeight;
}
TPropertyTagDisplayDimensions;

typedef struct TPropertyTagCommandLine
{
	TPropertyTag	Tag;
	u8		String[1024];
}
TPropertyTagCommandLine;

typedef struct TBcmPropertyTags
{
	TBcmMailBox m_MailBox;
}
TBcmPropertyTags;

void BcmPropertyTags (TBcmPropertyTags *pThis);
void _BcmPropertyTags (TBcmPropertyTags *pThis);

boolean BcmPropertyTagsGetTag (TBcmPropertyTags *pThis,
			       u32	  nTagId,			// tag identifier
			       void	 *pTag,				// pointer to tag struct
			       unsigned  nTagSize,			// size of tag struct
			       unsigned  nRequestParmSize /* = 0 */);	// number of parameter bytes

#ifdef __cplusplus
}
#endif

#endif
