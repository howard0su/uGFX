/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS/GFX.

    ChibiOS/GFX is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/GFX is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    drivers/gdisp/ILI9320/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the SSD1289 display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* Include the emulation code for things we don't support */
#include "gdisp_emulation.c"

#include "ili9320_lld.c.h"

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/* ---- Required Routines ---- */
/*
	The following 2 routines are required.
	All other routines are optional.
*/

/**
 * @brief   Low level GDISP driver initialization.
 *
 * @notapi
 */
bool_t GDISP_LLD(init)(void) {
#if defined(GDISP_USE_FSMC)

	/* FSMC setup for F1/F3 */
	rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

   	/* set pins to FSMC mode */
   	IOBus busD = {GPIOD, PAL_WHOLE_PORT, 0};
   	IOBus busE = {GPIOE, PAL_WHOLE_PORT, 0};

	palSetBusMode(&busD, PAL_MODE_STM32_ALTERNATE_PUSHPULL );
	palSetBusMode(&busE, PAL_MODE_STM32_ALTERNATE_PUSHPULL );

	// const unsigned char FSMC_Bank = 0;
	const unsigned char FSMC_Bank = 6;

	/* FSMC timing */
	FSMC_Bank1->BTCR[FSMC_Bank+1] = (10) | (10 << 8) | (10 << 16);

	/* Bank1 NOR/SRAM control register configuration
	 * This is actually not needed as already set by default after reset */
	FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
#endif

	palSetPadMode(GPIOE, GPIOE_TFT_RST, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPad(GPIOE, GPIOE_TFT_RST);

	uint32_t DISPLAY_CODE = lld_lcdReadReg(0);

	lld_lcdWriteReg(0x0000, 0x0001); //start Int. osc
    lld_lcdDelay(500);
    lld_lcdWriteReg(0x0001, 0x0100); //Set SS bit (shift direction of outputs is from S720 to S1)
    lld_lcdWriteReg(0x0002, 0x0700); //select  the line inversion
	lld_lcdWriteReg(0x0003, 0x1038); //Entry mode(Horizontal : increment,Vertical : increment, AM=1)
    lld_lcdWriteReg(0x0004, 0x0000); //Resize control(No resizing)
    lld_lcdWriteReg(0x0008, 0x0202); //front and back porch 2 lines
    lld_lcdWriteReg(0x0009, 0x0000); //select normal scan
    lld_lcdWriteReg(0x000A, 0x0000); //display control 4
    lld_lcdWriteReg(0x000C, 0x0000); //system interface(2 transfer /pixel), internal sys clock,     
    lld_lcdWriteReg(0x000D, 0x0000); //Frame marker position
    lld_lcdWriteReg(0x000F, 0x0000); //selects clk, enable and sync signal polarity,
    lld_lcdWriteReg(0x0010, 0x0000); //  
    lld_lcdWriteReg(0x0011, 0x0000); //power control 2 reference voltages = 1:1,
    lld_lcdWriteReg(0x0012, 0x0000); //power control 3 VRH
    lld_lcdWriteReg(0x0013, 0x0000); //power control 4 VCOM amplitude
    lld_lcdDelay(500);
    lld_lcdWriteReg(0x0010, 0x17B0); //power control 1 BT,AP
    lld_lcdWriteReg(0x0011, 0x0137); //power control 2 DC,VC
    lld_lcdDelay(500);
    lld_lcdWriteReg(0x0012, 0x0139); //power control 3 VRH
    lld_lcdDelay(500);
    lld_lcdWriteReg(0x0013, 0x1d00); //power control 4 vcom amplitude
    lld_lcdWriteReg(0x0029, 0x0011); //power control 7 VCOMH
    lld_lcdDelay(500);
    lld_lcdWriteReg(0x0030, 0x0007);
    lld_lcdWriteReg(0x0031, 0x0403);
    lld_lcdWriteReg(0x0032, 0x0404);
    lld_lcdWriteReg(0x0035, 0x0002);
    lld_lcdWriteReg(0x0036, 0x0707);
    lld_lcdWriteReg(0x0037, 0x0606);
    lld_lcdWriteReg(0x0038, 0x0106);
    lld_lcdWriteReg(0x0039, 0x0007);
    lld_lcdWriteReg(0x003c, 0x0700);
    lld_lcdWriteReg(0x003d, 0x0707);
    lld_lcdWriteReg(0x0020, 0x0000); //starting Horizontal GRAM Address
    lld_lcdWriteReg(0x0021, 0x0000); //starting Vertical GRAM Address
    lld_lcdWriteReg(0x0050, 0x0000); //Horizontal GRAM Start Position
    lld_lcdWriteReg(0x0051, 0x00EF); //Horizontal GRAM end Position
    lld_lcdWriteReg(0x0052, 0x0000); //Vertical GRAM Start Position
    lld_lcdWriteReg(0x0053, 0x013F); //Vertical GRAM end Position

	switch (DISPLAY_CODE) {
		case 0x9320:
			lld_lcdWriteReg(0x0060, 0x2700); //starts scanning from G1, and 320 drive lines
			break;
		case 0x9325:
			lld_lcdWriteReg(0x0060, 0xA700); //starts scanning from G1, and 320 drive lines
			break;
	}

    lld_lcdWriteReg(0x0061, 0x0001); //fixed base display
    lld_lcdWriteReg(0x006a, 0x0000); //no scroll
    lld_lcdWriteReg(0x0090, 0x0010); //set Clocks/Line =16, Internal Operation Clock Frequency=fosc/1,
    lld_lcdWriteReg(0x0092, 0x0000); //set gate output non-overlap period=0
    lld_lcdWriteReg(0x0093, 0x0003); //set Source Output Position=3
    lld_lcdWriteReg(0x0095, 0x0110); //RGB interface(Clocks per line period=16 clocks)
    lld_lcdWriteReg(0x0097, 0x0110); //set Gate Non-overlap Period 0 locksc
    lld_lcdWriteReg(0x0098, 0x0110); //
    lld_lcdWriteReg(0x0007, 0x0173); //display On

    /* Initialise the GDISP structure */
	GDISP.Width = GDISP_SCREEN_WIDTH;
	GDISP.Height = GDISP_SCREEN_HEIGHT;
	GDISP.Orientation = GDISP_ROTATE_0;
	GDISP.Powermode = powerOn;
	GDISP.Backlight = 100;
	GDISP.Contrast = 50;
	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		GDISP.clipx0 = 0;
		GDISP.clipy0 = 0;
		GDISP.clipx1 = GDISP.Width;
		GDISP.clipy1 = GDISP.Height;
	#endif
	return TRUE;
}

/**
 * @brief   Draws a pixel on the display.
 *
 * @param[in] x        X location of the pixel
 * @param[in] y        Y location of the pixel
 * @param[in] color    The color of the pixel
 *
 * @notapi
 */
void GDISP_LLD(drawpixel)(coord_t x, coord_t y, color_t color) {
	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		if (x < GDISP.clipx0 || y < GDISP.clipy0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
	#endif
	lld_lcdSetCursor(x, y);
	lld_lcdWriteReg(0x0022, color);
}

/* ---- Optional Routines ---- */
/*
	All the below routines are optional.
	Defining them will increase speed but everything
	will work if they are not defined.
	If you are not using a routine - turn it off using
	the appropriate GDISP_HARDWARE_XXXX macro.
	Don't bother coding for obvious similar routines if
	there is no performance penalty as the emulation software
	makes a good job of using similar routines.
		eg. If gfillarea() is defined there is little
			point in defining clear() unless the
			performance bonus is significant.
	For good performance it is suggested to implement
		fillarea() and blitarea().
*/

#if GDISP_HARDWARE_CLEARS || defined(__DOXYGEN__)
	/**
	 * @brief   Clear the display.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] color    The color of the pixel
	 *
	 * @notapi
	 */
	void GDISP_LLD(clear)(color_t color) {
	    unsigned i;

	    lld_lcdSetCursor(0, 0);
	    lld_lcdWriteStreamStart();

	    for(i = 0; i < GDISP_SCREEN_WIDTH * GDISP_SCREEN_HEIGHT; i++)
	    	lld_lcdWriteData(color);

	    lld_lcdWriteStreamStop();
	}
#endif

#if GDISP_HARDWARE_FILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a color.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] color    The color of the fill
	 *
	 * @notapi
	 */
	void GDISP_LLD(fillarea)(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		unsigned i, area;

		area = cx*cy;
		lld_lcdSetViewPort(x, y, cx, cy);
		lld_lcdWriteStreamStart();
		for(i = 0; i < area; i++)
			lld_lcdWriteData(color);
		lld_lcdWriteStreamStop();
		lld_lcdResetViewPort();
	}
#endif

#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a bitmap.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] srcx, srcy   The bitmap position to start the fill from
	 * @param[in] srccx    The width of a line in the bitmap.
	 * @param[in] buffer   The pixels to use to fill the area.
	 *
	 * @notapi
	 */
	void GDISP_LLD(blitareaex)(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
		coord_t endx, endy;
		unsigned lg;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; srcx += GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; srcy += GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (srcx+cx > srccx)		cx = srccx - srcx;
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		lld_lcdSetViewPort(x, y, cx, cy);
		lld_lcdWriteStreamStart();

		endx = srcx + cx;
		endy = y + cy;
		lg = srccx - cx;
		buffer += srcx + srcy * srccx;
		for(; y < endy; y++, buffer += lg)
			for(x=srcx; x < endx; x++)
				lld_lcdWriteData(*buffer++);
		lld_lcdWriteStreamStop();
		lld_lcdResetViewPort();
	}
#endif

#if (GDISP_NEED_PIXELREAD && GDISP_HARDWARE_PIXELREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Get the color of a particular pixel.
	 * @note    Optional.
	 * @note    If x,y is off the screen, the result is undefined.
	 *
	 * @param[in] x, y     The start of the text
	 *
	 * @notapi
	 */
	color_t GDISP_LLD(getpixelcolor)(coord_t x, coord_t y) {
		color_t color;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < 0 || x >= GDISP.Width || y < 0 || y >= GDISP.Height) return 0;
		#endif

		lld_lcdSetCursor(x, y);
		lld_lcdWriteStreamStart();

		color = lld_lcdReadData();
		color = lld_lcdReadData();

		lld_lcdWriteStreamStop();

		return color;
	}
#endif

#if (GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL) || defined(__DOXYGEN__)
	/**
	 * @brief   Scroll vertically a section of the screen.
	 * @note    Optional.
	 * @note    If x,y + cx,cy is off the screen, the result is undefined.
	 * @note    If lines is >= cy, it is equivelent to a area fill with bgcolor.
	 *
	 * @param[in] x, y     The start of the area to be scrolled
	 * @param[in] cx, cy   The size of the area to be scrolled
	 * @param[in] lines    The number of lines to scroll (Can be positive or negative)
	 * @param[in] bgcolor  The color to fill the newly exposed area.
	 *
	 * @notapi
	 */
	void GDISP_LLD(verticalscroll)(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		static color_t buf[((GDISP_SCREEN_HEIGHT > GDISP_SCREEN_WIDTH ) ? GDISP_SCREEN_HEIGHT : GDISP_SCREEN_WIDTH)];
		coord_t row0, row1;
		unsigned i, gap, abslines;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (!lines || cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		abslines = lines < 0 ? -lines : lines;

		if (abslines >= cy) {
			abslines = cy;
			gap = 0;
		} else {
			gap = cy - abslines;
			for(i = 0; i < gap; i++) {
				if(lines > 0) {
					row0 = y + i + lines;
					row1 = y + i;
				} else {
					row0 = (y - i - 1) + lines;
					row1 = (y - i - 1);
				}

				/* read row0 into the buffer and then write at row1*/
				lld_lcdSetViewPort(x, row0, cx, 1);
				lld_lcdReadStreamStart();
				lld_lcdReadStream(buf, cx);
				lld_lcdReadStreamStop();

				lld_lcdSetViewPort(x, row1, cx, 1);
				lld_lcdWriteStreamStart();
				lld_lcdWriteStream(buf, cx);
				lld_lcdWriteStreamStop();
			}
		}

		/* fill the remaining gap */
		lld_lcdSetViewPort(x, lines > 0 ? (y+gap) : y, cx, abslines);
		lld_lcdWriteStreamStart();
		gap = cx*abslines;
		for(i = 0; i < gap; i++) lld_lcdWriteData(bgcolor);
		lld_lcdWriteStreamStop();
		lld_lcdResetViewPort();
	}
#endif

#if (GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL) || defined(__DOXYGEN__)
	/**
	 * @brief   Driver Control
	 * @details	Unsupported control codes are ignored.
	 * @note	The value parameter should always be typecast to (void *).
	 * @note	There are some predefined and some specific to the low level driver.
	 * @note	GDISP_CONTROL_POWER			- Takes a gdisp_powermode_t
	 * 			GDISP_CONTROL_ORIENTATION	- Takes a gdisp_orientation_t
	 * 			GDISP_CONTROL_BACKLIGHT -	 Takes an int from 0 to 100. For a driver
	 * 											that only supports off/on anything other
	 * 											than zero is on.
	 * 			GDISP_CONTROL_CONTRAST		- Takes an int from 0 to 100.
	 * 			GDISP_CONTROL_LLD			- Low level driver control constants start at
	 * 											this value.
	 *
	 * @param[in] what		What to do.
	 * @param[in] value		The value to use (always cast to a void *).
	 *
	 * @notapi
	 */
	void GDISP_LLD(control)(unsigned what, void *value) {
		switch(what) {
		case GDISP_CONTROL_POWER:
			if (GDISP.Powermode == (gdisp_powermode_t)value)
				return;
			switch((gdisp_powermode_t)value) {
			case powerOff:
				lld_lcdWriteReg(0x0010, 0x0000);	// leave sleep mode
				lld_lcdWriteReg(0x0007, 0x0000);	// halt operation
				lld_lcdWriteReg(0x0000, 0x0000);	// turn off oszillator
				lld_lcdWriteReg(0x0010, 0x0001);	// enter sleepmode
				break;
			case powerOn:
				lld_lcdWriteReg(0x0010, 0x0000);	// leave sleep mode
				if (GDISP.Powermode != powerSleep)
					GDISP_LLD(init)();
				break;
			case powerSleep:
				lld_lcdWriteReg(0x0010, 0x0001);	// enter sleep mode
				break;
			default:
				return;
			}
			GDISP.Powermode = (gdisp_powermode_t)value;
			return;
		case GDISP_CONTROL_ORIENTATION:
			if (GDISP.Orientation == (gdisp_orientation_t)value)
				return;
			switch((gdisp_orientation_t)value) {
			case GDISP_ROTATE_0:
				lld_lcdWriteReg(0x0001, 0x2B3F);
				/* ID = 11 AM = 0 */
				lld_lcdWriteReg(0x0011, 0x6070);
				GDISP.Height = GDISP_SCREEN_HEIGHT;
				GDISP.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_90:
				lld_lcdWriteReg(0x0001, 0x293F);
				/* ID = 11 AM = 1 */
				lld_lcdWriteReg(0x0011, 0x6078);
				GDISP.Height = GDISP_SCREEN_WIDTH;
				GDISP.Width = GDISP_SCREEN_HEIGHT;
				break;
			case GDISP_ROTATE_180:
				lld_lcdWriteReg(0x0001, 0x2B3F);
				/* ID = 01 AM = 0 */
				lld_lcdWriteReg(0x0011, 0x6040);
				GDISP.Height = GDISP_SCREEN_HEIGHT;
				GDISP.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_270:
				lld_lcdWriteReg(0x0001, 0x293F);
				/* ID = 01 AM = 1 */
				lld_lcdWriteReg(0x0011, 0x6048);
				GDISP.Height = GDISP_SCREEN_WIDTH;
				GDISP.Width = GDISP_SCREEN_HEIGHT;
				break;
			default:
				return;
			}
			#if GDISP_NEED_CLIP || GDISP_NEED_VALIDATION
				GDISP.clipx0 = 0;
				GDISP.clipy0 = 0;
				GDISP.clipx1 = GDISP.Width;
				GDISP.clipy1 = GDISP.Height;
			#endif
			GDISP.Orientation = (gdisp_orientation_t)value;
			return;
/*
		case GDISP_CONTROL_BACKLIGHT:
		case GDISP_CONTROL_CONTRAST:
*/
		}
	}

#endif

#endif /* GFX_USE_GDISP */
/** @} */