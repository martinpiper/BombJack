# BombJack

This is a working schematic for the video display portion of the original Bomb Jack arcade hardware. Why bother, you're probably asking? Well if you have to ask then perhaps you're not the right audience. :)

## Background

This project started when old retro arcade hardware was being discussed. I was looking at Bomb Jack boards on ebay and pondering how they had enough fill-rate to draw 24 16x16 sprites and have the option for some to use 32x32 mode as well. A friend and I were discussing the clock speed and fill-rate while trying to deduce the operation of the hardware just by inspecting the hand drawn schematics, as you do.

In the end to get some clarity on the sprite plotting specifically I started to transcribe what was thought to be the sprite logic portion of the schematic into Proteus, since it can simulate digital electronics really well.

## Digital display simulation

The arcade hardware uses resistor ladders to convert 4x3 bit RGB+H+V sync digital signals into analog, this is a typical model for the period. Since the simulation software is optimised for digital model it makes sense to have a digital display driver simulation that accepts digital signals. Hence I created this Proteus VSM module: https://github.com/martinpiper/DigitalDisplay


## Developer technical details

If you have Proteus setup with the digital display model, you should see a screen that looks somewhat like this when starting the simulation:

![Screenshot](Capture.PNG?raw=true "Screenshot")

### High level schematic sheets

The Proteus sheet numbers correspond to the original schematic page numbers as written in the bottom right hand corner of each page. The layout of this schematic roughly matches the layout of the original schematic. So for example the analog RGB resistor ladders are on "Root sheet 8" in the top right of the sheet, as in the original schematic page 8 layout.

1. Logic analyser, virtual video display model, expansion bus header, pattern generators and associated debug memory write logic
	1. Memory selection logic based on external address bus
	2. Debug buttons for the video flip and spare background plane selection
2. Intentionally left blank, the original schematic has dipswitch logic
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
7. Read only background pixture logic
	1. Logic for selection of approriate picture, based on high address lines
	2. Bit plane reads and pixel shifters
	3. Output pixel logic for possible video display, or disable background logic
8. Output pixel pipeline and palette lookup
	1. Pixel transparency test from background, character screen and sprites
	2. Palette RAMs with external bus logic
	3. Final pixel latching logic and palette RAM lookup
	4. Output digital to analog conversion using resistor ladders
	5. Analog RGB and associated sync signal output header
	

### Memory map

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
		Bits 3..0: C : 4 bits to provide the color value for the tile decoder

	Byte 2: The sprite’s X position on screen
	Byte 3: The sprite’s Y position on screen


	
### Clock speeds

The original schematic uses a 6MHz clock for all the video hardware, as denoted by the "6MHz" signal line. The clocking can be found on "Root sheet 1" just about the logic analyser.

However the Proteus simulation uses 1MHz on this signal line, I kept the naming of the line the same as the original schematic however. This is due to the default RAM write timings for ICs 4A/4B/4C/4D being too tight. This means the digital display driver will detect ~10fps, not ~60fps as per the original design. It does however make the debug single step time easier to think about since it's not divided by 6MHz...

Remember to use:

	System->Set Animation Options
		Single Step Time: 83n
		500n For debugging full clock cycle and pixel clock


### Where is the Z80 CPU?

The Z80 CPU from the original schematic is not included, it was clocked independently and has its own RAM, except for the video hardware interfaces and isn't needed for the video simulation.


### Input data setup

The Z80 data write signals are mocked using a combination of simulator pattern generator and some simple timing logic, this is separate to the main video schematic and the components are excluded from the PCB layout.

As per the orignal design all writes to the video hardware should be timed to coincide with the VBLANK. This is because the video hardware is almost always reading the RAM during the visible portion of the frame. Writing to the sprite registers outside the VBLANK will especially produce nasty looking effects on the screen. This RAM sharing model is quite common is old arcade and console hardware.


* Using the pattern file: 9800.ptn

	This includes background screen setup, char screen data setup and sprite palette, frames and position setup.
	It provides good variation of sprites, palettes etc across the full range of screen coordinates. This is useful for testing expected masking logic and general visual integrity.


* Using the pattern file: 9800 top left all the same.ptn

	This sets all sprites to be in the top left of the screen, it is useful to testing maximum pixel write through and sprite selection logic scenarios. The background is disabled and char screen uses transparent chars. This leaves just the sprite plane outputting pixels.
	Setting a logic break on RV[0..7] = 0xe8 will allow the simulation timing and scan line RAM contents to be inspected in detail.
	The timing of sprite pixel writes into scan RAM 4A/4B and pixel reads from scan RAM 4C/4D with its clear ot $ff can be seen whilst signle stepping. Note the values for the RV and RH bus lines just below the video display.

	

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

	
	
## References

* https://github.com/mamedev/mame/blob/master/src/mame/drivers/bombjack.cpp
* https://floooh.github.io/2018/10/06/bombjack.html
* https://www.arcade-museum.com/game_detail.php?game_id=7180
