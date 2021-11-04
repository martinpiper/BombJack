#pragma once
#include <iostream>
#include <fstream>

void WaitForVSync(std::ofstream &file, int frame);

void WaitForRaster(std::ofstream &file, int yoff);

void SetSpriteAddress(std::ofstream &file);

void DisableSprites(std::ofstream &file);

void EnableSprites(std::ofstream &file);

void EnableSpritesNoWait(std::ofstream &file);

void SetMode7Address(std::ofstream &file);

void EnableMode7(std::ofstream &file);
