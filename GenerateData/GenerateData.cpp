#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>
#include "Common.h"

int main(int argc, char**argv)
{
	int baseColour1 = 1;
	int baseColour2 = 255;
	std::ofstream file;
	file.open(argv[1]);
    
	file << std::setfill('0');

	int frame = 0;
	const int maxFrames = 512;

	double rads1 = 0, rads2 = 0, rads3 = 0, rads4 = 0, rads5 = 0;
	double mode7Rot = 0;
	const double rads1Speed = M_PI / 100 , rads2Speed = M_PI / 173, rads3Speed = M_PI / 123, rads4Speed = M_PI / 200, rads5Speed = M_PI / 240;
	const double rads1Separation = M_PI / 20, rads2Separation = M_PI / 30, rads3Separation = M_PI / 15;
	const double mode7RotSpeed = M_PI / 150;
	const double radius1x = 150, radius2x = 60, radius3x = 100;
	const double radius1y = 50, radius2y = 20;

	const char *paletteHiNybbles =		"00000000046408a8ac8c0020";
	const char *paletteHiNybblesMSBX =	"11111111157519b9bd9d1131";
	EnableMode7(file);

	while (frame++ < maxFrames)
	{
		WaitForVSync(file, frame);

		// Priority change - Default
		if (!(frame & 0x40))
		{
			file << "d$9e0801e4" << std::endl;
		}
		else
		{
			file << "d$9e080136" << std::endl;
		}

		int screenXPos = (int)abs(128 + (sin(rads4) * 120) + (sin(rads4 / 3) * 768));
		int screenYPos = (int)abs((sin(rads5) * 256) + (sin(rads5 / 3) * 768));
//		screenXPos = frame + 35;
//		screenYPos = 0;
		file << "d$9e0101" << std::hex << std::setw(2) << (screenXPos & 0xff) << std::endl;
		file << "d$9e0201" << std::hex << std::setw(2) << ((screenXPos>>8) & 0xff) << std::endl;
		file << "d$9e0301" << std::hex << std::setw(2) << (screenYPos & 0xff) << std::endl;
		file << "d$9e0401" << std::hex << std::setw(2) << ((screenYPos >> 8) & 0xff) << std::endl;

		screenXPos = (int)(512 + (sin(rads1 * 0.5f) * 600));
		screenYPos = (int)(256 + (cos(rads1 * 1.25f) * 256));
		file << "d$900101" << std::hex << std::setw(2) << (screenXPos & 0xff) << std::endl;
		file << "d$900201" << std::hex << std::setw(2) << ((screenXPos >> 8) & 0xff) << std::endl;
		file << "d$900301" << std::hex << std::setw(2) << (screenYPos & 0xff) << std::endl;
		file << "d$900401" << std::hex << std::setw(2) << ((screenYPos >> 8) & 0xff) << std::endl;


		// Calculate suitable mode7 values
		double scaleValue = 256 + 32 + (sin(mode7Rot * 5.0f) * 256);

//		scaleValue = 256.0f;
//		mode7Rot = 0;
		SetMode7Address(file);
		double dx = sin(mode7Rot + M_PI_2) * scaleValue;
		int intValue = (int) dx;
		file << "b$" << std::hex << std::setw(2) << (intValue & 0xff);
		file << ",b$" << std::hex << std::setw(2) << ((intValue >> 8) & 0xff);
		file << ",b$" << std::hex << std::setw(2) << ((intValue >> 16) & 0xff);
		double dxy = sin(mode7Rot) * scaleValue;
		intValue = (int) dxy;
		file << ",b$" << std::hex << std::setw(2) << (intValue & 0xff);
		file << ",b$" << std::hex << std::setw(2) << ((intValue >> 8) & 0xff);
		file << ",b$" << std::hex << std::setw(2) << ((intValue >> 16) & 0xff);
		file << std::endl;

		double dy = -sin(mode7Rot + M_PI_2 + M_PI_2 + M_PI_2) * scaleValue;
		intValue = (int) dy;
		file << "b$" << std::hex << std::setw(2) << (intValue & 0xff);
		file << ",b$" << std::hex << std::setw(2) << ((intValue >> 8) & 0xff);
		file << ",b$" << std::hex << std::setw(2) << ((intValue >> 16) & 0xff);
		double dyx = sin(mode7Rot + M_PI_2 + M_PI_2) * scaleValue;
		intValue = (int) dyx;
		file << ",b$" << std::hex << std::setw(2) << (intValue & 0xff);
		file << ",b$" << std::hex << std::setw(2) << ((intValue >> 8) & 0xff);
		file << ",b$" << std::hex << std::setw(2) << ((intValue >> 16) & 0xff);
		file << std::endl;

		// xpos/ypos org calculation, note how the coordinates project back along the deltas calculated above
		// xorg neg dx + yorg neg dxy
		intValue = (int)((frame * 256.0f) + (192.5f * -dx) + (64.5f * -dxy));
//		intValue = (int)((192.5f * -dx) + (64.5f * -dxy));
		file << "b$" << std::hex << std::setw(2) << (intValue & 0xff);
		file << ",b$" << std::hex << std::setw(2) << ((intValue >> 8) & 0xff);
		file << ",b$" << std::hex << std::setw(2) << ((intValue >> 16) & 0xff);
		// xorg neg dyx + yorg neg dy
		intValue = (int)((192.5f * -dyx) + (64.5f * -dy));
		file << ",b$" << std::hex << std::setw(2) << (intValue & 0xff);
		file << ",b$" << std::hex << std::setw(2) << ((intValue >> 8) & 0xff);
		file << ",b$" << std::hex << std::setw(2) << ((intValue >> 16) & 0xff);
		file << std::endl;

		file << "d$0" << std::endl;


#if 1
		SetSprites2Address(file);
		dx = sin(1.25f * mode7Rot + M_PI_2) * scaleValue * 0.25f;
		intValue = (int)dx;
		file << "b$" << std::hex << std::setw(2) << (frame & 0x03);
		file << ",b$" << std::hex << std::setw(2) << (intValue & 0xff);
		file << ",b$" << std::hex << std::setw(2) << (0x20);
		dy = sin(mode7Rot) * scaleValue * -1.25f;
		intValue = (int)dy;
		file << ",b$" << std::hex << std::setw(2) << (intValue & 0xff);
		file << ",b$" << std::hex << std::setw(2) << (0x20);
		file << ",b$" << std::hex << std::setw(2) << (0x20);
		file << ",b$" << std::hex << std::setw(2) << (0x20);
		file << ",b$" << std::hex << std::setw(2) << (frame & 0x03);
		file << std::endl;

		dx = sin(1.55f * mode7Rot + M_PI_2) * scaleValue * 0.15f;
		intValue = (int)dx;
		file << "b$" << std::hex << std::setw(2) << (frame & 0x03);
		file << ",b$" << std::hex << std::setw(2) << (intValue & 0xff);
		file << ",b$" << std::hex << std::setw(2) << (0x30);
		dy = sin(mode7Rot) * scaleValue * -1.25f;
		intValue = (int)dy;
		file << ",b$" << std::hex << std::setw(2) << (intValue & 0xff);
		file << ",b$" << std::hex << std::setw(2) << (0x20);
		file << ",b$" << std::hex << std::setw(2) << (0x10);
		file << ",b$" << std::hex << std::setw(2) << (0x10);
		file << ",b$" << std::hex << std::setw(2) << ((frame + 1) & 0x03);
		file << std::endl;

		dx = sin(0.55f * mode7Rot + M_PI_2) * scaleValue * -0.15f;
		intValue = (int)dx;
		file << "b$" << std::hex << std::setw(2) << (0x0b);
		file << ",b$" << std::hex << std::setw(2) << (intValue & 0xff);
		file << ",b$" << std::hex << std::setw(2) << (0x80);
		dy = sin(0.45f * mode7Rot) * scaleValue * -0.25f;
		intValue = (int)dy;
		file << ",b$" << std::hex << std::setw(2) << (intValue & 0xff);
		file << ",b$" << std::hex << std::setw(2) << (0x20);
		file << ",b$" << std::hex << std::setw(2) << (0x08);
		file << ",b$" << std::hex << std::setw(2) << (0x08);
		file << ",b$" << std::hex << std::setw(2) << (0x00);
		file << std::endl;
#endif


#if 1
		// Top sprites
		SetSpriteAddress(file);
		double rads1Real = rads1, rads2Real = rads2;
		for(int i = 0; i < 24; i++)
		{
			file << "b$" << std::hex << std::setw(2) << 0x10+i;
			int xpos = (int)(192 + (sin(rads1Real) * radius1x) + (cos(rads2Real) * radius2x));
			if (xpos & 0x100)
			{
				file << ",b$" << paletteHiNybblesMSBX[i] << std::hex << std::setw(1) << ((i / 8) & 0xf);
			}
			else
			{
				file << ",b$" << paletteHiNybbles[i] << std::hex << std::setw(1) << ((i / 8) & 0xf);
			}
			file << ",b$" << std::hex << std::setw(2) << (int)(150 + (cos(rads1Real) * radius1y) + (sin(rads2Real) * radius2y));
			file << ",b$" << std::hex << std::setw(2) << (xpos & 0xff);
			file << std::endl;

			rads1Real += rads1Separation;
			rads2Real += rads2Separation;
		}
		file << std::endl;
#endif

#if 1
		// Output some copper bars that bounce
//		file << "++" << std::endl;
		int colour1 = baseColour1;
		int colour2 = baseColour2;
		for (int i = 0; i < 64; i++)
		{
			colour1 &= 0xff;
			colour2 &= 0xff;

			int ypos = (int)(64 + i + (sin(rads2) * 32));

			file << "d$0" << std::endl;
			file << "w$ff03ff00,$" << std::hex << std::setw(2) << ypos;
			file << "00" << std::hex << std::setw(2) << (int)(0x20 + (sin(i*M_PI / 64.0) * 16)) << "00" << std::endl;
//			file << "001000" << std::endl;
			file << "d$a01401" << std::hex << std::setw(2) << colour1 << std::endl;
			file << std::endl;

			file << "d$0" << std::endl;
			file << "w$ff03ff00,$" << std::hex << std::setw(2) << ypos;
			file << "00" << std::hex << std::setw(2) << (int)(0x50 + (cos(i*M_PI / 32.0) * 16)) << "00" << std::endl;
			file << "d$a01401" << std::hex << std::setw(2) << colour2 << std::endl;
			file << std::endl;

			file << "d$0" << std::endl;
			file << "w$ff03ff00,$" << std::hex << std::setw(2) << ypos;
			file << "00" << std::hex << std::setw(2) << (int)(0x90 + (sin(i*M_PI / 32.0) * 16)) << "00" << std::endl;
			file << "d$a01401" << std::hex << std::setw(2) << colour1 << std::endl;
			file << std::endl;

			file << "d$0" << std::endl;
			file << "w$ff03ff00,$" << std::hex << std::setw(2) << ypos;
			file << "00" << std::hex << std::setw(2) << (int)(0xc0 + (cos(i*M_PI / 64.0) * 16)) << "00" << std::endl;
			file << "d$a01401" << std::hex << std::setw(2) << colour2 << std::endl;
			file << std::endl;

			colour1++;
			if ((colour1 & 0x7) == 0)
			{
				colour1++;
			}

			colour2++;
			if ((colour2 & 0x7) == 0)
			{
				colour2++;
			}
		}
		file << "d$a0140100" << std::endl;
//		file << "+16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,+" << std::endl;

		baseColour1++;
		if ((baseColour1 & 0x7) == 0)
		{
			baseColour1++;
		}

		baseColour2--;
		if ((baseColour2 & 0x7) == 0)
		{
			baseColour2--;
		}
#endif

		// TODO: While this works, it needs a proper copper
#if 1
		file << "; Multiplex sprites" << std::endl;
		// Setup a wait for xpos ff ypos 160++ and transfer in new sprite data
		double rads3Real = rads3;
		int yoff = 0;
		int debugPal = 0;
		const int chunkSize = 12;
		for (int chunk = 0; chunk < 24; chunk += chunkSize)
		{
			// Wait for raster and the position in the line
			WaitForRaster(file, 160 + yoff);
			yoff++;

			// Priority change - Sprites should be behind everything, then mode 7 , then chars, then tiles in front
			// Vertical sprite palette lines should be seen because they are in the last layer
			if (!(frame & 0x40))
			{
				file << "d$9e080136" << std::endl;
			}
			else
			{
				file << "d$9e0801e4" << std::endl;
			}

			// Debug colour change
			file << "d$a01401" << std::hex << std::setw(2) << ((debugPal + 1) | 0xf0) << std::endl;
			debugPal = (debugPal + 1) & 0x7;

			DisableSprites(file);

			// Output address for sprites
			file << "s$" << std::hex << std::setw(4) << 0x9800 + (chunk * 4);
			file << "0100" << std::endl;

			for (int i = 0; i < chunkSize; i++)
			{
				file << "b$" << std::hex << std::setw(2) << 0x10 + (i + chunk) + frame;
				file << ",b$" << std::hex << std::setw(2) << (((i + chunk)*2) & 0x0f);
				file << ",b$" << std::hex << std::setw(2) << 32 + (i + chunk);
				file << ",b$" << std::hex << std::setw(2) << (int)(105 + (sin(rads3Real) * radius3x));
				file << std::endl;

				rads3Real += rads3Separation;
			}
			file << std::endl;

			// Debug colour change
			file << "d$a01401" << std::hex << std::setw(2) << ((debugPal + 1) | 0xf0) << std::endl;
			debugPal = (debugPal + 1) & 0x7;

			EnableSprites(file);

			// Debug colour change
			file << "d$a0140100" << std::endl;
			debugPal = (debugPal + 1) & 0x7;
		}
#endif
		// Idle bus
		file << "d$0" << std::endl;
		file << std::endl;


		// Reset the background colour
		file << "d$a0140100" << std::endl;
		file << "d$0" << std::endl;


		// Output a screen scroll split
		WaitForRaster(file, 0xb0);
		screenXPos = (int)(128 + (sin(rads1 * 2.0f) * 120));
		screenYPos = (int)(128 + (cos(rads1 * 2.0f) * 120));
		file << "d$9e0101" << std::hex << std::setw(2) << (screenXPos & 0xff) << std::endl;
		file << "d$9e0201" << std::hex << std::setw(2) << ((screenXPos >> 8) & 0xff) << std::endl;
		file << "d$9e0301" << std::hex << std::setw(2) << (screenYPos & 0xff) << std::endl;
		file << "d$9e0401" << std::hex << std::setw(2) << ((screenYPos >> 8) & 0xff) << std::endl;

		rads1 += rads1Speed;
		rads2 += rads2Speed;
		rads3 += rads3Speed;
		rads4 += rads4Speed;
		rads5 += rads5Speed;

		mode7Rot += mode7RotSpeed;

		file << "d$0" << std::endl;
		file << std::endl;
		file << std::endl;
	}

	file.close();
}
