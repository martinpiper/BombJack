# Bomb Jack display hardware

* Main social media: https://mastodon.social/@martin_piper
* Don't have a Mastodon account? Click here for an invitation link: https://mastodon.social/invite/nuGxMC8z
* Itch IO project page: https://martin-piper.itch.io/bomb-jack-display-hardware
* YouTube channel: https://www.youtube.com/c/MartinPiper6502
* Buy me a ko-fi: https://ko-fi.com/martinpiper0750
* Bluesky: https://bsky.app/profile/martinpiper.bsky.social
* Twitter links: https://twitter.com/MartinPiper https://twitter.com/hashtag/RetroTTLVideoCard https://twitter.com/hashtag/RetroTTL
* Threads: https://www.threads.net/@martinpiper1187
* Mostly using these tags: #MegaWang #RetroTTL #RetroTTLVideoCard #C64 #Commodore64 #electronics
 
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

Interestingly the Amiga computer, with its custom chips, was designed and prototyped using breadboards: https://arstechnica.com/gadgets/2007/08/a-history-of-the-amiga-part-3/


## Digital display and audio simulation

The arcade hardware uses resistor ladders to convert 4x3 bit RGB+H+V sync digital signals into analog, this is a typical model for the period. I later updated the design to use 15 bit colour with 16x16 palette entries. Since the simulation software is optimised for digital model it makes sense to have a digital display driver simulation that accepts digital signals. Hence I created this Proteus VSM module: https://github.com/martinpiper/DigitalDisplay

This simulation can also be used to debug failures in original boards, since disabling chips or signals in the simulation can produce similar output display artefacts. For example disabling one of the scan line RAM chips (4A/4B/4C/4D) produces alternate pixels and bright lines, similar to the original hardware.

## Why not use a CPLD or FPGA?

Good question! For my thinking, it is a question of what level to want to teach. In software eningeering there are numerous languages, C/C++, Python, Java, Forth, Pascal, etc. There is also machine code and usually some form of assembler mnemonic language which directly maps to machine code, the user accessible low-level language used by a particular CPU. Below that there can be microcode which is often not user accesible, this tends to control the internal state of a CPU. As a software engineer, I see value in using higher level languages, but often use low-level assembler when I want to optimise.

A FPGA or CPLD, while it deals with logical operations, is usually programmed via a higher level language (VHDL or Verilog or similar) abstraction above the low-level logic cells actually used by the device, often (not always) the same code can be compiled for subtly different devices with minimal changes.

7400-series discrete logic, specifically 74LS series logic used in this project, can be considered to be low-level, analogous to using machine code or assembler. There is a lower level, which is using discrete transistors to create logic cells, like NAND, OR, NOT, etc. but that really is a low level more like microcode.

The use of high level or low-level programming is a contextual decision, often my knowledge of low-level architecture and performace informs my decision making processes when using high level languages. While it is certainly possible to code an application entirely with only high level language knowledge, it is an advantage to know low-level architecture to help debug hopefully rare performance or error situations.

All that said, I believe that knowing how to engineer a solution using low-level discrete logic is an advantage, in other words it helps to improve the use of higher level CPLD or FPGA engineering to know lower level architecture. This is why this project exists, to help teach how to solve engineering problems using low-level discrete logic.

## Developer technical details

To produce the test data run: BDD6502\ForUnitTests.bat

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
4.	Sprite logic - Part 1
	1. Sprite RAM register access logic, timing with the external address/data bus and video internal timings
	2. Temporary storage for sprite position, palette and frame
	3. Bit plane access and bit shifters for output pixels to scan RAM
	4. 32x32 sprite selection registers and comparison
	5. Various signal timings generated for sprite loading, tile and colour fetch etc
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
11.	Scaled sprite logic - Part 1
	1. Register memory
	2. Sprite extent and scan line detection
	3. Sprite flipping
12.	Scaled sprite logic - Part 2
	1. Scanline - Two alternating banks
	2. Sprite definition RAM
13.	Vector logic
	1. Register logic - Bank display and kill
	2. Scan RAM banks - Two banks
14.	Pixel data combiner (merge layer) logic
	1. 2-1 pixel data combiners - Two blocks


### How to access the hardware

In the original hardware, used in the Bombjack arcade game, the Z80 data and its address bus was partially connected to the hardware with a couple of address windows at $9000 to $9fff (perhaps a bit wider than that), this was mostly write RAM as far as the Z80 was concerned. It was sprite registers, some control registers, palette and char screen RAM. The rest of the video internal memory was ROM, and could not be addressed by the Z80. The Z80 would have to time writes to the video address space (RAM) during the VBLANK when the video hardware was not expecting to read RAM to avoid contention.


For this updated hardware, the C64 (or a Z80) sends data bytes via the user port (parallel transfer, so it's fast, one clock cycle to write), with some control signals, to write to a 24 bit address space in the hardware. This is usually accomplished by the UserPortTo24BitAddress.pdsprj (or better) project. Basically a control signal resets the internal state, the next three bytes are the address to write, then next bytes are consecutively written to the address space until the internal state is reset again. There is a signal from the hardware to the C64 via the user port for screen refresh (VBLANK) which is read as an NMI (CIA2 FLAG pin) request on the C64. This means there is no bus contention with the C64 memory bus to the hardware. Writes to hardware RAM take precedence over what the hardware is reading from the RAM, this ensures data is written without contention. Writes to the hardware which involve RAM, rather than register latches, should usually be timed to coincide with VBLANK or HBLANK to avoid visual glitches. Each layer (chars, tiles, sprites etc) provides information on any RAM timing constraints.


### Memory map

Note: Most control registers can be updated at any point during the frame and the next pixel will reflect the change.

0x9000			Chars control register
		<unused>

0x9c00-0x9dff	Palette RAM


0x9e00	0x10	Use background colour
		0x20	Enable display
		0x80	BorderY shrink

0x9e08	Layer priority select, for each layer select the input header pixel input
		Default should be: %11100100 = 0xe4
		i.e. Back most layer	= %11 (3)
		Next layer closer		= %10 (2)
		Then next closest		= %01 (1)
		Then front most layer	= %00 (0)

0x9e09	0xlg	Setup nybble extents for overscan to enable the border (g = greater than, and l = less than) in 8 pixel steps
	Overscan mode is always on, so this needs to be set to sensible values
	Sensible wide value for this is $29 which hides the left edge (including 16x16 tiles) and brings in the right edge in slightly
	This gives a 320 pixel wide screen, displaying a full width of 40 8x8 characters
	

0x9e0a	Video layer enable flags. These are passed through the VideoX connectors. Each layer has the option of reading their enable flag and taking appropriate action.
		Bits	Action				Default layer assigned to this connector (can be changed)		Replaces logic
		0x01	Enable layer 1		Sprites (or Sprites2)											0x9a00 Upper nybble: $10 = Enable sprite output (6S SPREN)
		0x02	Enable layer 2		Chars															0x9000 Chars control register	0x02	Chars screen disable
		0x04	Enable layer 3		Tiles															0x9e00	0x10	Enable tiles
		0x08	Enable layer 4		Background or mode7												0xa015	: Flags	:	0 : Enable display

0x9e0b	Background colour
	If the final pixel is transparent and 0x9e00 0x10 is set, then the palette information is read from this colour value
	If the final pixel is transparent and 0x9e00 0x10 is not set, then the palette information is read from the last layer colour value

0x9e0c	Palette bank number
	See: kBus24Bit_VideoLayer_ExpandedPalettes


Sprite 32x32 size select
	0x9a00 start index of 32x32 sprites
	0x9a01 end index of 32x32 sprites (exclusive)
		So 0,0 = no 32x32 sprites
		0,8 means indexes 2 to 7 (exclusive) are 32x32
		Maximum number in both is 0xf



At 0x9800 - 0x985f each sprite is described by 4 bytes:

	Byte 0:
		The tile code for the sprite, used to look up the sprite’s image bitplanes in the tile ROMs
		(MAME Emu documentation is wrong, bit 7 has nothing to do with selecting double size mode)

	Byte 1: HVFMCCCC
		Bit 7: H : If set, the sprite is horizontally flipped
		Bit 6: V : If set, the sprite is vertically flipped
		Bit 5: F : If set, the sprite is full screen height. The sprite data will repeat.
		Bit 4: MSBX
		Bits 3..0: C : 4 bits to provide the colour value for the tile decoder

	Byte 2: The sprite’s Y position on screen
	Byte 3: The sprite’s X position on screen


0x9200 Sprites2 registers
	Sprites support X and Y flips with X & Y repeating patterns

	Byte 0: Palette | 0x10 = MSBX | 0x20 = MSBY | 0x40 = flipX | 0x80 = flipY
	Byte 1: Y pos
	Byte 2: Y size (in screen pixels, regardless of scale) (0 = end of list)
	Byte 3: X pos
	Byte 4: X scale extent (uses internal coordinates)
	Byte 5: Y inv scale (*32)
	Byte 6: X inv scale (*32)
	Byte 7: Sprite frame (index) | 0x40 = halfX | 0x80 = halfY


#### Tile and sprite data layout
16x16 pixel tile and sprite data is stored in 8x8 cells and in memory order:
	01
	23

For 32x32 sprites, this arrangement is expanded in memory as:
	0145
	2367
	89cd
	abef



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

Note: The internal accumulated values, x/xy/y/yx are not accessible via registers and are reset only by _EHSYNC and _EVSYNC

For each horizontal pixel (on +ve 6MHz):

	x += dx
	yx += dyx

	xo = x + xy + xorg
	yo = y + yx + yorg

For each scanline (on +ve _EHSYNC):

	xy += dxy
	y += dy

For each scanline (on _EHSYNC):

	yx = 0
	x = 0

For each frame (on _EVSYNC):

	y = 0
	xy = 0

All of the above is one way to progressively calculate xo,yo using addition. In other words:

	xo = (dx*x) + (dxy*y) + xorg
	yo = (dy*y) + (dyx*x) + yorg

During the coordinate (xo,yo) to screen transformation: bits 8-11 are used to lookup the tile pixel, bits 12-18 for the x screen tile index position, bits 12-17 for the y screen tile index position.

Note: The full, but hidden by borders, screen resolution is 384x264 pixels.

0xa014	: Background colour, any pixel index of zero in the mode7 tile data will show the background colour
0xa015	: Flags	:	0 : Enable X update, or reset to 0
					1 : Enable XY update, or reset to 0
					2 : Enable Y update, or reset to 0
					3 : Enable YX update, or reset to 0



### Audio

Current audio hardware has 4 voices, each using 11 bytes. Extra bytes are used for *currently unused byte* and voice active bit masks after the voice registers.

Each voice register layout is as follows:
	byte	register
	0		volume
	1/2		address lo/hi
	3/4		length lo/hi
	5/6		rate lo/hi
	7/8		loop address lo/hi
	9/10	loop length lo/hi

The sample frequency in hertz to voice rate conversion, using internal 4MHz clock, is:  (4096 * hertz) / 25000



	
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

For EBBS = $01 the RAM address select lines are from $8000 until $b800 in steps of $800


   | EXPANSIONBUS3 | Group                         | EXPANSIONBUS address | Behaviour                               |
   |---------------|-------------------------------|----------------------|-----------------------------------------|
   | $01           | Audio registers               | $8000 11 bytes/voice | Audio registers                         |
   | $01           | Original RAMs                 | $9000                | Char screen control                     |
   | $01           | Original RAMs                 | $9001/2              | Char screen X scroll                    |
   | $01           | Original RAMs                 | $9003/4              | Char screen Y scroll                    |
   | $01           | Sprite2 registers             | $9200-$92ff          | Sprite2 registers                       |
   | $01           | Original RAMs                 | $9800   $60 bytes    | Sprite registers                        |
   | $01           | Original RAMs                 | $9c00   $200 bytes   | Palette GR XB 2 bytes per entry         |
   | $01           | Original RAMs                 | $9e00                | Background image enable and borders     |
   | $01           | Original RAMs                 | $9e01/2              | Background image XL/XH pixel scroll     |
   | $01           | Original RAMs                 | $9e03/4              | Background image YL/YH pixel scroll     |
   | $01           | Original RAMs                 | $9e07                | Background colour select                |
   | $01           | Original RAMs                 | $9e08                | Layer priority select                   |
   | $01           | Original RAMs                 | $9a00-$9a01          | Start/end 32x32 sprite index 0-f only   |
   | $01           | Extension mode7 registers     | $a000-$a017          | Mode7 registers                         |
   | $01           | Vectors                       | $a100   1 byte       | Vectors display bank 					|
   | $01           | Combiner                      | $a200   4 bytes      | Combiner display bank 					|
   | Note: APU and vectors are by default using the same EBBS in the schematics										|
   | $02           | APU                           | $8000   8KB          | APU memory bank 0 instruction           |
   | $02           | APU                           | $4000   8KB          | APU memory bank 1 data                  |
   | $02           | APU                           | $2000   8KB          | APU bank 2 registers                    |
   | $02           | Vectors                       | $0000   16KB         | Vectors bank 0 data						|
   | $02           | Vectors                       | $8000   16KB         | Vectors bank 1 data						|
   | Note: Audio and Sprites2 are by default using the same EBBS in the schematics									|
   | $04           | Audio memory                  | $0000-$ffff          | Audio sample memory                     |
   | $04           | Sprites2 data                 | $2000   8KB++        | Sprites2 bit planes                     |
   | $05           | Sprites4 data                 | $0000   64KB++       | Sprites2 bit planes, plus extra address |
   | $80           | Background 16x16 Root sheet 7 | $2000   8KB          | Tiles and colours into 4P7R             |
   | $80           | Char screen                   | $4000   8KB          | Chars into U258                         |
   | $80           | Char screen colours           | $8000   8KB          | Chars colours into U137                 |
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
	Setting a logic break on RV[0..8] = 0xe8 will allow the simulation timing and scan line RAM contents to be inspected in detail.
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

	Counting from 0 (in computer terms), the first visible display line is 17 and the last visible line is 240, and with the Y border shrink it is 224.

	_EHSYNC continues its pulses on every line, even when _EVSYNC is low
	Positive edge of _EHSYNC indicates start of the line
	Negative edge of _EVSYNC indicates start of the frame
	
	Positive edge of VBLANK indicates the bottom of the visible screen edge
	Note: EXTWANTIRQ = _VBLANK which triggers on the negative edge at raster $f0

	RV = vertical raster line number
		$f8-$ff	lo _EVSYNC
		$00-$ff	hi _EVSYNC
			$10		lo VBLANK
			$e0		lo FINALENABLEPIXELS with bordery flag
			$f0		hi VBLANK
	RH = horizontal pixel clock
		Full raster line starts at $0 to $17e
			In reality, RH $0 occurs for two clocks, each subsequent pixel advances with one clock, up to a maximum of $17e
		Giving 384 pixel clocks per line
		Maximum visible portion (when _EHSYNC = 1) is $000 to $15f
		_EHSYNC = 0 (HBLANK portion of the display where nothing is visible) is $160 to $17e (plus the first half of the next $000)

	SREAD = Sprite register address (lo byte, also maps to ISAB[0..6] in the schematic)
	
	Line RH starts at $000
	$000	SREAD $00
			Begin sprite 0 register reads
			1V*. H
			lo to hi _EHSYNC (in second clock for $000)
	$002	SREAD $01
	$004	SREAD $02
	$006	SREAD $03
	$008 	SREAD $00 to $03 again
	$008H	OV0/1/2 LHH OC0/1/2/3 HLHL from M0-M7
	$008L	All L no data from $008H clocked into pixel output latch at 5E2
	$009H	Pixel data is latched into 5E2
	$009H	Begin sprite 0 pixel writes
			U77 First pixel written
	$00aH	U77 Second pixel written
	$00a	Pixels start arriving at the real video output, resistor ladders
			Pixel also cleared to $ff in U78
	$010	SREAD $04 same 4x2 repeating pattern every 2 pixels
			Begin sprite 1 register reads
			Meaning 16 pixels elapse for each sprite
	$012	lo to hi FINALENABLEPIXELS (with overscan $29)
	$0c0	SREAD $30 same 4x2 pattern as above
	$152	hi to lo FINALENABLEPIXELS (with overscan $29)
	$160	lo _EHSYNC
	$17e	hi _EHSYNC (into first half of $000)
	$17e	SREAD $7f
	Loops back to $000 again and RV increment


	The file "BombJack\RecordingRHSprAddr.txt" contains recorded address and RH values from Root Sheet 4 device VSMDD5.
	This clearly shows (sparse extracts)
		Recording		Index	Address	@RH
		d$00000000		0		$00		$000
		d$00100040		4		$10		$040
		d$00200080		8		$20		$080
		d$003000c0		12		$30		$0c0
		d$00400100		16		$40		$100
		d$00500140		20		$50		$140


#### Sprite scan RAM logic

	A sprite in 16x16 mode, with a Y register value of 224, which uses internal inverted logic, will have a calculated sprite Y coordinate of 256 - 224 = 32, this will be the last calculated scan line of the sprite. The result of the scan line calculation will be rendered and visible on the next scan line 33.
	15 scan lines above calculated scan line 32 is the first calculated scan line of the sprite, in this example scan line 17, and the next scan line 18 is the first visible scan line of the sprite.


	Assume the sprite scan line RAM has been cleared by a previous scan rendering all the pixels and clearing the values of $ff.
		Old revision: The weak pull-up resisters connected to the RAM output help to clear the RAM values to $ff
			These are overidden by any pull down logic 0 from the RAM or the input 74258 demultiplexors
		New revision: Uses digital logic to switch between $ff and input values as needed. This is much more stable than using pull-up resistors which needed to be tuned for the actual TTL chips used.
	For each sprite the index, colour, X and Y pos are read
		After a successful Y pos test with the vertical raster position...
			The sprite data counter is set with the X position of the sprite, then for 16 (or 32) pixels
			The data in the scan line RAM is read on _6MHz = 0
			On +ve edge _6MHz this read value is latched by 2C/2D 2B/2A, and inverted for output
			The value is then tested for transparency with 1C:B or 1C:C 3-input NOR gates
			If it is transparent, the value from the input pixel from the current sprite is output by 3C/3D 3B/3A by the 74258 demultiplexors, which inverts the information. This could be a transparent pixel with its colour, or an opaque pixel with its colour.
			If it is not transparent, an opaque sprite pixel exists for this position, then the value just read from the RAM is selected by the 74258 demultiplexors, inverted and written back to the RAM
			In effect, the first opaque (non-transparent) pixel from the sprite written to the scan line RAM has priority and the rest of the pixels from sprites are ignored


	Timing enabling or disabling the sprite display and updating sprite registers allows multiplexing. See: kBus24Bit_VideoLayer_XPos_SpriteIndex_*
	The sprite scan line schedule, which renders each sprite in 16 pixel chunks, can be syncronised to the video pixel clock, or overclocked.
	When syncronised to the video pixel clock each 16x16 sprite takes 16 pixels to process. If the sprite clock is overclocked, the scan schedule will correspondingly contract by the ratio of video clock to sprite clock. In other words each sprite will take fewer than 16 pixels to process.
	Overclocking the sprite pixel clock will alter the timing for sprite multiplexing: kBus24Bit_VideoLayer_XPos_SpriteIndex_*


	Remember, the scanline before a sprite is visible/displayed on the screen is the actual scanline used for rendering the sprite data.
		The x position of the sprite does not affect its rendering timing in the scanline, only the sprite register address used affects its rendering schedule time, see: Raster line schedule

### APU - What is it

The Advanced Processing Unit is an optimised raster and data transfer processor that can execute several instructions at the same time.
It is able to precisely wait for raster H&V positions, transfer data, and perform simple comparisons based on internal data with conditional executions based on the results of the comparison.
This significantly reduces load on the CPU and the amount of memory the CPU needs, allowing the CPU to transfer data when it can.

There is significant logic internally that allows data to be processed: https://docs.google.com/presentation/d/1Tb4B5-INhH2oiOSG4jfb3BfGGtdT9jPvyWoryOIJNzo/edit?usp=sharing



### PCB Layout

There is a PCB layout design included in the Proteus workspace file. It is deliberately and mostly blank with the exception of adding placement for the external address/data/RW lines, clock, external VBLANK (for IRQ triggering) and video signals output header and the associated resistor ladders. The dual sprite bit plane pull-up resistors are also placed near the edge of the sprites board. The sprite resistors and RAMs are placed to maintain an aesthetic symmetry to the board.

There are six boards in the layout:
	Video generation
	Character screen
	Sprites
	Scaled sprites
	Tiles
	Mode7

Previous versions used one large board, but this turned out to be expensive to produce and difficult to get quotations for. Apparently the companies I contacted were not that happy to produce large boards in small numbers. The design was changed to use four smaller boards, this reduces cost if someone wanted to just fabricate the video and character screen. Another benefit is that the layers can be changed and connected in different orders. This allows customisations, for example by having three sprites boards (layers 1 to 3) and one tile board (layer 4), giving a total of 72 sprites and tiles.

The layout is configured with a six layer standard signal and power plane setup, using the technology template "All PCB Six Layer 1.58532mm.LTF" file. The layout will correctly populate with the auto-placer and default placement options, choose all then schedule before starting. It will then correctly route, again using the default router options. This is so placement can be mostly left to the optimiser while maintaining output video signal lines via the resistor placement.

An entire place and route on a decent machine will take approximately 2 hours and result in an output image like:
![Layout](Layout1.PNG?raw=true "Layout")
![Tracks](Layout2.PNG?raw=true "Tracks")

The protoype boards have been built and documented here: https://www.youtube.com/playlist?list=PLAAYJEX1JbCPFzQDl8KOoVQR9_Us1lB46 or this project page: https://martin-piper.itch.io/bomb-jack-display-hardware


I use Proteus simulation and PCB design: https://www.labcenter.com/
And PCBWay PCB manufacture: https://pcbway.com/g/Xr8ZrI
( Using the PCBWay referral link gives you a discount and also helps me reduce the cost of my next board. )

	
## References

* https://github.com/mamedev/mame/blob/master/src/mame/drivers/bombjack.cpp
* https://floooh.github.io/2018/10/06/bombjack.html
* https://www.arcade-museum.com/game_detail.php?game_id=7180
