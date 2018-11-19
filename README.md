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
