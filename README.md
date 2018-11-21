# BombJack

* https://github.com/mamedev/mame/blob/master/src/mame/drivers/bombjack.cpp
* https://floooh.github.io/2018/10/06/bombjack.html
* https://www.arcade-museum.com/game_detail.php?game_id=7180


Sprite 32x32 size select
0x9a00 start index of 32x32 sprites
0x9a01 end index of 32x32 sprites (exclusive)
	So 0,0 = no 32x32 sprites
	0,8 means indexes 0 to 7 (inclusive) are 32x32
	Maximum number in both is is 0xf

At 0x9820 - 0x987f each sprite is described by 4 bytes:

Byte 0:
	The tile code for the sprite, used to look up the sprite’s image bitplanes in the tile ROMs
	(MAME Emu documentation is wrong, bit 7 has nothing to do with selecting double size mode)

Byte 1:
	Bit 7: if set, the sprite is horizontally flipped
	Bit 6: if set, the sprite is vertically flipped
	Bits 3..0: 4 bits to provide the color value for the tile decoder
Byte 2: The sprite’s X position on screen
Byte 3: The sprite’s Y position on screen



Using a "6MHz" clock of 1M. Due to default 4A/4B/4C/4D RAM timings being too tight.
System->Set Animation Options
	Single Step Time: 83n
	500n For debugging full clock cycle and pixel clock
	
	
	

Using: 9800 top left all the same.ptn
Set BV[0..7] break for 0xe8

Raster line schedule
	BV = vertical raster
	BH = horizontal pixel clock
		Full line starts at $180 to $1ff then $000 to $0ff
		Giving 384 pixel clocks per line
		Visible portion $008 to $188.
		The 8 pixel delay is to sync with the 8 pixel delay for sprite data output into the off scan buffer
	SREAD = Sprite register address (lo byte)
	
	Line BH starts at $180
	$180	SREAD $20
			Begin sprite 0 register reads
			1V*. H
			Note 4A/B are $ff blank
			Note 4C/D have pixel data in the first 8 bytes (16 pixels) then 0xff blank
	$182	SREAD $21
	$184	SREAD $22
	$186	SREAD $23
	$188 	SREAD $20 to $23 again
			1V*. L
	$188H	OV0/1/2 LHH OC0/1/2/3 HLHL from M0-M7
	$188L	All L no data from $188H clocked into pixel output latch at 5E2
	$189H	Begin sprite 0 pixel writes
			4A/B First pixel written $7f (0 offset LSB byte in the window)
			Note 7 written into 4B since it is the high nybble, 4A looks unchanged
			This is transparent .111 with palette .1111
	$18aH	4A/B Second pixel written $7b (0 offset MSB byte in the window)
			This is not transparent .011 with palette .1111
	$190	SREAD $24 same 4x2 repeating pattern every 2 pixels
			Begin sprite 1 register reads
			Meaning 16 pixels elapse for each sprite
	$198	4A/B Last transparent pixel $7f written at byte offset $7 (pixel $f) high nybble
	$199H	Begin sprite 1 pixel writes
			Pixel $67 written (0 offset LSB byte in the window)
			This is transparent .111 with palette .1100
	$000	SREAD $40 same 4x2 pattern as above
	$008L	Pixel data arrives at 5E2
	$009H	Pixel data is latched into 5E2
			Pixel also cleared in 4C/D
			This clears the pixel just output to the final video palette check since it's obviously not needed
	$0fe	SREAD $7f
	Loops back to $180 again and BV increment
