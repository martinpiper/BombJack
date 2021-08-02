#ifndef __DISPLAYBOMBJACK_H
#define __DISPLAYBOMBJACK_H

class DisplayBombJack;

class MemoryBus {
public:
    int busContention = 0;
    bool memoryAsserted = false;

    static bool addressLower8KActive(int addressEx);

	static bool addressActive(int addressEx, int selector);
	
    static bool addressActive(long addressEx, long selector);

    void ageContention();

    bool hasContention();

    virtual void writeData(int address, int addressEx, int data);

    virtual void writeData(int address, int addressEx, unsigned char data);

    virtual void setAddressBus(int address, int addressEx);

    int sequentialValue = 0;
    int getByteOrContention(int value);

    bool extEXTWANTIRQ();

    void resetExtEXTWANTIRQ();
};

class UserPortTo24BitAddress {
public:
	void setDisplay(DisplayBombJack *);
	virtual void writeData(int address, int addressEx, unsigned char data);
	void calculatePixel();
};

class DisplayLayer : public MemoryBus {
public:
	void setDisplay(DisplayBombJack *);
	virtual void writeData(int address, int addressEx, unsigned char data);
	virtual int calculatePixel(int displayH, int displayV, bool _hSync, bool _vSync);
};

class DisplayBombJack : public MemoryBus {

public:
    DisplayBombJack();
	
	void init(void);

    DisplayLayer *layersRaw[4] = {0,0,0,0};

    int frameNumber = 0;
//    int displayWidth = 384;
//    int displayHeight = 264;
    int busContentionPalette = 0;
    int addressPalette = 0x9c00, addressExPalette = 0x01;
    int addressRegisters = 0x9e00, addressExRegisters = 0x01;
    int displayPriority = 0;  // Default to be 0, this helps ensure startup code correctly sets this option

    int getFrameNumberForSync();

    int frameNumberForSync = 0;

    int getDisplayH();

    int getDisplayV();

    int getDisplayX(int cia1RasterOffsetX);

    int getDisplayYForCIA(int cia1RasterOffsetY);

    bool is_hSync();

    bool is_vSync();

    int displayH = 0, displayV = 0;
    int displayX = 0, displayY = 0xf8;	// Start when the vsync negative edge happens
    int displayBitmapX = 0, displayBitmapY = 0xf8;	// Start when the vsync negative edge happens
    bool enablePixels = false;
    bool borderX = true, borderY = true;
    bool enableDisplay = false;  // Default to be display off, this helps ensure startup code correctly sets this option
    int latchedPixel = 0;
    int paletteMem[512];
    int palette[256];
    int lastDataWritten = 0;
    bool vBlank = false;
    bool _hSync = true, _vSync = true;
    bool extEXTWANTIRQFlag = false;
//    int pixelsSinceLastDebugWrite = 0;
//    int pixelsSinceLastDebugWriteMax = 32;
    bool is16Colours = false;
    UserPortTo24BitAddress *callbackAPU = 0;

    bool getVSync();
    bool getVBlank();

    bool extEXTWANTIRQ();

    void resetExtEXTWANTIRQ();

    void make16Colours();

    void setCallbackAPU(UserPortTo24BitAddress *apu);

    int getBusContentionPixels();

	int layerIndex = 0;

    void addLayer(DisplayLayer *layer);

    virtual void writeData(int address, int addressEx, unsigned char data);

    void startCalculateAFrame();

    int pixelsInWholeFrame();

	unsigned char *pixelAddr = 0;

    void calculatePixel();

    int sequentialValue = 0;
    int getContentionColouredPixel();
};

#endif //< ifndef __DISPLAYBOMBJACK_H
