# Bomb Jack display hardware

https://twitter.com/hashtag/RetroTTLVideoCard
https://twitter.com/hashtag/RetroTTL
#RetroTTL #RetroTTLVideoCard #C64 #Commodore64
 
This started life as a working schematic for the video display portion of the original Bomb Jack arcade hardware. Why bother, you're probably asking? Well if you have to ask then perhaps you're not the right audience. :)

The version 1.0 of this work was approximately interface pin and chip compatible with the original hardware schematic. Now since V2.0 the hardware has progressed to add extra features not present in the original, such as:

	* Addressable RAM instead of ROMs
	* Extra display blanking capability
	* Tile X/Y pixel scroll
	* 64x64 Tile X/Y offset, useful for fast hardware scrolling
	* Full screen height sprites
	* Background "mode7" screen that allows per-pixel affine transformation
	* Background colour select (Can be rapidly updated during the frame for plasma effects)


## Background

This project started when old retro arcade hardware was being discussed. In the back of my mind was the often fabled "Mega games" by Imagine Software which were planned to use extra hardware on the Spectrum and Commodore 64 to augment the machine's capabilities. Since this hardware uses TTL logic available back from the same time period I was wondering exactly how much extra graphical grunt could have been engineered and interfaced with these old 8-bit computers.

Truth be told, the Imagine hardware was pretty much just extra RAM https://www.gamesthatwerent.com/gtw64/mega-games/ but this was a fun project to see how far the arcade hardware was pushing the limits of board size and signal complexity.

I was looking at Bomb Jack boards on ebay and pondering how they had enough fill-rate to draw 24 16x16 sprites and have the option for some to use 32x32 mode as well. A friend and I were discussing the clock speed and fill-rate while trying to deduce the operation of the hardware just by inspecting the hand drawn schematics, as you do.

In the end to get some clarity on the sprite plotting specifically I started to transcribe what was thought to be the sprite logic portion of the schematic into Proteus, since it can simulate digital electronics really well.

## Digital display simulation

The arcade hardware uses resistor ladders to convert 4x3 bit RGB+H+V sync digital signals into analog, this is a typical model for the period. Since the simulation software is optimised for digital model it makes sense to have a digital display driver simulation that accepts digital signals. Hence I created this Proteus VSM module: https://github.com/martinpiper/DigitalDisplay

This simulation can also be used to debug failures in original boards, since disabling chips or signals in the simulation can produce similar output display artefacts. For example disabling one of the scan line RAM chips (4A/4B/4C/4D) produces alternate pixels and bright lines, similar to the original hardware.

## Developer technical details

If you have Proteus setup with the digital display model, you should see a screen that looks somewhat like this when starting the simulation:

![Screenshot](Capture.PNG?raw=true "Screenshot")

The new hardware scrolling registers also work well:

![animation](output/debug.gif?raw=true "animation")

The hardware supports sprite multiplexing and stacking, giving the option of having more than eight colours in sprites:

![animation](output/debug2.gif?raw=true "animation2")

Many thanks to https://opengameart.org/content/rpg-town-pixel-art-assets and https://opengameart.org/content/colored-16x16-fantasy-tileset for the game art.



### High level schematic sheets

The Proteus sheet numbers correspond to the original schematic page numbers as written in the bottom right hand corner of each page. The layout of this schematic roughly matches the layout of the original schematic. So for example the analog RGB resistor ladders are on "Root sheet 8" in the top right of the sheet, as in the original schematic page 8 layout.

1.	Logic analyser, virtual video display model, expansion bus header, data generator
	1. Memory selection logic based on external address bus
	2. VSMDD2 is a replacement for the Proteus data generators because they have a 1024 byte limit. Proteus VSM project source: https://github.com/martinpiper/DigitalData
	3. EXPANSIONBUS, EXPANSIONBUS2 and EXPANSIONBUS3 all relate to the C64 user port to 24 bit address logic in the project: UserPortTo24BitAddress.pdsprj
2.	Headers, the original schematic has dip switch logic
3.	Video timing
	1. Horizontal video signal generation
	2. Vertical video signal generation
	3. HBLANK and VBLANK signal generation
	4. 32x32 sprite selection registers and comparison
	5. Various signal timings generated for sprite loading, tile and colour fetch etc
4.	Sprite logic - Part 1
	1. Sprite RAM register access logic, timing with the external address/data bus and video internal timings
	2. Temporary storage for sprite position, palette and frame
	3. Bit plane access and bit shifters for output pixels to scan RAM
5.	Sprite logic - Part 2
	1. Dual scan line RAM buffers
	2. Transparent pixel test logic based on inverted pixel temporary storage and selection with NOR check
	3. Pixel writing to RAM based on 16 pixel chunks and position register contents
	4. Logic for timing of pixel reads for possible video display, plus clearing of data just read
6.	Character screen RAM with associated colour screen
	1. Associated external bus interface logic
	2. Bit plane reads and pixel shifters
	3. Output pixel logic for possible video display
7.	16x16 tile background picture logic
	1. Logic for selection of appropriate picture, based on high address lines
	2. Bit plane reads and pixel shifters
	3. Output pixel logic for possible video display, or disable background logic
8.	Output pixel pipeline and palette lookup
	1. Pixel transparency test from background, character screen and sprites
	2. Palette RAMs with external bus logic
	3. Final pixel latching logic and palette RAM lookup
	4. Output digital to analog conversion using resistor ladders
	5. Analog RGB and associated sync signal output header
9.	Mode7 calculation
	1. Register addressing and latches
	2. dx, dxy, dy, dyx calculation
	3. xorg, yorg addition
10.	Mode7 pixel logic
	1. Background colour latch, transparent pixel detection
	2. Interleaved blocks (for RAM timing) mode7 screen access, feeding into tile access, H & V flip logic

### Memory map

Note: Most control registers can be updated at any point during the frame and the next pixel will reflect the change.

0x9000			Chars control register
		0x01	Palette bank lo/hi control
		0x02	Chars screen disable
		0x40	Bank select 0x01 with below
		0x80	Bank select 0x02 with above
0x9c00-0x9cff	Palette RAM


0x9e00	0x10	Enable tiles
		0x20	Enable display
		0x40	BorderX shrink
		0x80	BorderY shrink

0x9e08	Layer priority select, for each layer select the input header pixel input
		Default should be: %11100100 = 0xe4
		i.e. Back most layer	= %11 (3)
		Next layer closer		= %10 (2)
		Then next closest		= %01 (1)
		Then front most layer	= %00 (0)



Sprite 32x32 size select

	0x9a00 start index of 32x32 sprites
		Upper nybble: $10 = Enable sprite output (6S SPREN)
	0x9a01 end index of 32x32 sprites (exclusive)
		So 0,0 = no 32x32 sprites
		0,8 means indexes 0 to 7 (inclusive) are 32x32
		Maximum number in both is is 0xf



At 0x9820 - 0x987f each sprite is described by 4 bytes:

	Byte 0:
		The tile code for the sprite, used to look up the sprite’s image bitplanes in the tile ROMs
		(MAME Emu documentation is wrong, bit 7 has nothing to do with selecting double size mode)

	Byte 1: HVFCCCCC
		Bit 7: H : If set, the sprite is horizontally flipped
		Bit 6: V : If set, the sprite is vertically flipped
		But 5: F : If set, the sprite is full screen height. The sprite data will repeat.
		Bits 4..0: C : 5 bits to provide the colour value for the tile decoder

	Byte 2: The sprite’s X position on screen
	Byte 3: The sprite’s Y position on screen


### Mode7 registers

Mode7 borrows its name from the graphics mode on the Super NES video game console. https://en.wikipedia.org/wiki/Mode_7

0xa000 - 0xa011	: Sets of 24 bit (8 byte) registers in l/h/hh order. There are 8 bits of accuracy, i.e. numbers are multiplied by 256.0f
	
	dx
	dxy
	dy
	dyx
	xorg
	yorg

Each pixel on the layer is accessed using the accumulated results of these registers, this means it is possible to have per-pixel transformations combining translation, scaling, reflection, rotation, and shearing.
It is possible to update these registers per scanline, or per pixel, and generate even more complex results. Register values are latched and used immediately, so timing the update of the three bytes must be considered.

Note: The internal accumulated values, x/xy/y/yx are not accessible via registers and are reset only by _HSYNC and _VSYNC

For each horizontal pixel (on +ve 6MHz):

	x += dx
	yx += dyx

	xo = x + xy + xorg
	yo = y + yx + yorg

For each scanline (on +ve _HSYNC):

	xy += dxy
	y += dy

For each scanline (on _HSYNC):

	yx = 0
	x = 0

For each frame (on _VSYNC):

	y = 0
	xy = 0

All of the above is one way to progressively calculate xo,yo using addition. In other words:

	xo = (dx*x) + (dxy*y) + xorg
	yo = (dy*y) + (dyx*x) + yorg

During the coordinate (xo,yo) to screen transformation: bits 8-11 are used to lookup the tile pixel, bits 12-18 for the x screen tile index position, bits 12-17 for the y screen tile index position.

Note: The full, but hidden by borders, screen resolution is 384x264 pixels.

0xa014	: Background colour, any pixel index of zero in the mode7 tile data will show the background colour
0xa015	: Flags	:	0 : Enable display
					1 : Enable X update, or reset to 0
					2 : Enable XY update, or reset to 0
					3 : Enable Y update, or reset to 0
					4 : Enable YX update, or reset to 0


	
### Clock speeds

The original schematic uses a 6MHz clock for all the video hardware, as denoted by the "6MHz" signal line. The clocking can be found on "Root sheet 1" just about the logic analyser.

To help debug timing issues, especially with RAMs, the schematic should be simulated at 6MHz. However the Proteus simulation can use 1MHz on this signal line, I kept the naming of the line the same as the original schematic however. This is due to the default RAM write timings for ICs 4A/4B/4C/4D being too tight. This means the digital display driver will detect ~10fps, not ~60fps as per the original design. It does however make the debug single step time easier to think about since it's not divided by 6MHz...

With VIDCLK = 2M

Logic analyser:
	Display scale 0.2u
	Capture Resolution 0.2u
	
Remember to use:

	System->Set Animation Options
		Single Step Time: 250n
		500n For debugging full clock cycle and pixel clock


### Where is the Z80 CPU?

The Z80 CPU from the original schematic is not included, it was clocked independently and has its own RAM, except for the video hardware interfaces and isn't needed for the video simulation.


### Input data setup

The Z80 data write signals are mocked using a simulator pattern generator VSMDD2, this is separate to the main video schematic and the generator is excluded from the PCB layout.

As per the original design all writes to the video hardware should be carefully timed to coincide with the VBLANK or other safe write blanking periods. This is because the video hardware is almost always reading the RAM during the visible portion of the frame. Writing to the sprite registers outside the VBLANK will especially produce nasty looking effects on the screen. This RAM sharing model is quite common is old arcade and console hardware.

The original hardware has been expanded to include RAMs where the ROMs were located. These are addressed by an combination of EXPANSIONBUS3 to select the groups of RAMs and the EXPANSIONBUS group selector. It is entirely possible to write more than one group at a time by enabling multiple output bits in EXPANSIONBUS3.

   | EXPANSIONBUS3 | Group                         | EXPANSIONBUS address | Behaviour                               |
   |---------------|-------------------------------|----------------------|-----------------------------------------|
   | $01           | Audio registers               | $8000 11 bytes/voice | Audio registers                         |
   | $01           | Original RAMs                 | $9000                | Screen low/high palette bank            |
   | $01           | Original RAMs                 | $9820   $60 bytes    | Sprite registers                        |
   | $01           | Original RAMs                 | $9c00   $200 bytes   | Palette GR XB 2 bytes per entry         |
   | $01           | Original RAMs                 | $9e00                | Background image enable and borders     |
   | $01           | Original RAMs                 | $9e01/2              | Background image XL/XH pixel scroll     |
   | $01           | Original RAMs                 | $9e03/4              | Background image YL/YH pixel scroll     |
   | $01           | Original RAMs                 | $9e07                | Background colour select                |
   | $01           | Original RAMs                 | $9a00-$9a01          | Start/end 32x32 sprite index 0-f only   |
   | $01           | Extension mode7 registers     | $a000-$a017          | Mode7 registers                         |
   | $02           | Audio memory                  | $0000-$ffff          | Audio sample memory                     |
   | $04           | APU                           | $8000   8KB          | APU memory bank 0 instruction           |
   | $04           | APU                           | $4000   8KB          | APU memory bank 1 data                  |
   | $04           | APU                           | $2000   8KB          | APU bank 2 registers                    |
   | $80           | Background 16x16 Root sheet 7 | $2000   8KB          | Tiles and colours into 4P7R             |
   | $80           | Char screen banks 0-3         | $4000   8KB          | Chars and colours into U258             |
   | Note 1 spare  |                               |                      |                                         |
   | $40           | Background 16x16 Root sheet 7 | $2000   8KB          | Tiles bit plane 0 into 8R7R             |
   | $40           | Background 16x16 Root sheet 7 | $4000   8KB          | Tiles bit plane 1 into 8N7R             |
   | $40           | Background 16x16 Root sheet 7 | $8000   8KB          | Tiles bit plane 2 into 8L7R             |
   | $20           | Char screen 8x8 Root sheet 6  | $2000   8KB          | Char data bit plane 0 into 8K6R         |
   | $20           | Char screen 8x8 Root sheet 6  | $4000   8KB          | Char data bit plane 1 into 8H6R         |
   | $20           | Char screen 8x8 Root sheet 6  | $8000   8KB          | Char data bit plane 2 into 8E6R         |
   | $10           | Sprite data Root sheet 4      | $2000   8KB          | Sprite bit plane 0 into 7JR             |
   | $10           | Sprite data Root sheet 4      | $4000   8KB          | Sprite bit plane 1 into 7LR             |
   | $10           | Sprite data Root sheet 4      | $8000   8KB          | Sprite bit plane 2 into 7MR             |
   | $08           | Mode7 Root sheet 10           | $2000   8KB          | Mode7 screen data 128x64 tile index     |
   | $08           | Mode7 Root sheet 10           | $4000   8KB          | Mode7 tile data 32 of 16x16 tiles       |
   | $08           | Mode7 Root sheet 10           | $8000   8KB          | Mode7 tile data 32 of 16x16 tiles       |


* Using the pattern file: TestData.txt

	The sprite writes particularly have various configurations for sprite displays, these are commented in/out depending on the desired patterns

	** Comment: Spread out all over the screen
	
	This includes background screen setup, char screen data setup and sprite palette, frames and position setup.
	It provides good variation of sprites, palettes etc across the full range of screen coordinates. This is useful for testing expected masking logic and general visual integrity.


	** Comment: Top left all the same

	This sets all sprites to be in the top left of the screen, it is useful to testing maximum pixel write through and sprite selection logic scenarios. The background is disabled and char screen uses transparent chars. This leaves just the sprite plane outputting pixels.
	Setting a logic break on RV[0..7] = 0xe8 will allow the simulation timing and scan line RAM contents to be inspected in detail.
	The timing of sprite pixel writes into scan RAM 4A/4B and pixel reads from scan RAM 4C/4D with its clear to $ff can be seen whilst single stepping. Note the values for the RV and RH bus lines just below the video display.

	** Comment: X pos group
	
		This regularly spaces the sprites with the same X pos +/-4 pixels. Useful for debugging.
		
	** Comment: Y pos group
		
		This regularly spaces the sprites with the same Y pos +/-4 pixels. Useful for debugging.



* Using the pattern file: TestData2.txt

	Using the ImageToBitplane tool Run/Debug Configuration: Main all conv
	Using GenerateData configuration: Debug2

	This demonstrates stacked multiplexed sprites over a 16x16 tiled background.

	Using the ImageToBitplane tool Run/Debug Configuration: Main old bridge RGB 0 0 0 palette opt rgbfactor 32

	This demonstrates a detailed background picture with 32 palettes of 8 entries each.
	
	Using the ImageToBitplane tool Run/Debug Configuration: Main mode7 "map_9 - mode7 test.png"
		And enabling the data load at: ; Write Mode7 registers
		This will demonstrate how the mode7 export with flipped tile detection works



### Raster line schedule

	_HSYNC continues its pulses on every line, even when _VSYNC is low
	Positive edge of _HSYNC indicates start of the line
	Negative edge of _VSYNC indicates start of the frame
	
	Positive edge of VBLANK indicates the bottom of the visible screen edge
	Note: EXTWANTIRQ = _VBLANK which triggers on the negative edge at raster $f0

	RV = vertical raster line number
		$f8-$ff	lo _VSYNC
		$00-$ff	hi _VSYNC
			$10		lo VBLANK
			$e0		lo ENABLEPIXELS with bordery flag
			$f0		hi VBLANK
	RH = horizontal pixel clock
		Full line starts at $180 to $1ff then $000 to $0ff
		Giving 384 pixel clocks per line
		Visible portion $008 to $188.
		The 8 pixel delay is to sync with the 8 pixel delay for sprite data output into the off scan buffer
	SREAD = Sprite register address (lo byte)
	
	Line RH starts at $180
	$180	SREAD $20
			Begin sprite 0 register reads
			1V*. H
			Note 4A/B are $ff blank
			Note 4C/D have pixel data in the first 8 bytes (16 pixels) then 0xff blank
			hi _HSYNC
	$181	lo ENABLEPIXELS (with borderx flag)
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
			lo ENABLEPIXELS (with no border flags)
	$18aH	4A/B Second pixel written $7b (0 offset MSB byte in the window)
			This is not transparent .011 with palette .1111
	$190	SREAD $24 same 4x2 repeating pattern every 2 pixels
			Begin sprite 1 register reads
			Meaning 16 pixels elapse for each sprite
	$198	4A/B Last transparent pixel $7f written at byte offset $7 (pixel $f) high nybble
	$199H	Begin sprite 1 pixel writes
			Pixel $67 written (0 offset LSB byte in the window)
			This is transparent .111 with palette .1100
	$1b0	lo _HSYNC
	$1d0	hi _HSYNC
	$000	SREAD $40 same 4x2 pattern as above
	$008L	Pixel data arrives at 5E2
	$009H	Pixel data is latched into 5E2
			hi ENABLEPIXELS (with no border flags)
	$00a	Pixels start arriving at the real video output, resistor ladders
			Pixel also cleared in 4C/D
			This clears the pixel just output to the final video palette check since it's obviously not needed
	$00d	hi ENABLEPIXELS (with borderx flag)
	$0fe	SREAD $7f
	Loops back to $180 again and RV increment

#### Sprite scan RAM logic

	Assume the sprite scan line RAM has been cleared by a previous scan rendering all the pixels and clearing the values of $ff.
		The weak pull-up resisters connected to the RAM output help to clear the RAM values to $ff
			These are overidden by any pull down logic 0 from the RAM or the input 74258 demultiplexors
	For each sprite the index, colour, X and Y pos are read
		After a successful Y pos test with the vertical raster position...
			The sprite data counter is set with the X position of the sprite, then for 16 (or 32) pixels
			The data in the scan line RAM is read on _6MHz = 0
			On +ve edge _6MHz this read value is latched by 2C/2D 2B/2A, and inverted for output
			The value is then tested for transparency with 1C:B or 1C:C 3-input NOR gates
			If it is transparent, the value from the input pixel from the current sprite is output by 3C/3D 3B/3A by the 74258 demultiplexors, which inverts the information. This could be a transparent pixel with its colour, or an opaque pixel with its colour.
			If it is not transparent, an opaque sprite pixel exists for this position, then the value just read from the RAM is selected by the 74258 demultiplexors, inverted and written back to the RAM
			In effect, the first opaque (non-transparent) pixel from the sprite written to the scan line RAM has priority and the rest of the pixels from sprites are ignored
		


### PCB Layout

There is a PCB layout design included in the Proteus workspace file. It is deliberately and mostly blank with the exception of adding placement for the external address/data/RW lines, clock, external VBLANK (for IRQ triggering) and video signals output header and the associated resistor ladders. The dual sprite bit plane pull-up resistors are also placed near the edge of the sprites board. The sprite resistors and RAMs are placed to maintain an aesthetic symmetry to the board.

There are five boards in the layout:
	Video generation
	Character screen
	Sprites
	Tiles
	Mode7

Previous versions used one large board, but this turned out to be expensive to produce and difficult to get quotations for. Apparently the companies I contacted were not that happy to produce large boards in small numbers. The design was changed to use four smaller boards, this reduces cost if someone wanted to just fabricate the video and character screen. Another benefit is that the layers can be changed and connected in different orders. This allows customisations, for example by having three sprites boards (layers 1 to 3) and one tile board (layer 4), giving a total of 72 sprites and tiles.

The layout is configured with a six layer standard signal and power plane setup, using the technology template "All PCB Six Layer 1.58532mm.LTF" file. The layout will correctly populate with the auto-placer and default placement options, choose all then schedule before starting. It will then correctly route, again using the default router options. This is so placement can be mostly left to the optimiser while maintaining output video signal lines via the resistor placement.

An entire place and route on a decent machine will take approximately 2 hours and result in an output image like:
![Layout](Layout1.PNG?raw=true "Layout")
![Tracks](Layout2.PNG?raw=true "Tracks")

This board has not yet been built and tested, but it should work. A common cause of failure is probably going to be finding RAMs that have fast enough write timings. Some of these components are old and hard to find.
	
## References

* https://github.com/mamedev/mame/blob/master/src/mame/drivers/bombjack.cpp
* https://floooh.github.io/2018/10/06/bombjack.html
* https://www.arcade-museum.com/game_detail.php?game_id=7180
