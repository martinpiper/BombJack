#include <iomanip>
#include "Common.h"

void WaitForVSync(std::ofstream &file, int frame)
{
	file << "; WaitForVSync " << std::dec << frame << " $" << std::hex << frame << std::endl;
	file << "d$0" << std::endl;
	file << "^-$01" << std::endl;
	file << "d$0" << std::endl;
}

void WaitForRaster(std::ofstream &file, int yoff)
{
	file << "; WaitForRaster" << std::endl;
	file << "d$0" << std::endl;
	file << "w$ff03ff00,$" << std::hex << std::setw(2) << yoff;
	// xpos = 320 ($140)
	file << "014000" << std::endl;
}

void SetSpriteAddress(std::ofstream &file)
{
	file << "; SetSpriteAddress" << std::endl;
	file << "s$98000100" << std::endl;
}


void DisableSprites(std::ofstream &file)
{
	// Disable sprites, this is immediate for the next pixel output from the sprite shifts
	file << "; DisableSprites" << std::endl;
	file << "d$9e0a010e" << std::endl;
}

void EnableSprites(std::ofstream &file)
{
// Enable sprites after a short delay without the sprite RAM bus being busy
// This gives enough time for the sprite registers to read new sprite positions and setup the shift buffers (16 pixels)
// This depends on the DigitalData output speed, but can be precisely timed with a real copper running at the full clock rate
	file << "; EnableSprites" << std::endl;
	file << "d$0" << std::endl;
	file << "d$0" << std::endl;
	file << "d$0" << std::endl;
	file << "d$0" << std::endl;
	file << "d$0" << std::endl;
	file << "d$0" << std::endl;
	file << "d$0" << std::endl;
	file << "d$0" << std::endl;
	EnableSpritesNoWait(file);
}

void EnableSpritesNoWait(std::ofstream &file)
{
	file << "; EnableSpritesNoWait" << std::endl;
	// Examining the rendered output the tall stretched sprites not yet updated are visible without artefacts
	file << "d$9e0a010f" << std::endl;
}

void SetMode7Address(std::ofstream &file)
{
	file << "; SetMode7Address" << std::endl;
	file << "s$a0000100" << std::endl;
}

void EnableMode7(std::ofstream &file)
{
	file << "; EnableMode7" << std::endl;
	file << "d$a015011f" << std::endl;
	file << "d$0" << std::endl;
}
