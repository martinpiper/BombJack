#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	FILE *fp = fopen("SampleVolume.bin" , "wb");

	for (int a = 0 ; a < 256 ; a++)
	{
		for (int b = 0 ; b < 256 ; b++)
		{
			int output = (a * b) / 255;
			if (output > 255)
			{
				output = 255;
			}
			fputc(output , fp);
		}
	}

	fclose(fp);
	return 0;
}
