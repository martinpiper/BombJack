#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "gfx.h"
}
#include "DisplayBombJack.h"

class DisplayBombJack;

bool MemoryBus::addressLower8KActive(int addressEx) {
	return addressEx < 0x2000;
}

bool MemoryBus::addressActive(int addressEx, int selector) {
	if ((addressEx & selector) > 0) {
		return true;
	}
	return false;
}
	
bool MemoryBus::addressActive(long addressEx, long selector) {
	if ((addressEx & selector) > 0) {
		return true;
	}
	return false;
}

void MemoryBus::ageContention() {
	if (memoryAsserted) {
		return;
	}
	if (busContention > 0) {
		busContention--;
	}
}

bool MemoryBus::hasContention() {
	if (memoryAsserted) {
		return true;
	}
	if (busContention > 0) {
		return true;
	}
	return false;
}

void MemoryBus::writeData(int address, int addressEx, int data) {
	writeData(address, addressEx, (unsigned char) data);
}

//virtual void MemoryBus::writeData(int address, int addressEx, unsigned char data);

//virtual void MemoryBus::setAddressBus(int address, int addressEx);

int MemoryBus::getByteOrContention(int value) {
	sequentialValue++;
	if (busContention > 0) {
		return value ^ sequentialValue;
	}
	return value;
}

bool MemoryBus::extEXTWANTIRQ() {
	return false;
}

void MemoryBus::resetExtEXTWANTIRQ() {
}

//void UserPortTo24BitAddress::setDisplay(DisplayBombJack *);
//void UserPortTo24BitAddress::writeData(int address, int addressEx, unsigned char data);
//void UserPortTo24BitAddress::calculatePixel();

//void DisplayLayer::setDisplay(DisplayBombJack *);
//void DisplayLayer::writeData(int address, int addressEx, unsigned char data);
//int DisplayLayer::calculatePixel(int displayH, int displayV, bool _hSync, bool _vSync);

DisplayBombJack::DisplayBombJack() {
}

int DisplayBombJack::getFrameNumberForSync() {
	return frameNumberForSync;
}

int DisplayBombJack::getDisplayH() {
	return displayH;
}

int DisplayBombJack::getDisplayV() {
	return displayV;
}

int DisplayBombJack::getDisplayX(int cia1RasterOffsetX) {
	return displayX + cia1RasterOffsetX;
}

int DisplayBombJack::getDisplayYForCIA(int cia1RasterOffsetY) {
	return ((displayHeight - (displayY + cia1RasterOffsetY)) - 16) & 0x1ff;   // These tweak values are aligned with using Bus24Bit_StartRasterTimers immediately after using Bus24Bit_WaitVBlank
}

bool DisplayBombJack::is_hSync() {
	return _hSync;
}

bool DisplayBombJack::is_vSync() {
	return _vSync;
}

bool DisplayBombJack::getVSync() {
	return _vSync;
}
bool DisplayBombJack::getVBlank() {
	return vBlank;
}

bool DisplayBombJack::extEXTWANTIRQ() {
	return extEXTWANTIRQFlag;
}

void DisplayBombJack::resetExtEXTWANTIRQ() {
	extEXTWANTIRQFlag = false;
}

void DisplayBombJack::make16Colours() {
	is16Colours = true;
}

void DisplayBombJack::setCallbackAPU(UserPortTo24BitAddress *apu) {
	callbackAPU = apu;
}

int DisplayBombJack::getBusContentionPixels() {
	return 0x08;
}

void DisplayBombJack::addLayer(DisplayLayer *layer) {
	layer->setDisplay(this);
	layersRaw[layerIndex] = layer;
	layerIndex++;
}

void DisplayBombJack::writeData(int address, int addressEx, unsigned char data) {

	lastDataWritten = data;
	if (MemoryBus::addressActive(addressEx, addressExPalette) && address >= addressPalette && address < (addressPalette + 0x200)) {
		busContentionPalette = getBusContentionPixels();
		paletteMem[address & 0x1ff] = data;

		int index = (address & 0x1ff) >> 1;
		palette[index] = ((paletteMem[index << 1] &0x0f) << 1) | ((paletteMem[index << 1] &0xf0) << 2) | ((paletteMem[(index << 1)+1] &0x0f) << 1);
	}

	// This logic now exists on the video layer hardware
	if (MemoryBus::addressActive(addressEx, addressExRegisters) && address == addressRegisters) {
		if ((data & 0x20) > 0) {
			enableDisplay = true;
		} else {
			enableDisplay = false;
		}
		if ((data & 0x80) > 0) {
			borderY = true;
		} else {
			borderY = false;
		}
		if ((data & 0x40) > 0) {
			borderX = true;
		} else {
			borderX = false;
		}
	}

	if (MemoryBus::addressActive(addressEx, addressExRegisters) && address == addressRegisters + 0x08) {
		displayPriority = data;
	}

	// Handle other layer writes
	for (int i = 0 ; i < 4 ; i++) {
		DisplayLayer *layer = layersRaw[i];
		if (layer != 0) {
			layer->writeData(address, addressEx, data);
		}
	}
}

void DisplayBombJack::calculateAFrame() {
	pixelAddr = gfx_get_pixel_addr(0,0);
	for (int i = 0 ; i < pixelsInWholeFrame() ; i++) {
		calculatePixel();
	}
}

int DisplayBombJack::pixelsInWholeFrame() {
	return displayWidth*displayHeight;
}

void DisplayBombJack::calculatePixel() {
	pixelsSinceLastDebugWrite++;
	_hSync = true;
	_vSync = true;

	if (displayX >= 0 && displayX < 0x80) {
		displayH = 0x180 + displayX;
	} else {
		displayH = displayX - 0x80;
	}
	if (displayH >= 0x1b0 && displayH < 0x1d0) {
		_hSync = false;
	}
	// Positive edge, new video scan line
	if (displayH == 0x1cf) {
		displayBitmapX = 0;
		displayBitmapY++;
		pixelAddr = gfx_get_pixel_addr(0,displayBitmapY);
	}

	if (displayY >= 0 && displayY < 0x08) {
		displayV = 0xf8 + displayY;
		_vSync = false;
	} else {
		displayV = displayY - 0x08;
	}

	if (callbackAPU != 0) {
		callbackAPU->calculatePixel();
	}

	// Save the frame
	if (displayX == 0 && displayY == 0) {
		frameNumberForSync++;
		pixelsSinceLastDebugWrite = 0;
	}
	if (displayX == 0 && displayY == (displayHeight-1)) {
		// Debug frame write
	}

	// One pixel delay from U95:A
	enablePixels = true;
	if (borderX && (displayH >= 0xfe/*0x181*/)) {
		enablePixels = false;
	}
	if (!borderX && (displayH >= 0x188/*0x189*/)) {
		enablePixels = false;
	}
	if (borderX && (displayH < 0x00d)) {
		enablePixels = false;
	}
	if (!borderX && (displayH < 0x009)) {
		enablePixels = false;
	}

	if (borderY && (displayV >= 0xe0)) {
		enablePixels = false;
	}

	vBlank = false;
	if (displayV < 0x10 || displayV >= 0xf0) {
		vBlank = true;
	}
	if (displayH == 0x180 && displayV == 0xf0) {
		extEXTWANTIRQFlag = true;
	}

	if (vBlank /*|| (displayH & 256) == 256*/) {
		enablePixels = false;
	}
	// The hardware syncs on -ve _VBLANK
	if (displayX == 0 && displayV == 0xf0) {
		// vsync triggered
	}


	if (enableDisplay) {
		int tempy = displayBitmapY;
		// Make sure the rendering position is in the screen
		if (displayBitmapX >= 0 && tempy >= 0 && displayBitmapX < displayWidth && tempy < displayHeight) {
			// Delayed due to pixel latching in the output mixer 8B2 and 7A2
			if (enablePixels) {
				if (busContentionPalette > 0) {
					latchedPixel = getContentionColouredPixel();
				}
				int realColour = palette[latchedPixel & 0xff];
				//panel.fastSetRGB(displayBitmapX, tempy, realColour);
				*pixelAddr++ = (unsigned char) realColour;
				*pixelAddr++ = (unsigned char) (realColour >> 8);
			} else {
				//panel.fastSetRGB(displayBitmapX, tempy, 0);
				*pixelAddr++ = 0;
				*pixelAddr++ = 0;
			}
		}
	}

	// Calculate any pixels in the layers
	int cachedPixel[4];
	for (int i = 0 ; i < 4 ; i++) {
		DisplayLayer *displayLayer = layersRaw[i];
		if (displayLayer != 0) {
			int pixel = displayLayer->calculatePixel(displayH, displayV, _hSync, _vSync);
			cachedPixel[i] = pixel;
			displayLayer->ageContention();
		} else {
			cachedPixel[i] = 0xff;
		}
	}

	// Calculate output pixel based on layer order
	latchedPixel = 0;
	bool firstLayer = true;
	for (int i = 3 ; i >= 0 ; i--) {
		int theLayer = (displayPriority >> (i*2)) & 0x03;
		theLayer = 3 - theLayer;
		int pixel = cachedPixel[theLayer];
		if (is16Colours) {
			if ((pixel & 0x0f) != 0 || firstLayer) {
				latchedPixel = pixel;
			}
		} else {
			if ((pixel & 0x07) != 0 || firstLayer) {
				latchedPixel = pixel;
			}
		}
		firstLayer = false;
	}

	displayX++;
	displayBitmapX++;
	if (displayX >= displayWidth) {
		displayX = 0;
		displayY++;
	}
	if (displayY >= displayHeight) {
		displayY = 0;
		enablePixels = false;
		displayBitmapX = 0;
		displayBitmapY = 0;
		pixelAddr = gfx_get_pixel_addr(0,0);
	}
	if (busContentionPalette > 0) {
		busContentionPalette--;
	}
}

int DisplayBombJack::getContentionColouredPixel() {
	// Introduce some colour and pattern using the part of last byte written to simulate a bus with contention
	sequentialValue++;
	if ((sequentialValue & 1) > 0) {
		return (sequentialValue & 0x0f) | (lastDataWritten & 0xf0);
	}
	return lastDataWritten;
}
