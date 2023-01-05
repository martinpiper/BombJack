#include "gpio.h"
#include "irq.h"
#include "ee_printf.h"
#include "timer.h"
#include "utils.h"
#include "synchronize.h"
#include "config.h"
#include "C64UserPort24Bit.h"

// When defined, just returns raw bytes, no address or value time filtering
#define SIMPLE_MODE

// https://www.c64-wiki.com/wiki/User_Port
// Broadcom GPIO numbers, *not* header pin numbers

// Level shifter IO1 to IO8
#define	PIN_DATA0		17
#define	PIN_DATA1		18
#define	PIN_DATA2		27
#define	PIN_DATA3		22
#define	PIN_DATA4		23
#define	PIN_DATA5		24
#define	PIN_DATA6		25
#define	PIN_DATA7		4

// Level shifter IO1 to IO8
#define	PIN_PA2			7
#define	PIN_PC2			8
#define	PIN_FLAG2		9	// Output
#define	PIN_SERIALATN	5
#define	PIN_SP1			6
#define	PIN_SP2			12



#define	BUFFERSIZE	(1<<16)
static unsigned int volatile gotBytes[BUFFERSIZE];
static unsigned int volatile gotBytesTime[BUFFERSIZE];
static volatile int gotBytesCount = 0;
static volatile int gotBytesCountDisplay = 0;
static volatile int interfaceState = 0;
static volatile unsigned int interfaceStateEBBS = 0;
static volatile unsigned int interfaceStateAddress = 0;
static unsigned char *largeData = 0;

extern unsigned char _binary_binaryData_bin_start;
extern unsigned char _binary_binaryData_bin_end;
extern unsigned char _binary_binaryData_bin_size;
extern unsigned char _binary_smp_raw_start;
extern unsigned char _binary_smp_raw_end;
extern unsigned char _binary_smp_raw_size;


unsigned char *real_binary_binaryData_bin_start;
unsigned char *real_binary_binaryData_bin_end;
unsigned int real_binary_binaryData_bin_size;
unsigned char *real_binary_smp_raw_start;
unsigned char *real_binary_smp_raw_end;
unsigned int real_binary_smp_raw_size;

unsigned char *real_current_bitmap;
unsigned char *real_current_audio;



static void onPC2Fall(void)
{
	unsigned int value = 0;
	int allBits = gpio_get0to31();
	if (allBits & (1<<PIN_DATA0))
	{
		value |= 1 << 0;
	}
	if (allBits & (1<<PIN_DATA1))
	{
		value |= 1 << 1;
	}
	if (allBits & (1<<PIN_DATA2))
	{
		value |= 1 << 2;
	}
	if (allBits & (1<<PIN_DATA3))
	{
		value |= 1 << 3;
	}
	if (allBits & (1<<PIN_DATA4))
	{
		value |= 1 << 4;
	}
	if (allBits & (1<<PIN_DATA5))
	{
		value |= 1 << 5;
	}
	if (allBits & (1<<PIN_DATA6))
	{
		value |= 1 << 6;
	}
	if (allBits & (1<<PIN_DATA7))
	{
		value |= 1 << 7;
	}

#ifdef SIMPLE_MODE
	gotBytes[gotBytesCount] = value | (interfaceStateEBBS << 8) | ((interfaceStateAddress & 0xffff) << 16);
	gotBytesCount = (gotBytesCount+1) & (BUFFERSIZE-1);
#else
	switch(interfaceState)
	{
		case 0:
		default:
			interfaceStateEBBS = (unsigned int) value;
			interfaceState++;
			break;

		case 1:
			interfaceStateAddress = (unsigned int) value;
			interfaceState++;
			break;

		case 2:
			interfaceStateAddress |= (unsigned int) value << 8;
			interfaceState++;
			break;

		case 3:
			// Ordering matches the 32 bits used in the schematic
			gotBytes[gotBytesCount] = value | (interfaceStateEBBS << 8) | ((interfaceStateAddress & 0xffff) << 16);
			gotBytesTime[gotBytesCount] = time_microsec();

			gotBytesCount = (gotBytesCount+1) & (BUFFERSIZE-1);
			
			interfaceStateAddress++;
			break;
	}
#endif
}

static void setupOutput(void)
{
	unsigned char value = *real_current_bitmap;
	
	gpio_set(PIN_DATA0 , value & 0x01);
	gpio_set(PIN_DATA1 , value & 0x02);
	gpio_set(PIN_DATA2 , value & 0x04);
	gpio_set(PIN_DATA3 , value & 0x08);
	gpio_set(PIN_DATA4 , value & 0x10);
	gpio_set(PIN_DATA5 , value & 0x20);
	gpio_set(PIN_DATA6 , value & 0x40);
	gpio_set(PIN_DATA7 , value & 0x80);
}

static void setupOutputAudio(void)
{
	unsigned char value = *real_current_audio;
	
	gpio_set(PIN_DATA0 , value & 0x01);
	gpio_set(PIN_DATA1 , value & 0x02);
	gpio_set(PIN_DATA2 , value & 0x04);
	gpio_set(PIN_DATA3 , value & 0x08);
	gpio_set(PIN_DATA4 , value & 0x10);
	gpio_set(PIN_DATA5 , value & 0x20);
	gpio_set(PIN_DATA6 , value & 0x40);
	gpio_set(PIN_DATA7 , value & 0x80);
}

static int sDataCount = 0;
static int sDataCount1 = 0;
static int sDataCount2 = 0;
static int doAudioFor = 0;

static void onNotPA2(void)
{
	// Clear internal state
	interfaceState = 0;
	interfaceStateEBBS = 0;
	interfaceStateAddress = 0;
	sDataCount = 0;
	sDataCount1 = 0;
	sDataCount2 = 0;
	doAudioFor = 0;
	
	real_current_bitmap = real_binary_binaryData_bin_start;
	real_current_audio = real_binary_smp_raw_start;
}

static void onGenericStream(void)
{
	int allBits = gpio_get0to31();
}

static void onPC2Stream(void)
{
	int allBits = gpio_get0to31();
	
	if (!(allBits & (1<<PIN_PA2)))
	{
		onNotPA2();
	}
	else
	{
//		if (!(allBits & (1<<PIN_SERIALATN)))
		
		if (doAudioFor > 0)
		{
			setupOutputAudio();

			real_current_audio++;
			doAudioFor--;
			if (real_current_audio >= real_binary_smp_raw_end)
			{
				real_current_audio = real_binary_smp_raw_start;
			}
		}
		else
		{
			setupOutput();

			if (sDataCount >= 0 && sDataCount < (22*16*8))
			{
				sDataCount++;
				sDataCount1++;
				real_current_bitmap++;
				if ( sDataCount1 >= 16 )
				{
					doAudioFor = 1;
					sDataCount1 = 0;
				}
			}
			else if (sDataCount >= (22*16*8) && sDataCount < (22*16*8) + (22*16*2))
			{
				sDataCount++;
				sDataCount2++;
				real_current_bitmap++;
				if ( sDataCount2 >= 24 )
				{
					doAudioFor = 1;
					sDataCount2 = 0;
				}
			}
			else
			{
				sDataCount = 0;
				sDataCount1 = 0;
				sDataCount2 = 0;
				doAudioFor = 23;	// ????!!!!
			}

			if (real_current_bitmap >= real_binary_binaryData_bin_end)
			{
				real_current_bitmap = real_binary_binaryData_bin_start;
				sDataCount = 0;
			}
		}
	}
	
	// Fake some output activity
//	gotBytes[gotBytesCount] = *real_current_bitmap;
//	gotBytesCount = (gotBytesCount+1) & (BUFFERSIZE-1);
}

unsigned char C64UserPort24Bit_init(void)
{
	real_binary_binaryData_bin_start = &_binary_binaryData_bin_start;
	real_binary_binaryData_bin_end = &_binary_binaryData_bin_end;
	real_binary_binaryData_bin_size = &_binary_binaryData_bin_size;
	real_binary_smp_raw_start = &_binary_smp_raw_start;
	real_binary_smp_raw_end = &_binary_smp_raw_end;
	real_binary_smp_raw_size = &_binary_smp_raw_size;

	onNotPA2();
//	real_current_bitmap = real_binary_binaryData_bin_start;
//	real_current_audio = real_binary_smp_raw_start;
	ee_printf("[C64UserPort24Bit] Large data addr $%x size %d\n" , (int)real_binary_binaryData_bin_start , real_binary_binaryData_bin_size);
	ee_printf("[C64UserPort24Bit] smp data addr $%x size %d\n" , (int)real_binary_smp_raw_start , real_binary_smp_raw_size);


	gotBytesCount = 0;
	gotBytesCountDisplay = 0;
	interfaceState = 0;
	interfaceStateEBBS = 0;
	interfaceStateAddress = 0;

#if 0
    gpio_select(PIN_DATA0, GPIO_INPUT);
    gpio_select(PIN_DATA1, GPIO_INPUT);
    gpio_select(PIN_DATA2, GPIO_INPUT);
    gpio_select(PIN_DATA3, GPIO_INPUT);
    gpio_select(PIN_DATA4, GPIO_INPUT);
    gpio_select(PIN_DATA5, GPIO_INPUT);
    gpio_select(PIN_DATA6, GPIO_INPUT);
    gpio_select(PIN_DATA7, GPIO_INPUT);
#else
    gpio_select(PIN_DATA0, GPIO_OUTPUT);
    gpio_select(PIN_DATA1, GPIO_OUTPUT);
    gpio_select(PIN_DATA2, GPIO_OUTPUT);
    gpio_select(PIN_DATA3, GPIO_OUTPUT);
    gpio_select(PIN_DATA4, GPIO_OUTPUT);
    gpio_select(PIN_DATA5, GPIO_OUTPUT);
    gpio_select(PIN_DATA6, GPIO_OUTPUT);
    gpio_select(PIN_DATA7, GPIO_OUTPUT);
#endif

    gpio_select(PIN_PA2, GPIO_INPUT);
    gpio_select(PIN_PC2, GPIO_INPUT);

    gpio_select(PIN_SERIALATN, GPIO_INPUT);
    gpio_select(PIN_SP1, GPIO_INPUT);
    gpio_select(PIN_SP2, GPIO_INPUT);

    gpio_select(PIN_FLAG2, GPIO_OUTPUT);

    gpio_setpull(PIN_DATA0, GPIO_PULL_OFF);
    gpio_setpull(PIN_DATA1, GPIO_PULL_OFF);
    gpio_setpull(PIN_DATA2, GPIO_PULL_OFF);
    gpio_setpull(PIN_DATA3, GPIO_PULL_OFF);
    gpio_setpull(PIN_DATA4, GPIO_PULL_OFF);
    gpio_setpull(PIN_DATA5, GPIO_PULL_OFF);
    gpio_setpull(PIN_DATA6, GPIO_PULL_OFF);
    gpio_setpull(PIN_DATA7, GPIO_PULL_OFF);

    gpio_setpull(PIN_PA2, GPIO_PULL_OFF);
    gpio_setpull(PIN_PC2, GPIO_PULL_OFF);

    gpio_setpull(PIN_SERIALATN, GPIO_PULL_OFF);
    gpio_setpull(PIN_SP1, GPIO_PULL_OFF);
    gpio_setpull(PIN_SP2, GPIO_PULL_OFF);

    gpio_setpull(PIN_FLAG2, GPIO_PULL_OFF);

//    gpio_setedgedetect(PIN_PA2, GPIO_EDGE_DETECT_FALLING);
//    fiq_attach_gpio_handler(PIN_PA2, onPA2);

//    gpio_setedgedetect(PIN_PC2, GPIO_EDGE_DETECT_FALLING);
//    fiq_attach_gpio_handler(PIN_PC2, onPC2Fall);

//    gpio_setedgedetect(PIN_PC2, GPIO_EDGE_DETECT_RISING);
    gpio_setedgedetect(PIN_PC2, GPIO_EDGE_DETECT_FALLING);
    fiq_attach_gpio_handler(PIN_PC2, onPC2Stream);

//    gpio_setedgedetect(PIN_SERIALATN, GPIO_EDGE_DETECT_FALLING);
//    fiq_attach_gpio_handler(PIN_SERIALATN, onGenericStream);

	
	setupOutput();
	
	ee_printf("[C64UserPort24Bit] Initialised\n");
//	largeData = lookForLargedata("mpdata1.bin");
//	if (largeData != 0)
//	{
//		ee_printf("[C64UserPort24Bit] Got large data\n");
//	}

	return 0;
}

// Used for debugging
void C64UserPort24Bit_addNext(unsigned int theTime , unsigned int theValue)
{
	gotBytes[gotBytesCount] = theValue;
	gotBytesTime[gotBytesCount] = theTime;

	gotBytesCount = (gotBytesCount+1) & (BUFFERSIZE-1);
}

unsigned int C64UserPort24Bit_getNext(unsigned int startTimeWindow , unsigned int lowTimeFilter , unsigned int highTimeFilter , int totalPixels , int *nextPixelPos)
{
	// Don't return anything if there isn't any value pending
	if (gotBytesCount == gotBytesCountDisplay)
	{
#ifdef SIMPLE_MODE
		return 0;
#else
		if (nextPixelPos != 0)
		{
			*nextPixelPos = totalPixels;
		}
		return 0;
#endif
	}
	
#ifdef SIMPLE_MODE
	unsigned int temp = gotBytes[gotBytesCountDisplay];
	gotBytesCountDisplay = (gotBytesCountDisplay+1) & (BUFFERSIZE-1);

	return temp;
#else
	// Retire any early values in the time window
	while ( (gotBytesCount != gotBytesCountDisplay) && (gotBytesTime[gotBytesCountDisplay] - startTimeWindow) < lowTimeFilter )
	{
		gotBytesCountDisplay = (gotBytesCountDisplay+1) & (BUFFERSIZE-1);
	}

	// Don't return anything if there isn't any value pending
	if (gotBytesCount == gotBytesCountDisplay)
	{
		*nextPixelPos = totalPixels;
		return 0;
	}

	// Ignore values beyond the maximum time window
	if ( (gotBytesTime[gotBytesCountDisplay] - startTimeWindow) >= highTimeFilter )
	{
		*nextPixelPos = totalPixels;
		return 0;
	}

	// Now should be left with only values we want
	unsigned int temp = gotBytes[gotBytesCountDisplay];
	*nextPixelPos = (((gotBytesTime[gotBytesCountDisplay] - startTimeWindow) - lowTimeFilter) * totalPixels) / (highTimeFilter - lowTimeFilter);
	gotBytesCountDisplay = (gotBytesCountDisplay+1) & (BUFFERSIZE-1);

//	ee_printf("[C64UserPort24Bit] 0x%02x\n" , temp);

	return temp;
#endif

}

static int lastVSync = 0;
int C64UserPort24Bit_setVSync(int vsync)
{
	if (vsync == lastVSync)
	{
		return 0;
	}
	
	lastVSync = vsync;	
	gpio_set(PIN_FLAG2 , vsync);
	
	return 1;
}

