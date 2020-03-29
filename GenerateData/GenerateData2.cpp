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
	const int maxFrames = 256;

	enum {
		kFramePersonDown = 0,
		kFramePersonLeft,
		kFramePersonRight,
		kFramePersonUp
	};
	enum {
		kNPCTypeMan = 0,
		kNPCTypeWoman,
		kNPCTypeGirl,
		kNPCTypeBoy
	};

	const int kNumNPCs = 14;
	// MPi: TODO: Move to class
	int npcXPos[kNumNPCs];
	int npcYPos[kNumNPCs];
	int npcFrame[kNumNPCs];
	int npcVelX[kNumNPCs];
	int npcVelY[kNumNPCs];
	int npcType[kNumNPCs];

	// Init NPC animations
	int npcIndex = 0;
	npcXPos[npcIndex] = 64;
	npcYPos[npcIndex] = 200;
	npcFrame[npcIndex] = kFramePersonUp;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = 1;
	npcType[npcIndex] = kNPCTypeMan;
	npcIndex++;

	npcXPos[npcIndex] = 128;
	npcYPos[npcIndex] = 164;
	npcFrame[npcIndex] = kFramePersonLeft;
	npcVelX[npcIndex] = -1;
	npcVelY[npcIndex] = 0;
	npcType[npcIndex] = kNPCTypeWoman;
	npcIndex++;

	npcXPos[npcIndex] = 128;
	npcYPos[npcIndex] = 200;
	npcFrame[npcIndex] = kFramePersonRight;
	npcVelX[npcIndex] = 1;
	npcVelY[npcIndex] = 0;
	npcType[npcIndex] = kNPCTypeGirl;
	npcIndex++;

	npcXPos[npcIndex] = 64;
	npcYPos[npcIndex] = 80;
	npcFrame[npcIndex] = kFramePersonRight;
	npcVelX[npcIndex] = 1;
	npcVelY[npcIndex] = 0;
	npcType[npcIndex] = kNPCTypeBoy;
	npcIndex++;

	npcXPos[npcIndex] = 64;
	npcYPos[npcIndex] = 48;
	npcFrame[npcIndex] = kFramePersonUp;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = 1;
	npcType[npcIndex] = kNPCTypeGirl;
	npcIndex++;

	npcXPos[npcIndex] = 128;
	npcYPos[npcIndex] = 128;
	npcFrame[npcIndex] = kFramePersonUp;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = 1;
	npcType[npcIndex] = kNPCTypeBoy;
	npcIndex++;

	npcXPos[npcIndex] = 128;
	npcYPos[npcIndex] = 200;
	npcFrame[npcIndex] = kFramePersonDown;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = -1;
	npcType[npcIndex] = kNPCTypeWoman;
	npcIndex++;

	npcXPos[npcIndex] = 64;
	npcYPos[npcIndex] = 180;
	npcFrame[npcIndex] = kFramePersonDown;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = -1;
	npcType[npcIndex] = kNPCTypeBoy;
	npcIndex++;

	npcXPos[npcIndex] = 164;
	npcYPos[npcIndex] = 28;
	npcFrame[npcIndex] = kFramePersonUp;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = 1;
	npcType[npcIndex] = kNPCTypeMan;
	npcIndex++;

	npcXPos[npcIndex] = 198;
	npcYPos[npcIndex] = 54;
	npcFrame[npcIndex] = kFramePersonUp;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = 1;
	npcType[npcIndex] = kNPCTypeGirl;
	npcIndex++;

	npcXPos[npcIndex] = 18;
	npcYPos[npcIndex] = 120;
	npcFrame[npcIndex] = kFramePersonDown;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = -1;
	npcType[npcIndex] = kNPCTypeWoman;
	npcIndex++;

	npcXPos[npcIndex] = 94;
	npcYPos[npcIndex] = 240;
	npcFrame[npcIndex] = kFramePersonDown;
	npcVelX[npcIndex] = 0;
	npcVelY[npcIndex] = -1;
	npcType[npcIndex] = kNPCTypeMan;
	npcIndex++;

	npcXPos[npcIndex] = 192;
	npcYPos[npcIndex] = 32;
	npcFrame[npcIndex] = kFramePersonRight;
	npcVelX[npcIndex] = 1;
	npcVelY[npcIndex] = 0;
	npcType[npcIndex] = kNPCTypeGirl;
	npcIndex++;

	npcXPos[npcIndex] = 128;
	npcYPos[npcIndex] = 16;
	npcFrame[npcIndex] = kFramePersonLeft;
	npcVelX[npcIndex] = -1;
	npcVelY[npcIndex] = 0;
	npcType[npcIndex] = kNPCTypeGirl;
	npcIndex++;

	assert(npcIndex == kNumNPCs);

	int switchDir = 16;

	while (frame++ < maxFrames)
	{
		if (frame > 1)
		{
			WaitForVSync(file, frame);
		}

		EnableSpritesNoWait(file, '0');
		SetSpriteAddress(file);
		for (int i = 0; i < 24; i++)
		{
			file << "b0,b0,b0,b0" << std::endl;
		}
		SetSpriteAddress(file);

		int lastSpriteBottomOutputYPos = 256;
		bool spritesDisabled = false;
		int numSprites = 0;

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
//							file << "d$a0140100" << std::endl;
						}
						// TODO: MPi: Investigate if the -14 here and -16 for "New lowest sprite Y pos" can be eliminated completely for 16x16 and 32x32 sprites
						// Perhaps the sprites are ending at the Y position, not starting from it?
						WaitForRaster(file, (255-lastSpriteBottomOutputYPos) - 14);	// Adjustment due to sprite position timing logic?
						DisableSprites(file, '0');
						spritesDisabled = true;
					}
					// Debug colour change
//					file << "d$a0140107" << std::endl;

					SetSpriteAddress(file);
					numSprites = 0;
				}
#endif

				// New lowest sprite Y pos
				lastSpriteBottomOutputYPos = npcYPos[i] - 16;

				file << std::dec;
				switch (npcType[i])
				{
				case kNPCTypeMan:
				default:
					switch (npcFrame[i])
					{
					case kFramePersonDown:
					default:
						file << "b3,b6,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b4,b7,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b5,b5,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 3;
						break;
					case kFramePersonLeft:
						file << "b6,b6,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b7,b7,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b8,b5,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 3;
						break;
					case kFramePersonRight:
						file << "b6,b$46,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b7,b$47,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b8,b$45,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 3;
						break;
					case kFramePersonUp:
						file << "b9,b6,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b10,b7,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b11,b5,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 3;
						break;
					}
					break;

				case kNPCTypeWoman:
					switch (npcFrame[i])
					{
					case kFramePersonDown:
					default:
						file << "b12,b8,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b13,b9,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 2;
						break;
					case kFramePersonLeft:
						file << "b14,b8,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b15,b9,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 2;
						break;
					case kFramePersonRight:
						file << "b14,b$48,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b15,b$49,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 2;
						break;
					case kFramePersonUp:
						file << "b16,b8,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b17,b9,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 2;
						break;
					}
					break;

				case kNPCTypeGirl:
					switch (npcFrame[i])
					{
					case kFramePersonDown:
					default:
						file << "b20,b10,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b21,b11,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b22,b12,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 3;
						break;
					case kFramePersonLeft:
						file << "b23,b10,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b24,b11,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b25,b12,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 3;
						break;
					case kFramePersonRight:
						file << "b23,b$4a,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b24,b$4b,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b25,b$4c,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 3;
						break;
					case kFramePersonUp:
						file << "b26,b10,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b27,b12,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 2;
						break;
					}
					break;

				case kNPCTypeBoy:
					switch (npcFrame[i])
					{
					case kFramePersonDown:
					default:
						file << "b28,b6,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b29,b5,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b30,b7,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 3;
						break;
					case kFramePersonLeft:
						file << "b31,b6,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b32,b5,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b33,b7,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 3;
						break;
					case kFramePersonRight:
						file << "b31,b$46,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b32,b$45,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b33,b$47,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 3;
						break;
					case kFramePersonUp:
						file << "b34,b6,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b35,b5,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						file << "b36,b7,b" << npcYPos[i] << ",b" << npcXPos[i] << std::endl;
						numSprites += 3;
						break;
					}
					break;
				}
			}
			if (numSprites >= 24)
			{
				printf("Warning: Sprite limit %d at frame %d\n", numSprites , frame);
			}
		}
		file << "d$0" << std::endl; 


#ifdef KEnableMultiplex
		// Check if the sprites need enabling
		if (spritesDisabled)
		{
			// Remove any remaining sprites hanging around from previous frames
			while (numSprites < 24)
			{
				file << "b0,b0,b0,b0" << std::endl;
				numSprites++;
			}

			EnableSprites(file, '0');
			// Debug colour change
//			file << "d$a0140100" << std::endl;
		}
#endif

		file << "d$0" << std::endl;
		file << std::endl;
		file << std::endl;

		for (int i = 0; i < kNumNPCs; i++)
		{
			npcXPos[i] += npcVelX[i];
			npcXPos[i] = npcXPos[i] & 255;
			npcYPos[i] += npcVelY[i];
			npcYPos[i] = npcYPos[i] & 255;
		}

		switch (frame)
		{
		case 16:
			npcVelX[0] = 1;
			npcVelY[0] = 0;
			npcFrame[0] = kFramePersonRight;
			break;
		case 48:
			npcVelX[0] = 0;
			npcVelY[0] = -1;
			npcFrame[0] = kFramePersonDown;
			break;
		default:
			break;
		}

		if (switchDir-- <= 0)
		{
			switchDir = 16 + (rand() & 1) * 16;

			int i = rand() % kNumNPCs;
			switch (rand() & 3)
			{
			case 0:
			default:
				npcVelX[i] = rand() & 1;
				npcVelY[i] = 0;
				npcFrame[i] = kFramePersonRight;
				break;

			case 1:
				npcVelX[i] = -(rand() & 1);
				npcVelY[i] = 0;
				npcFrame[i] = kFramePersonLeft;
				break;

			case 2:
				npcVelX[i] = 0;
				npcVelY[i] = rand() & 1;
				npcFrame[i] = kFramePersonUp;
				break;

			case 3:
				npcVelX[i] = 0;
				npcVelY[i] = -(rand() & 1);
				npcFrame[i] = kFramePersonDown;
				break;
			}
		}
	}

	file.close();
}
