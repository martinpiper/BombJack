#include "gpio.h"
#include "irq.h"
#include "ee_printf.h"
#include "timer.h"
#include "utils.h"
#include "synchronize.h"
#include "C64UserPort24Bit.h"

// https://www.c64-wiki.com/wiki/User_Port
// Broadcom GPIO numbers
// Inputs
#define	PIN_DATA0	17
#define	PIN_DATA1	18
#define	PIN_DATA2	27
#define	PIN_DATA3	22
#define	PIN_DATA4	23
#define	PIN_DATA5	24
#define	PIN_DATA6	25
#define	PIN_DATA7	4

#define	PIN_PA2		7
#define	PIN_PC2		8

// Outputs
#define	PIN_FLAG2	9

#define	BUFFERSIZE	(1<<16)
static unsigned int volatile gotBytes[BUFFERSIZE];
static unsigned int volatile gotBytesTime[BUFFERSIZE];
static volatile int gotBytesCount = 0;
static volatile int gotBytesCountDisplay = 0;
static volatile int interfaceState = 0;
static volatile unsigned int interfaceStateEBBS = 0;
static volatile unsigned int interfaceStateAddress = 0;

static void onPC2(void)
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

}

static void onPA2(void)
{
	// Clear internal state
	interfaceState = 0;
	interfaceStateEBBS = 0;
	interfaceStateAddress = 0;
}

unsigned char C64UserPort24Bit_init(void)
{
    gpio_select(PIN_DATA0, GPIO_INPUT);
    gpio_select(PIN_DATA1, GPIO_INPUT);
    gpio_select(PIN_DATA2, GPIO_INPUT);
    gpio_select(PIN_DATA3, GPIO_INPUT);
    gpio_select(PIN_DATA4, GPIO_INPUT);
    gpio_select(PIN_DATA5, GPIO_INPUT);
    gpio_select(PIN_DATA6, GPIO_INPUT);
    gpio_select(PIN_DATA7, GPIO_INPUT);

    gpio_select(PIN_PA2, GPIO_INPUT);
    gpio_select(PIN_PC2, GPIO_INPUT);

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

    gpio_setpull(PIN_FLAG2, GPIO_PULL_OFF);

//    gpio_setedgedetect(PIN_PA2, GPIO_EDGE_DETECT_FALLING);
//    fiq_attach_gpio_handler(PIN_PA2, onPA2);

    gpio_setedgedetect(PIN_PC2, GPIO_EDGE_DETECT_FALLING);
    fiq_attach_gpio_handler(PIN_PC2, onPC2);
	
	ee_printf("[C64UserPort24Bit] Initialised\n");
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
		return 0;
	}

	// Retire any early values in the time window
	while ( (gotBytesCount != gotBytesCountDisplay) && (gotBytesTime[gotBytesCountDisplay] - startTimeWindow) < lowTimeFilter )
	{
		gotBytesCountDisplay = (gotBytesCountDisplay+1) & (BUFFERSIZE-1);
	}

	// Don't return anything if there isn't any value pending
	if (gotBytesCount == gotBytesCountDisplay)
	{
		return 0;
	}

	// Ignore values beyond the maximum time window
	if ( (gotBytesTime[gotBytesCountDisplay] - startTimeWindow) >= highTimeFilter )
	{
		return 0;
	}

	// Now should be left with only values we want
	unsigned int temp = gotBytes[gotBytesCountDisplay];
	*nextPixelPos = (((gotBytesTime[gotBytesCountDisplay] - startTimeWindow) - lowTimeFilter) * totalPixels) / (highTimeFilter - lowTimeFilter);
	gotBytesCountDisplay = (gotBytesCountDisplay+1) & (BUFFERSIZE-1);

//	ee_printf("[C64UserPort24Bit] 0x%02x\n" , temp);

	return temp;
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

