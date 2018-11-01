# BombJack

* https://github.com/mamedev/mame/blob/master/src/mame/drivers/bombjack.cpp
* https://floooh.github.io/2018/10/06/bombjack.html
* https://www.arcade-museum.com/game_detail.php?game_id=7180


At 0x9820 - 0x987f each sprite is described by 4 bytes:

Byte 0:
Bit 7: if set, this is a 32x32 sprite, otherwise 16x16
Bits 6..0: 7 bits to define the tile code for the sprite, used to look up the sprite’s image bitplanes in the tile ROMs
Byte 1:
Bit 7: if set, the sprite is horizontally flipped
Bit 6: if set, the sprite is vertically flipped
Bits 3..0: 4 bits to provide the color value for the tile decoder
Byte 2: the sprite’s X position on screen
Byte 3: the sprite’s Y position on screen
