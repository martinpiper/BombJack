//
// exceptionhandler.c
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
#include <uspienv/exceptionhandler.h>
#include <uspienv/synchronize.h>
#include <uspienv/logger.h>
#include <uspienv/debug.h>
#include <uspienv/assert.h>

static const char FromExcept[] = "except";

// order must match exception identifiers in uspienv/exception.h
static const char *s_pExceptionName[] =
{
	"Division by zero",
	"Undefined instruction",
	"Prefetch abort",
	"Data abort"
};

static TExceptionHandler *s_pThis = 0;

void ExceptionHandler2 (TExceptionHandler *pThis)
{
	assert (pThis != 0);

	assert (s_pThis == 0);
	s_pThis = pThis;

	TExceptionTable *pTable = (TExceptionTable *) ARM_EXCEPTION_TABLE_BASE;

	pTable->UndefinedInstruction = ARM_OPCODE_BRANCH (ARM_DISTANCE (
					pTable->UndefinedInstruction, UndefinedInstructionStub));

	pTable->PrefetchAbort = ARM_OPCODE_BRANCH (ARM_DISTANCE (
					pTable->PrefetchAbort, PrefetchAbortStub));

	pTable->DataAbort = ARM_OPCODE_BRANCH (ARM_DISTANCE (pTable->DataAbort, DataAbortStub));

	CleanDataCache ();
	DataSyncBarrier ();

	InvalidateInstructionCache ();
	FlushBranchTargetCache ();
	DataSyncBarrier ();

	InstructionSyncBarrier ();
}

void _ExceptionHandler (TExceptionHandler *pThis)
{
	assert (pThis != 0);

	s_pThis = 0;
}

void ExceptionHandlerThrow (TExceptionHandler *pThis, unsigned nException)
{
	assert (pThis != 0);

	LoggerWrite (LoggerGet (), FromExcept, LogPanic, "Exception: %s", s_pExceptionName[nException]);
}

void ExceptionHandlerThrow2 (TExceptionHandler *pThis, unsigned nException, TAbortFrame *pFrame)
{
	assert (pThis != 0);

	u32 FSR = 0, FAR = 0;
	switch (nException)
	{
	case EXCEPTION_PREFETCH_ABORT:
		asm volatile ("mrc p15, 0, %0, c5, c0,  1" : "=r" (FSR));
		asm volatile ("mrc p15, 0, %0, c6, c0,  2" : "=r" (FAR));
		break;

	case EXCEPTION_DATA_ABORT:
		asm volatile ("mrc p15, 0, %0, c5, c0,  0" : "=r" (FSR));
		asm volatile ("mrc p15, 0, %0, c6, c0,  0" : "=r" (FAR));
		break;

	default:
		break;
	}

	assert (pFrame != 0);
	u32 lr = pFrame->lr;
	u32 sp = pFrame->sp;

	if ((pFrame->spsr & 0x1F) == 0x12)	// IRQ mode?
	{
		lr = pFrame->lr_irq;
		sp = pFrame->sp_irq;
	}
	
#ifndef NDEBUG
	debug_stacktrace ((u32 *) sp, FromExcept);
#endif

	LoggerWrite (LoggerGet (), FromExcept, LogPanic,
		"%s (PC 0x%X, FSR 0x%X, FAR 0x%X, SP 0x%X, LR 0x%X, PSR 0x%X)",
		s_pExceptionName[nException],
		pFrame->pc, FSR, FAR,
		sp, lr, pFrame->spsr);
}

TExceptionHandler *ExceptionHandlerGet (void)
{
	assert (s_pThis != 0);
	return s_pThis;
}

void ExceptionHandler (u32 nException, TAbortFrame *pFrame)
{
	DataMemBarrier ();

	ExceptionHandlerThrow2 (ExceptionHandlerGet (), nException, pFrame);
}
