#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	FILE *fp = fopen("Sprite4Lookup.bin", "wb");

	for (int currentSpriteStride = 0; currentSpriteStride < 256; currentSpriteStride++)
	{
		for (int currentSpriteYPixel = 0; currentSpriteYPixel < 256; currentSpriteYPixel++)
		{
			int multiply = currentSpriteYPixel * (currentSpriteStride + 1);

			// And clamp
			if (multiply > 255)
			{
				multiply = 255;
			}
			if (multiply < 0)
			{
				multiply = 0;
			}

			fputc(multiply, fp);
		}
	}

	fclose(fp);

	return 0;
}
