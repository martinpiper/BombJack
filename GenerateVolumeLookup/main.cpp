#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	FILE *fp = fopen("SampleVolume.bin" , "wb");

	for (int b = 0 ; b < 256 ; b++)
	{
		for (int a = 0 ; a < 256 ; a++)
		{
			// Convert from u8 to s8 sample
			int samp = a - 0x80;
			int output = (samp * b) / 255;
			// Convert from s8 to u8 sample
			output = output + 0x80;

			// And clamp
			if (output > 255)
			{
				output = 255;
			}
			if (output < 0)
			{
				output = 0;
			}

			fputc(output , fp);
		}
	}

	fclose(fp);
	return 0;
}
