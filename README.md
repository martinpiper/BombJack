# Bomb Jack display hardware

This started life as a working schematic for the video display portion of the original Bomb Jack arcade hardware. Why bother, you're probably asking? Well if you have to ask then perhaps you're not the right audience. :)

The version 1.0 of this work was approximately interface pin and chip compatible with the original hardware schematic. Now since V2.0 the hardware has progressed to add extra features not present in the original, such as addressable RAM instead of ROMs, extra display blanking capability, X and Y pixel scroll logic added etc.

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

### High level schematic sheets

The Proteus sheet numbers correspond to the original schematic page numbers as written in the bottom right hand corner of each page. The layout of this schematic roughly matches the layout of the original schematic. So for example the analog RGB resistor ladders are on "Root sheet 8" in the top right of the sheet, as in the original schematic page 8 layout.

1. Logic analyser, virtual video display model, expansion bus header, data generator
	1. Memory selection logic based on external address bus
	2. Debug button for the spare background plane selection
	3. VSMDD2 is a replacement for the Proteus data generators because they have a 1024 byte limit. Proteus VSM project source: https://github.com/martinpiper/DigitalData
	4. EXPANSIONBUS, EXPANSIONBUS2 and EXPANSIONBUS3 all relate to the C64 user port to 24 bit address logic in the project: UserPortTo24BitAddress.pdsprj
2. Intentionally left blank, the original schematic has dip switch logic
3. Video timing
	1. Horizontal video signal generation
	2. Vertical video signal generation
	3. HBLANK and VBLANK signal generation
	4. 32x32 sprite selection registers and comparison
	5. Various signal timings generated for sprite loading, tile and colour fetch etc
4. Sprite logic - Part 1
	1. Sprite RAM register access logic, timing with the external address/data bus and video internal timings
	2. Temporary storage for sprite position, palette and frame
	3. Bit plane access and bit shifters for output pixels to scan RAM
5. Sprite logic - Part 2
	1. Dual scan line RAM buffers
	2. Transparent pixel test logic based on inverted pixel temporary storage and selection with NOR check
	3. Pixel writing to RAM based on 16 pixel chunks and position register contents
	4. Logic for timing of pixel reads for possible video display, plus clearing of data just read
6. Character screen RAM with associated colour screen
	1. Associated external bus interface logic
	2. Bit plane reads and pixel shifters
	3. Output pixel logic for possible video display
7. Read only background picture logic
	1. Logic for selection of appropriate picture, based on high address lines
	2. Bit plane reads and pixel shifters
	3. Output pixel logic for possible video display, or disable background logic
8. Output pixel pipeline and palette lookup
	1. Pixel transparency test from background, character screen and sprites
	2. Palette RAMs with external bus logic
	3. Final pixel latching logic and palette RAM lookup
	4. Output digital to analog conversion using resistor ladders
	5. Analog RGB and associated sync signal output header
	

### Memory map

0x9000-0x93ff	Video RAM
0x9400-0x97ff	Colour RAM
0x9c00-0x9cff	Palette RAM

0x9e00 xy		Background screen image enable (x) and index (y)


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

	Byte 1: HV..CCCC
		Bit 7: H : If set, the sprite is horizontally flipped
		Bit 6: V : If set, the sprite is vertically flipped
		Bits 3..0: C : 4 bits to provide the colour value for the tile decoder

	Byte 2: The sprite’s X position on screen
	Byte 3: The sprite’s Y position on screen


	
### Clock speeds

The original schematic uses a 6MHz clock for all the video hardware, as denoted by the "6MHz" signal line. The clocking can be found on "Root sheet 1" just about the logic analyser.

However the Proteus simulation uses 1MHz on this signal line, I kept the naming of the line the same as the original schematic however. This is due to the default RAM write timings for ICs 4A/4B/4C/4D being too tight. This means the digital display driver will detect ~10fps, not ~60fps as per the original design. It does however make the debug single step time easier to think about since it's not divided by 6MHz...

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

The Z80 data write signals are mocked using a simulator pattern generator VSMDD2, this is separate to the main video schematic and the generator is are excluded from the PCB layout.

As per the original design all writes to the video hardware should be timed to coincide with the VBLANK. This is because the video hardware is almost always reading the RAM during the visible portion of the frame. Writing to the sprite registers outside the VBLANK will especially produce nasty looking effects on the screen. This RAM sharing model is quite common is old arcade and console hardware.

The original hardware has been expanded to include RAMs where the ROMs were located. These are addressed by an combination of EXPANSIONBUS3 to select the groups of RAMs and the EXPANSIONBUS group selector. It is entirely possible to write more than one group at a time by enabling multiple output bits in EXPANSIONBUS3.

   | EXPANSIONBUS3 | Group                         | EXPANSIONBUS address | Behaviour                               |
   |---------------|-------------------------------|----------------------|-----------------------------------------|
   | $01           | Original RAMs                 | $9000   $400 bytes   | Screen character index                  |
   | $01           | Original RAMs                 | $9400   $400 bytes   | Screen colour                           |
   | $01           | Original RAMs                 | $9820   $60 bytes    | Sprite registers                        |
   | $01           | Original RAMs                 | $9c00   $100 bytes   | Palette GR XB 2 bytes per entry         |
   | $01           | Original RAMs                 | $9e00                | Background image enable, index, borders |
   | $01           | Original RAMs                 | $9e01                | Background image X/Y pixel scroll       |
   | $01           | Original RAMs                 | $9a00-$9a01          | Start/end 32x32 sprite index 0-f only   |
   | $80           | Background 16x16 Root sheet 7 | $2000   8KB          | Tiles and colours into 4P7R             |
   | Note 2 spare  |                               |                      |                                         |
   | $40           | Background 16x16 Root sheet 7 | $2000   8KB          | Tiles bit plane 0 into 8R7R             |
   | $40           | Background 16x16 Root sheet 7 | $4000   8KB          | Tiles bit plane 1 into 8N7R             |
   | $40           | Background 16x16 Root sheet 7 | $8000   8KB          | Tiles bit plane 2 into 8L7R             |
   | $20           | Char screen 8x8 Root sheet 6  | $2000   8KB          | Char data bit plane 0 into 8K6R         |
   | $20           | Char screen 8x8 Root sheet 6  | $4000   8KB          | Char data bit plane 1 into 8H6R         |
   | $20           | Char screen 8x8 Root sheet 6  | $8000   8KB          | Char data bit plane 2 into 8E6R         |
   | $10           | Sprite data Root sheet 4      | $2000   8KB          | Sprite bit plane 0 into 7JR             |
   | $10           | Sprite data Root sheet 4      | $4000   8KB          | Sprite bit plane 1 into 7LR             |
   | $10           | Sprite data Root sheet 4      | $8000   8KB          | Sprite bit plane 2 into 7MR             |



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
	

### Raster line schedule

	RV = vertical raster line number
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
	Loops back to $180 again and RV increment


### PCB Layout

There is a PCB layout design included in the Proteus workspace file. It is deliberately and mostly blank with the exception of adding placement for the external address/data/RW lines, clock, external VBLANK (for IRQ triggering) and video signals output header and the associated resistor ladders. The dual sprite bit plane pull-up resistors are also placed near the middle of the board.

The board edge is set with a four layer standard signal and power plane setup. The board will correctly populate with the auto-placer and default placement options, choose all then schedule before starting. It will then correctly route, again using the default router options. This is so placement of the board can be mostly left to the optimiser while maintaining output video signal lines via the resistor placement. The placement areas are to encourage areas of the board with high placement density to leave space for signals.

An entire place and route on a decent machine will take approximately 20 minutes and result in an output image like:
![Layout](Layout.PNG?raw=true "Layout")

This board has not yet been built and tested, but it should work. A common cause of failure is probably going to be finding RAMs that have fast enough write timings. Some of these components are old and hard to find.
	
## References

* https://github.com/mamedev/mame/blob/master/src/mame/drivers/bombjack.cpp
* https://floooh.github.io/2018/10/06/bombjack.html
* https://www.arcade-museum.com/game_detail.php?game_id=7180
