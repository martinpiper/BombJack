#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>
#include "Common.h"

#define KEnableMultiplex

int main(int argc, char**argv)
{
	std::ofstream file;
	file.open(argv[1]);

	file << std::setfill('0');

	int frame = 0;
	const int maxFrames = 64;

	enum {
		kFrameManDown = 0,
		kFrameManLeft,
		kFrameManRight,
		kFrameManUp
	};

	const int kNumNPCs = 12;
	int npcXPos[kNumNPCs];
	int npcYPos[kNumNPCs];
	int npcFrame[kNumNPCs];
	int npcVelX[kNumNPCs];
	int npcVelY[kNumNPCs];

	// Init NPC animations
	int npcIndex = 0;
	npcXPos[npcIndex] = 64;
	npcYPos[npcIndex] = 200;
	npcFrame[npcIndex] = kFrameManUp;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = 1;
	npcIndex++;

	npcXPos[npcIndex] = 128;
	npcYPos[npcIndex] = 64;
	npcFrame[npcIndex] = kFrameManLeft;
	npcVelX[npcIndex] = -1;
	npcVelY[npcIndex] = 0;
	npcIndex++;

	npcXPos[npcIndex] = 128;
	npcYPos[npcIndex] = 200;
	npcFrame[npcIndex] = kFrameManRight;
	npcVelX[npcIndex] = 1;
	npcVelY[npcIndex] = 0;
	npcIndex++;

	npcXPos[npcIndex] = 64;
	npcYPos[npcIndex] = 80;
	npcFrame[npcIndex] = kFrameManRight;
	npcVelX[npcIndex] = 1;
	npcVelY[npcIndex] = 0;
	npcIndex++;

	npcXPos[npcIndex] = 64;
	npcYPos[npcIndex] = 48;
	npcFrame[npcIndex] = kFrameManUp;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = 1;
	npcIndex++;

	npcXPos[npcIndex] = 128;
	npcYPos[npcIndex] = 64;
	npcFrame[npcIndex] = kFrameManUp;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = 1;
	npcIndex++;

	npcXPos[npcIndex] = 128;
	npcYPos[npcIndex] = 200;
	npcFrame[npcIndex] = kFrameManDown;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = -1;
	npcIndex++;

	npcXPos[npcIndex] = 64;
	npcYPos[npcIndex] = 180;
	npcFrame[npcIndex] = kFrameManDown;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = -1;
	npcIndex++;

	npcXPos[npcIndex] = 164;
	npcYPos[npcIndex] = 28;
	npcFrame[npcIndex] = kFrameManUp;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = 1;
	npcIndex++;

	npcXPos[npcIndex] = 198;
	npcYPos[npcIndex] = 54;
	npcFrame[npcIndex] = kFrameManUp;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = 1;
	npcIndex++;

	npcXPos[npcIndex] = 18;
	npcYPos[npcIndex] = 220;
	npcFrame[npcIndex] = kFrameManDown;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = -1;
	npcIndex++;

	npcXPos[npcIndex] = 94;
	npcYPos[npcIndex] = 240;
	npcFrame[npcIndex] = kFrameManDown;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = -1;
	npcIndex++;

	assert(npcIndex == kNumNPCs);

	while (frame++ < maxFrames)
	{
		if (frame > 1)
		{
			WaitForVSync(file, frame);
		}

		EnableSpritesNoWait(file, '0');
#ifndef KEnableMultiplex
		SetSpriteAddress(file); 
#endif

		int lastSpriteBottomOutputYPos = 256;
		bool spritesDisabled = false;

		// Advance through the Y positions
		for (int ypos = 255; ypos >= 0; ypos--)
		{
			for (int i = 0; i < kNumNPCs; i++)
			{
				if (npcYPos[i] != ypos)
				{
					continue;
				}

#ifdef KEnableMultiplex
				// Check for triggering a fresh new sprite position underneath the current sprite and not within a few rasters to account for any updates
				// A simple (naive) multiplexor
				if (npcYPos[i] < lastSpriteBottomOutputYPos - 3)
				{
					if (lastSpriteBottomOutputYPos < 240)
					{
						// Check if the sprites need enabling
						if (spritesDisabled)
						{
							EnableSprites(file, '0');
							// Debug colour change
//							file << "d$9e030100" << std::endl;
						}
						// TODO: MPi: Investigate if the -14 here and -16 for "New lowest sprite Y pos" can be eliminated completely for 16x16 and 32x32 sprites
						// Perhaps the sprites are ending at the Y position, not starting from it?
						WaitForRaster(file, (255-lastSpriteBottomOutputYPos) - 14);	// Adjustment due to sprite position timing logic?
						DisableSprites(file, '0');
						spritesDisabled = true;
					}
					// Debug colour change
//					file << "d$9e030107" << std::endl;

					SetSpriteAddress(file);
				}
#endif

				// New lowest sprite Y pos
				lastSpriteBottomOutputYPos = npcYPos[i] - 16;

				file << std::dec;
				switch (npcFrame[i])
				{
				case kFrameManDown:
				default:
					file << "b3,b6,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
					file << "b4,b7,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
					file << "b5,b5,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
					break;
				case kFrameManLeft:
					file << "b6,b6,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
					file << "b7,b7,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
					file << "b8,b5,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
					break;
				case kFrameManRight:
					file << "b6,b$46,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
					file << "b7,b$47,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
					file << "b8,b$45,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
					break;
				case kFrameManUp:
					file << "b9,b6,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
					file << "b10,b7,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
					file << "b11,b5,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
					break;
				}
			}
		}
		file << "d$0" << std::endl; 


#ifdef KEnableMultiplex
		// Check if the sprites need enabling
		if (spritesDisabled)
		{
			EnableSprites(file, '0');
			// Debug colour change
//			file << "d$9e030100" << std::endl;
		}
#endif

		file << "d$0" << std::endl;
		file << std::endl;
		file << std::endl;

		for (int i = 0; i < kNumNPCs; i++)
		{
			npcXPos[i] += npcVelX[i];
			npcYPos[i] += npcVelY[i];
		}

		switch (frame)
		{
		case 16:
			npcVelX[0] = 1;
			npcVelY[0] = 0;
			npcFrame[0] = kFrameManRight;
			break;
		case 48:
			npcVelX[0] = 0;
			npcVelY[0] = -1;
			npcFrame[0] = kFrameManDown;
			break;
		default:
			break;
		}
	}

	file.close();
}
