//
// keyboard.c
// Keyboard input handling
//
// PiGFX is a bare metal kernel for the Raspberry Pi
// that implements a basic ANSI terminal emulator with
// the additional support of some primitive graphics functions.
// Copyright (C) 2020 Christian Lehner
// Most of this code is loosely taken from the USPI project

#include "keyboard.h"
#include "c_utils.h"
#include "config.h"
#include "uart.h"
#include "gfx.h"
#include "timer.h"
#include "ps2.h"



TKeyMap actKeyMap;
unsigned int backspace_n_skip;
unsigned int last_backspace_t;

void fSetKbdLeds(TKeyboardLeds* leds)
{
}

void fUpdateKeyboardLeds(unsigned char useUSB)
{
}

void fInitKeyboard(char* layout)
{
}

unsigned short ScancodeToKey (TKeyMap *pThis, unsigned char nPhyCode, unsigned char nModifiers)
{
    return 0;
}

const char *KeyToString (TKeyMap *pThis, unsigned short nKeyCode, unsigned char nModifiers, char Buffer[2])
{
    return 0;
}

void KeyEvent(unsigned short ucKeyCode, unsigned char ucModifiers)
{
}

void RepeatKey( unsigned hnd, void* pParam, void *pContext )
{
}

void KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6])  // key code or 0 in each byte
{
}

