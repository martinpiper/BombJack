//
// pigfx.c
// Main part
//
// PiGFX is a bare metal kernel for the Raspberry Pi
// that implements a basic ANSI terminal emulator with
// the additional support of some primitive graphics functions.
// Copyright (C) 2014-2020 Filippo Bergamasco, Christian Lehner

#include "peri.h"
#include "pigfx_config.h"
#include "uart.h"
#include "utils.h"
#include "c_utils.h"
#include "timer.h"
#include "console.h"
#include "gfx.h"
#include "framebuffer.h"
#include "irq.h"
#include "dma.h"
#include "nmalloc.h"
#include "ee_printf.h"
#include "prop.h"
#include "board.h"
#include "mbox.h"
#include "actled.h"
#include "emmc.h"
#include "mbr.h"
#include "fat.h"
#include "config.h"
#include "keyboard.h"
#include "ps2.h"
#include "memory.h"
#include "mmu.h"
#include "C64UserPort24Bit.h"
#include "synchronize.h"

#define UART_BUFFER_SIZE 16384 /* 16k */


unsigned int led_status = 0;
unsigned char usbKeyboardFound = 0;
unsigned char ps2KeyboardFound = 0;
volatile unsigned int* pUART0_DR;
volatile unsigned int* pUART0_ICR;
volatile unsigned int* pUART0_IMSC;
volatile unsigned int* pUART0_FR;

volatile char* uart_buffer;
volatile char* uart_buffer_start;
volatile char* uart_buffer_end;
volatile char* uart_buffer_limit;

tPiGfxConfig PiGfxConfig;

extern unsigned int pheap_space;
extern unsigned int heap_sz;

extern unsigned char G_STARTUP_LOGO;


static void _heartbeat_timer_handler( __attribute__((unused)) unsigned hnd,
                                      __attribute__((unused)) void* pParam,
                                      __attribute__((unused)) void *pContext )
{
    if( led_status )
    {
        led_set(0);
        led_status = 0;
    } else
    {
        led_set(1);
        led_status = 1;
    }

    attach_timer_handler( HEARTBEAT_FREQUENCY, _heartbeat_timer_handler, 0, 0 );
}


void uart_fill_queue( __attribute__((unused)) void* data )
{
    while( !( *pUART0_FR & 0x10 ))
    {
        *uart_buffer_end++ = (char)( *pUART0_DR & 0xFF );

        if( uart_buffer_end >= uart_buffer_limit )
           uart_buffer_end = uart_buffer;

        if( uart_buffer_end == uart_buffer_start )
        {
            uart_buffer_start++;
            if( uart_buffer_start >= uart_buffer_limit )
                uart_buffer_start = uart_buffer;
        }
    }

    /* Clear UART0 interrupts */
    *pUART0_ICR = 0xFFFFFFFF;
}


void initialize_uart_irq()
{
    uart_buffer_start = uart_buffer_end = uart_buffer;
    uart_buffer_limit = &( uart_buffer[ UART_BUFFER_SIZE ] );

    pUART0_DR   = (volatile unsigned int*)UART0_DR;
    pUART0_IMSC = (volatile unsigned int*)UART0_IMSC;
    pUART0_ICR = (volatile unsigned int*)UART0_ICR;
    pUART0_FR   = (volatile unsigned int*)UART0_FR;

    *pUART0_IMSC = (1<<4); // Masked interrupts: RXIM (See pag 188 of BCM2835 datasheet)
    *pUART0_ICR = 0xFFFFFFFF; // Clear UART0 interrupts

    irq_attach_handler( 57, uart_fill_queue, 0 );
}


/** Sets the frame buffer with given width, height and bit depth.
 *   Other effects:
 *  	- font is set to 8x16
 *  	- tabulation is set to 8
 *  	- chars/sprites drawing mode is set to normal

 */
void initialize_framebuffer(unsigned int width, unsigned int height, unsigned int bpp)
{
    fb_release();

    unsigned char* p_fb=0;
    unsigned int fbsize;
    unsigned int pitch;

    unsigned int p_w = width;
    unsigned int p_h = height;
    unsigned int v_w = p_w;
    unsigned int v_h = p_h;

    fb_init( p_w, p_h,
             v_w, v_h,
             bpp,
             (void*)&p_fb,
             &fbsize,
             &pitch );

    if (fb_set_palette(0) != 0)
    {
#if ENABLED(FRAMEBUFFER_DEBUG)
        cout("Set Palette failed"); cout_endl();
#endif
    }

    gfx_set_env( p_fb, v_w, v_h, bpp, pitch, fbsize );
    gfx_set_drawing_mode(drawingNORMAL);
    gfx_term_set_tabulation(8);
    gfx_term_set_font(8,16);
    gfx_clear();
}


void video_test(int maxloops)
{
    unsigned char ch='A';
    unsigned int row=0;
    unsigned int col=0;
    unsigned int term_cols, term_rows;
    gfx_get_term_size( &term_rows, &term_cols );

#if 0
    unsigned int t0 = time_microsec();
    for( row=0; row<1000000; ++row )
    {
        gfx_putc(0,col,ch);
    }
    t0 = time_microsec()-t0;
    cout("T: ");cout_d(t0);cout_endl();
    return;
#endif
#if 0
    while(1)
    {
        gfx_putc(row,col,ch);
        col = col+1;
        if( col >= term_cols )
        {
            usleep(100000);
            col=0;
            gfx_scroll_up(8);
        }
        ++ch;
        gfx_set_fg( ch );
    }
#endif
#if 1
    int count = maxloops;
    while(count >= 0)
    {
    	count--;
        gfx_putc(row,col,ch);
        col = col+1;
        if( col >= term_cols )
        {
            usleep(50000);
            col=0;
            row++;
            if( row >= term_rows )
            {
                row=term_rows-1;
                int i;
                for(i=0;i<10;++i)
                {
                    usleep(500000);
                    gfx_scroll_down(8);
                    gfx_set_bg( i );
                }
                usleep(1000000);
                gfx_clear();
                return;
            }

        }
        ++ch;
        gfx_set_fg( ch );
    }
#endif

#if 0
    while(1)
    {
        gfx_set_bg( RR );
        gfx_clear();
        RR = (RR+1)%16;
        usleep(1000000);
    }
#endif

}


void video_line_test(int maxloops)
{
    int x=-10;
    int y=-10;
    int vx=1;
    int vy=0;

    gfx_set_fg( 15 );

    // what is current display size?
    unsigned int width=0, height=0;
    gfx_get_gfx_size( &width, &height );

    int count = maxloops;
    while(count >= 0)
    {
    	count --;

        // Render line
        gfx_line( width, height, x, y );

        usleep( 1000 );

        // Clear line
        gfx_swap_fg_bg();
        gfx_line( width, height, x, y );
        gfx_swap_fg_bg();

        x = x+vx;
        y = y+vy;

        if( x>700 )
        {
            x--;
            vx--;
            vy++;
        }
        if( y>500 )
        {
            y--;
            vx--;
            vy--;
        }
        if( x<-10 )
        {
            x++;
            vx++;
            vy--;
        }
        if( y<-10 )
        {
            y++;
            vx++;
            vy++;
        }

    }
}

#define WHOLE_FRAME_TIME		(1000000 / 60)
#define FRAME_WIDTH_PIXELS		384
#define FRAME_HEIGHT_PIXELS		264
#define FRAME_PIXELS			(FRAME_WIDTH_PIXELS * FRAME_HEIGHT_PIXELS)
void term_main_loop()
{
	unsigned int frameStartTime = time_microsec();
	int theFrame = 0;

	
#if 0
	// To prove the coordinates being drawn to are correct
	int x,y;
	for (y = 0; y < 128; y++)
	{
		for (x=0;x<192;x++)
		{
			gfx_draw_pixel(x , y , 10);
		}
	}
	for (y = 128; y < 256; y++)
	{
		for (x=192;x<384;x++)
		{
			gfx_draw_pixel(x , y , 20);
		}
	}
	CleanDataCache ();
	InvalidateDataCache ();
//	while(1)
//	{
//	}
#endif
	
    while(1)
    {
		unsigned int frameTime = time_microsec() - frameStartTime;
		// Try to catch-up any pending vsync pulses
		while (frameTime >= WHOLE_FRAME_TIME)
		{
			theFrame++;
			C64UserPort24Bit_setVSync(0);
			C64UserPort24Bit_setVSync(1);

#if 1
			// Add some debug values
			C64UserPort24Bit_addNext(frameStartTime + (WHOLE_FRAME_TIME / 64) , 0x04);
			C64UserPort24Bit_addNext(frameStartTime + (WHOLE_FRAME_TIME / 32) , 0x08);
			C64UserPort24Bit_addNext(frameStartTime + (WHOLE_FRAME_TIME / 16) , 0x18);
			C64UserPort24Bit_addNext(frameStartTime + (WHOLE_FRAME_TIME / 2) , 0x1f);
#endif

			// Before rendering, sync all data from GPIO IRQs
			CleanDataCache();
			InvalidateDataCache();
	
			// We have a frame worth of time, so render the data associated with it
			int calculatedPixelPos = 0;
			int currentColour = 1;
			int frameX = 0;
			int frameY = 0;
			int nextPixelPos = FRAME_PIXELS;
			unsigned int nextPixelColour = C64UserPort24Bit_getNext(frameStartTime , 0 , WHOLE_FRAME_TIME , FRAME_PIXELS, &nextPixelPos);

			for (frameY = 0 ; frameY < FRAME_HEIGHT_PIXELS ; frameY++)
			{
				// Pre-calc the current span start address based on frameY
				unsigned char *pixelAddr = gfx_get_pixel_addr(0,frameY);
				for (frameX = 0 ; frameX < FRAME_WIDTH_PIXELS ; frameX++)
				{
					if (calculatedPixelPos > nextPixelPos)
					{
						currentColour = nextPixelColour;
						// Need to reset the value before the next call to cope with an empty buffer
						nextPixelColour = C64UserPort24Bit_getNext(frameStartTime , 0 , WHOLE_FRAME_TIME , FRAME_PIXELS, &nextPixelPos);
//						ee_printf("frm%d : @%d,%d : 0x%02x  " , theFrame , frameX , frameY , currentColour);
					}

					*pixelAddr++ = (unsigned char) currentColour;
					*pixelAddr++ = (unsigned char) 0;

					calculatedPixelPos++;
				}
			}

			// Otherwise we see very strange cached data behaviour in the display
//			CleanDataCache();
//			DataSyncBarrier();
//			InvalidateDataCache();
//			gfx_switch_framebuffer();

//			ee_printf("sent vsync %d   secs %d\n" , theFrame++ , theFrame / 60);
			frameStartTime += WHOLE_FRAME_TIME;
			frameTime -= WHOLE_FRAME_TIME;

//			ee_printf("@%d : vsync  " , frameTime);
		}

        timer_poll();
    }

}

void entry_point(unsigned int r0, unsigned int r1, unsigned int *atags)
{
    unsigned int boardRevision;
    board_t raspiBoard;
    tSysRam ArmRam;

    //unused
    (void) r0;
    (void) r1;
    (void) atags;

	// clear BSS
	extern unsigned char __bss_start;
	extern unsigned char _end;
	for (unsigned char *pBSS = &__bss_start; pBSS < &_end; pBSS++)
	{
		*pBSS = 0;
	}

    // Heap init
    unsigned int memSize = ARM_MEMSIZE-MEM_HEAP_START;
    nmalloc_set_memory_area( (unsigned char*)MEM_HEAP_START, memSize);

    // UART buffer allocation
    uart_buffer = (volatile char*)nmalloc_malloc( UART_BUFFER_SIZE );
    uart_init(9600);

    // Init Pagetable
    CreatePageTable(ARM_MEMSIZE);
    EnableMMU();

    // Get informations about the board we are booting
    boardRevision = prop_revision();
    raspiBoard = board_info(boardRevision);
    prop_ARMRAM(&ArmRam);

    // Where is the Act LED?
    led_init(raspiBoard);

    // Timers and heartbeat
    timers_init();
    attach_timer_handler( HEARTBEAT_FREQUENCY, _heartbeat_timer_handler, 0, 0 );

//    initialize_framebuffer(640, 480, 8);
    initialize_framebuffer(FRAME_WIDTH_PIXELS, FRAME_HEIGHT_PIXELS, 16);

    gfx_term_putstring( "\x1B[2J" ); // Clear screen
    gfx_set_bg(BLUE);
    gfx_term_putstring( "\x1B[2K" ); // Render blue line at top

	ee_printf("\n");

    gfx_set_bg(BLACK);
    gfx_set_fg(GRAY);
    ee_printf("\nBooting on Raspberry Pi ");
    ee_printf(board_model(raspiBoard.model));
    ee_printf(", ");
    ee_printf(board_processor(raspiBoard.processor));
    ee_printf(", %iMB ARM RAM\n", ArmRam.size, ArmRam.baseAddr);

    // Set default config
    setDefaultConfig();

    gfx_set_bg(BLUE);
    gfx_set_fg(YELLOW);
    ee_printf("Initializing filesystem:\n");
    gfx_set_bg(BLACK);
    gfx_set_fg(GRAY);

    // Try to load a config file
    lookForConfigFile();

    uart_init(PiGfxConfig.uartBaudrate);
    initialize_uart_irq();

    gfx_set_bg(BLUE);
    gfx_set_fg(YELLOW);
    ee_printf("Initializing PS/2:\n");
    gfx_set_bg(BLACK);
    gfx_set_fg(GRAY);
//    if (initPS2() == 0)
//    {
//        ps2KeyboardFound = 1;
//    }
	C64UserPort24Bit_init();

    if (PiGfxConfig.showRC2014Logo)
    {
        gfx_set_drawing_mode(drawingTRANSPARENT);
        gfx_put_sprite( (unsigned char*)&G_STARTUP_LOGO, 0, 42 );
    }

    gfx_set_drawing_mode(drawingNORMAL);
    gfx_set_fg(GRAY);

    term_main_loop();
}
