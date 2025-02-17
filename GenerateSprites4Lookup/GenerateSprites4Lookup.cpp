#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	FILE *fpL = fopen("Sprite4LookupLow.bin", "wb");
	FILE *fpH = fopen("Sprite4LookupHigh.bin", "wb");

	for (int currentSpriteStride = 0; currentSpriteStride < 256; currentSpriteStride++)
	{
		for (int currentSpriteYPixel = 0; currentSpriteYPixel < 256; currentSpriteYPixel++)
		{
			int multiply = currentSpriteYPixel * (currentSpriteStride + 1);

			fputc(multiply & 0xff, fpL);
			fputc((multiply >> 8) & 0xff, fpH);
		}
	}

	fclose(fpL);
	fclose(fpH);

	return 0;
}
