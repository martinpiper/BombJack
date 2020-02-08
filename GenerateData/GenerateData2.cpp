#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>

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

	const int kNumNPCs = 1;
	int npcXPos[kNumNPCs];
	int npcYPos[kNumNPCs];
	int npcFrame[kNumNPCs];
	int npcVelX[kNumNPCs];
	int npcVelY[kNumNPCs];

	// Init NPC animations
	npcXPos[0] = 64;
	npcYPos[0] = 200;
	npcFrame[0] = kFrameManUp;
	npcVelX[0] = 0;
	npcVelY[0] = 1;

	while (frame++ < maxFrames)
	{
		file << ";Frame " << frame << std::endl;
		file << "d$0" << std::endl;
		file << "^-$01" << std::endl;
		file << "d$0" << std::endl;

		for (int i = 0; i < kNumNPCs; i++)
		{
			file << "s$98200100" << std::endl;

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

			npcXPos[i] += npcVelX[i];
			npcYPos[i] += npcVelY[i];
		}

		file << "d$0" << std::endl;
		file << std::endl;
		file << std::endl;

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
