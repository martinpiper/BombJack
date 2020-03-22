#pragma once
#include <iostream>
#include <fstream>

void WaitForVSync(std::ofstream &file, int frame);

void WaitForRaster(std::ofstream &file, int yoff);

void SetSpriteAddress(std::ofstream &file);

void DisableSprites(std::ofstream &file, char spr32);

void EnableSprites(std::ofstream &file, char spr32);

void EnableSpritesNoWait(std::ofstream &file, char spr32);

void SetMode7Address(std::ofstream &file);
