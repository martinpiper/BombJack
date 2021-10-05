#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	FILE *fp = fopen("Sprite2Scale.bin", "wb");

	for (int currentSpriteScaleYInv = 0; currentSpriteScaleYInv < 256; currentSpriteScaleYInv++)
	{
		for (int insideHeight = 0; insideHeight < 256; insideHeight++)
		{
			int currentSpriteYPixel = ((currentSpriteScaleYInv / 2) + ((insideHeight & 0xff) * currentSpriteScaleYInv)) >> 4;

			// And clamp
			if (currentSpriteYPixel > 255)
			{
				currentSpriteYPixel = 255;
			}
			if (currentSpriteYPixel < 0)
			{
				currentSpriteYPixel = 0;
			}

			fputc(currentSpriteYPixel, fp);
		}
	}

	fclose(fp);

	return 0;
}
