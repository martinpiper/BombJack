//
// libhelper.cpp
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014-2015  R. Stange <rsta2@o2online.de>
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
#include <uspienv/assert.h>
#include <uspienv/exceptionhandler.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned Divide (unsigned nDividend, unsigned nDivisor, unsigned *pRest)
{
	if (nDivisor == 0)
	{
		assert (0);
		ExceptionHandlerThrow (ExceptionHandlerGet (), EXCEPTION_DIVISION_BY_ZERO);
	}
	
	unsigned long long ullDivisor = nDivisor;

	unsigned nCount = 1;
	while (nDividend > ullDivisor)
	{
		ullDivisor <<= 1;
		nCount++;
	}

	unsigned nQuotient = 0;
	while (nCount--)
	{
		nQuotient <<= 1;

		if (nDividend >= ullDivisor)
		{
			nQuotient |= 1;
			nDividend -= ullDivisor;
		}

		ullDivisor >>= 1;
	}
	
	if (pRest != 0)
	{
		*pRest = nDividend;
	}
	
	return nQuotient;
}

#ifdef __cplusplus
}
#endif
