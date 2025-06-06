#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
//#include <wiringPi.h>
#include "WiringPi-master/wiringPi/wiringPi.h"



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


// setup pins using the gpio utility so that we can execute the blink program without sudo
// gpio export 17 in && gpio export 18 in && gpio export 27 in && gpio export 22 in && gpio export 23 in && gpio export 24 in && gpio export 25 in && gpio export 4 in && gpio edge 7 rising && gpio edge 8 rising && gpio export 9 out

static volatile int sGot = 0;

static unsigned char volatile gotBytes[1024];
static volatile int gotBytesCount = 0;
static volatile int gotBytesCountDisplay = 0;

void onPC2(void)
{
	int value = digitalReadFromCache(PIN_DATA0) | (digitalReadFromCache(PIN_DATA1) << 1) | (digitalReadFromCache(PIN_DATA2) << 2) | (digitalReadFromCache(PIN_DATA3) << 3)
		| (digitalReadFromCache(PIN_DATA4) << 4) | (digitalReadFromCache(PIN_DATA5) << 5) | (digitalReadFromCache(PIN_DATA6) << 6) | (digitalReadFromCache(PIN_DATA7) << 7);
//	int value = digitalRead8(PIN_DATA0);
//	printf("%d onPC2 %x %d\n", sGot++, value, value);

	gotBytes[gotBytesCount] = (unsigned char) value;
	gotBytesCount = (gotBytesCount+1) & 1023;
}

void onPC2NotCached(void)
{
	int value = digitalRead(PIN_DATA0) | (digitalReadFromCache(PIN_DATA1) << 1) | (digitalReadFromCache(PIN_DATA2) << 2) | (digitalReadFromCache(PIN_DATA3) << 3)
		| (digitalReadFromCache(PIN_DATA4) << 4) | (digitalReadFromCache(PIN_DATA5) << 5) | (digitalReadFromCache(PIN_DATA6) << 6) | (digitalReadFromCache(PIN_DATA7) << 7);
	//	int value = digitalRead8(PIN_DATA0);
	//	printf("%d onPC2 %x %d\n", sGot++, value, value);

	gotBytes[gotBytesCount] = (unsigned char)value;
	gotBytesCount = (gotBytesCount + 1) & 1023;
}

PI_THREAD(myThread)
{
	while (true)
	{
		int value = digitalRead(PIN_PC2);
		while (value)
		{
			value = digitalRead(PIN_PC2);
		}
		// Falling edge here
		onPC2();
		while (!value)
		{
			value = digitalRead(PIN_PC2);
		}
		// Rising edge here
	}

}

void onPA2Falling(void)
{
//	printf("%d onPA2 falling\n", sGot++);
}

void onPA2Rising(void)
{
//	printf("%d onPA2 rising\n", sGot++);
}


int main(void)
{
	printf("Hello world 4\n");
//	wiringPiSetupSys();
	wiringPiSetupGpio();

	memset((void*)gotBytes, 0, sizeof(gotBytes));

	pinMode(PIN_DATA0, INPUT);
	pinMode(PIN_DATA1, INPUT);
	pinMode(PIN_DATA2, INPUT);
	pinMode(PIN_DATA3, INPUT);
	pinMode(PIN_DATA4, INPUT);
	pinMode(PIN_DATA5, INPUT);
	pinMode(PIN_DATA6, INPUT);
	pinMode(PIN_DATA7, INPUT);

	pinMode(PIN_PA2, INPUT);
//	wiringPiISR(PIN_PA2, INT_EDGE_FALLING, onPA2Falling);
//	wiringPiISR(PIN_PA2, INT_EDGE_RISING, onPA2Rising);
	pinMode(PIN_PC2, INPUT);
//	wiringPiISR(PIN_PC2, INT_EDGE_RISING, onPC2NotCached);
	piHiPri(99);
	piThreadCreate(myThread);

	pinMode(PIN_FLAG2, OUTPUT);

#if 0
	int fbfd = 0; // framebuffer filedescriptor
	struct fb_var_screeninfo var_info;

	// Open the framebuffer device file for reading and writing
	fbfd = open("/dev/fb0", O_RDWR);
	if (fbfd == -1) {
		printf("Error: cannot open framebuffer device.\n");
		return(1);
	}
	printf("The framebuffer device opened.\n");

	// Get variable screen information
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &var_info)) {
		printf("Error reading variable screen info.\n");
	}
	printf("Display info %dx%d, %d bpp\n",
		var_info.xres, var_info.yres,
		var_info.bits_per_pixel);

	int rgb = 0x0000ff00;
	int i;
	for (i = 0; i < 1000; i++)
	{
		write(fbfd, &rgb, sizeof(rgb));
	}

	// close file  
	close(fbfd);
#endif

	while (true)
	{
		digitalWrite(PIN_FLAG2, LOW);
		digitalWrite(PIN_FLAG2, HIGH);

		// Display any received data by processing the circular buffer here
		while (gotBytesCountDisplay != gotBytesCount)
		{
			printf("%d gotBytes %x %d\n", gotBytesCountDisplay, gotBytes[gotBytesCountDisplay], gotBytes[gotBytesCountDisplay]);
			gotBytesCountDisplay = (gotBytesCountDisplay + 1) & 1023;
		}
/*
		int value = digitalRead(PIN_DATA0) | (digitalRead(PIN_DATA1) << 1) | (digitalRead(PIN_DATA2) << 2) | (digitalRead(PIN_DATA3) << 3)
			| (digitalRead(PIN_DATA4) << 4) | (digitalRead(PIN_DATA5) << 5) | (digitalRead(PIN_DATA6) << 6) | (digitalRead(PIN_DATA7) << 7);
		printf("%x %d\n", value, value);
*/
		delay(100);
	}
	return 0;
}
