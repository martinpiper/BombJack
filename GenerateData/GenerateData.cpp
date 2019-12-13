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
	const int maxFrames = 512;

	double rads1 = 0, rads2 = 0, rads3 = 0, rads4 = 0, rads5 = 0;
	const double rads1Speed = M_PI / 100 , rads2Speed = M_PI / 173, rads3Speed = M_PI / 123, rads4Speed = M_PI / 200, rads5Speed = M_PI / 240;
	const double rads1Separation = M_PI / 20, rads2Separation = M_PI / 30, rads3Separation = M_PI / 15;
	const double radius1x = 70, radius2x = 30, radius3x = 100;
	const double radius1y = 50, radius2y = 20;

	const char *paletteHiNybbles = "00000000244428882ccc2000";

	while (frame++ < maxFrames)
	{
		file << ";Frame " << frame << std::endl;
		file << "d$0" << std::endl;
		file << "^-$01" << std::endl;
		file << "d$0" << std::endl;

		int screenXPos = (int)(128 + (sin(rads4) * 120));
		int screenYPos = (int)(abs((sin(rads5) * 256)));
		int xy = (screenXPos & 15) | ((screenYPos & 15) << 4);
		file << "d$9e0101" << std::hex << std::setw(2) << xy << std::endl;
		xy = ((screenXPos / 16) & 15) | (((screenYPos / 16) & 15) << 4);
		file << "d$9e0201" << std::hex << std::setw(2) << xy << std::endl;

		file << "s$98200100" << std::endl;
		double rads1Real = rads1, rads2Real = rads2;
		for(int i = 0; i < 24; i++)
		{
			file << "b$" << std::hex << std::setw(2) << 0x10+i;
			file << ",b$" << paletteHiNybbles[i] << std::hex << std::setw(1) << ((i / 8)&0xf);
			file << ",b$" << std::hex << std::setw(2) << (int)(150 + (cos(rads1Real) * radius1y) + (sin(rads2Real) * radius2y));
			file << ",b$" << std::hex << std::setw(2) << (int)(128 + (sin(rads1Real) * radius1x) + (cos(rads2Real) * radius2x));
			file << std::endl;

			rads1Real += rads1Separation;
			rads2Real += rads2Separation;
		}
		file << std::endl;

		// Output some copper bars that bounce
		int colour = 1;
		for (int i = 0; i < 64; i++)
		{
			file << "d$0" << std::endl;
			file << "w$ff01ff00,$" << std::hex << std::setw(2) << (int) (64 + i + (sin(rads2) * 32));
			file << "018000" << std::endl;
			file << "d$9e0301" << std::hex << std::setw(2) << colour << std::endl;
			file << "d$9e0301" << std::hex << std::setw(2) << colour << std::endl;
			file << std::endl;
			colour++;
			if ((colour & 0xf) == 0)
			{
				colour++;
			}
		}
		file << "d$9e030100" << std::endl;
		file << "d$9e030100" << std::endl;


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
			file << "d$0" << std::endl;
			file << "w$ff01ff00,$" << std::hex << std::setw(2) << 160 + yoff;
			file << "018000" << std::endl;
			yoff++;

			// Debug colour change
			file << "d$9e0301" << std::hex << std::setw(2) << ((debugPal + 1) | 0xf0) << std::endl;
			file << "d$9e0301" << std::hex << std::setw(2) << ((debugPal + 1) | 0xf0) << std::endl;
			debugPal = (debugPal + 1) & 0x7;

			// Disable sprites, this is immediate for the next pixel output from the sprite shifts
			file << "d$9a00010c" << std::endl;
			file << "d$9a00010c" << std::endl;

			// Output address for sprites
			file << "s$" << std::hex << std::setw(4) << 0x9820 + (chunk * 4);
			file << "0100" << std::endl;

			for (int i = 0; i < chunkSize; i++)
			{
				file << "b$" << std::hex << std::setw(2) << 0x10 + (i + chunk) + frame;
				file << ",b$" << std::hex << std::setw(2) << (((i + chunk) / 8) & 0xf);
				file << ",b$" << std::hex << std::setw(2) << 32 + (i + chunk);
				file << ",b$" << std::hex << std::setw(2) << (int)(105 + (sin(rads3Real) * radius3x));
				file << std::endl;

				rads3Real += rads3Separation;
			}
			file << std::endl;

			// Debug colour change
			file << "d$9e0301" << std::hex << std::setw(2) << ((debugPal + 1) | 0xf0) << std::endl;
			file << "d$9e0301" << std::hex << std::setw(2) << ((debugPal + 1) | 0xf0) << std::endl;
			debugPal = (debugPal + 1) & 0x7;

			// Enable sprites after a short delay without the sprite RAM bus being busy
			// This gives enough time for the sprite registers to read new sprite positions and setup the shift buffers (16 pixels)
			// This depends on the DigitalData output speed, but can be precisely timed with a real copper running at the full clock rate
			file << "d$0" << std::endl;
			file << "d$0" << std::endl;
			file << "d$0" << std::endl;
			file << "d$0" << std::endl;
			file << "d$0" << std::endl;
			file << "d$0" << std::endl;
			file << "d$0" << std::endl;
			file << "d$0" << std::endl;
			// Enable sprites and preserve the 32x32 size value
			// Examining the rendered output the tall stretched sprites not yet updated are visible without artefacts
			file << "d$9a00011c" << std::endl;
			file << "d$9a00011c" << std::endl;

			// Debug colour change
			file << "d$9e030100" << std::endl;
			file << "d$9e030100" << std::endl;
			debugPal = (debugPal + 1) & 0x7;
		}
#endif
		// Idle bus
		file << "d$0" << std::endl;
		file << std::endl;


		// Reset the background colour
		file << "d$9e030100" << std::endl;
		file << "d$9e030100" << std::endl;
		file << "d$0" << std::endl;


		// Output a screen scroll split
		file << "; Screen split" << std::endl;
		file << "d$0" << std::endl;
		file << "w$ff01ff00,$b0018000" << std::endl;
		screenXPos = (int)(128 + (sin(rads1 * 2.0f) * 120));
		screenYPos = (int)(128 + (cos(rads1 * 2.0f) * 120));
		xy = (screenXPos & 15) | ((screenYPos & 15) << 4);
		file << "d$9e0101" << std::hex << std::setw(2) << xy << std::endl;
		file << "d$9e0101" << std::hex << std::setw(2) << xy << std::endl;
		xy = ((screenXPos / 16) & 15) | (((screenYPos / 16) & 15) << 4);
		file << "d$9e0201" << std::hex << std::setw(2) << xy << std::endl;
		file << "d$9e0201" << std::hex << std::setw(2) << xy << std::endl;

		rads1 += rads1Speed;
		rads2 += rads2Speed;
		rads3 += rads3Speed;
		rads4 += rads4Speed;
		rads5 += rads5Speed;

		file << "d$0" << std::endl;
		file << std::endl;
		file << std::endl;
	}

	file.close();
}
