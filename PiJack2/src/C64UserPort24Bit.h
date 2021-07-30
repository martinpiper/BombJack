#ifndef C64USERPORT24BIT_H__
#define C64USERPORT24BIT_H__

extern unsigned char C64UserPort24Bit_init(void);

// Used for debugging
extern void C64UserPort24Bit_addNext(unsigned int theTime , int theValue);

extern int C64UserPort24Bit_getNext(unsigned int startTimeWindow , unsigned int lowTimeFilter , unsigned int highTimeFilter , int totalPixels , int *nextPixelPos);

extern int C64UserPort24Bit_setVSync(int vsync);

#endif
