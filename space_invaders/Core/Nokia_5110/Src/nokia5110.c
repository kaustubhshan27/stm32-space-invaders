/* This example accompanies the book
 "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
 ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
 You may use, edit, run or distribute this file
 as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110)
// Reset         (RST, pin 1) connected to PA9
// SPI1_NSS      (CE,  pin 2) connected to PA4
// Data/Command  (DC,  pin 3) connected to PA8
// SPI1_MOSI     (Din, pin 4) connected to PA7
// SPI1_SCK      (Clk, pin 5) connected to PA5
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

#include <string.h>
#include "stm32f4xx_hal.h"
#include "nokia5110.h"

// This table contains the hex values that represent pixels
// for a font that is 5 pixels wide and 8 pixels high
static uint8_t ASCII[][5] = {
   {0x00, 0x00, 0x00, 0x00, 0x00} // 20
  ,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
  ,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
  ,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
  ,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
  ,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
  ,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
  ,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
  ,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
  ,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
  ,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
  ,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
  ,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
  ,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
  ,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
  ,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
  ,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
  ,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
  ,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
  ,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
  ,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
  ,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
  ,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
  ,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
  ,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
  ,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
  ,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
  ,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
  ,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
  ,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
  ,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
  ,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
  ,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
  ,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
  ,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
  ,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
  ,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
  ,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
  ,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
  ,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
  ,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
  ,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
  ,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
  ,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
  ,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
  ,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
  ,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
  ,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
  ,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
  ,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
  ,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
  ,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
  ,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
  ,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
  ,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
  ,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
  ,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
  ,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
  ,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
  ,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
  ,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c '\'
  ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
  ,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
  ,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
  ,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
  ,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
  ,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
  ,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
  ,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
  ,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
  ,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
  ,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
  ,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
  ,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
  ,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
  ,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
  ,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
  ,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
  ,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
  ,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
  ,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
  ,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
  ,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
  ,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
  ,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
  ,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
  ,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
  ,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
  ,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
  ,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
  ,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
  ,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
  ,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
  ,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
  ,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ~
  ,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f DEL
  ,{0x1f, 0x24, 0x7c, 0x24, 0x1f} // 7f UT sign
};

uint8_t screen[SCREENW * SCREENH / 8];	// buffer stores the next image to be printed on the screen

enum typeOfWrite {
	COMMAND,                              // the transmission is an LCD command
	DATA                                  // the transmission is data
};

extern SPI_HandleTypeDef hspi1;		// SPI handle for communication with Nokia 5110
extern uint8_t txCmpltFlag;			// flag to indicate completion of SPI communication

// The Data/Command pin must be valid when the eighth bit is
// sent.  The SSI module has hardware input and output FIFOs
// that are 8 locations deep.  Based on the observation that
// the LCD interface tends to send a few commands and then a
// lot of data, the FIFOs are not used when writing
// commands, and they are used when writing data.  This
// ensures that the Data/Command pin status matches the byte
// that is actually being transmitted.
// The write command operation waits until all data has been
// sent, configures the Data/Command pin for commands, sends
// the command, and then waits for the transmission to
// finish.
// The write data operation waits until there is room in the
// transmit FIFO, configures the Data/Command pin for data,
// and then adds the data to the transmit FIFO.

// This is a helper function that sends an 8-bit message to the LCD.
// inputs: type     COMMAND or DATA
//         message  8-bit code to transmit
// outputs: none
void static lcdwrite(enum typeOfWrite type, uint8_t *message, uint16_t message_size) {
	while(!txCmpltFlag);		// wait for the previous data tx to finish
	txCmpltFlag = 0;

	if (type == COMMAND) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	} else if (type == DATA) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	}

	HAL_SPI_Transmit_DMA(&hspi1, message, message_size);
}

//********Nokia5110_Init*****************
// Initialize Nokia 5110 48x84 LCD by sending the proper
// commands to the PCD8544 driver.  One new feature of the
// LM4F120 is that its SSIs can get their baud clock from
// either the system clock or from the 16 MHz precision
// internal oscillator.
// inputs: none
// outputs: none
void Nokia5110_Init(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);	// RST = LOW, reset the LCD to a known state
	for (uint8_t delay = 0; delay < 100; delay = delay + 1)
		;                    								// delay minimum 100 ms
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);    // RST = HIGH, negative logic

	uint8_t cmd[1];

	cmd[0] = 0x21;
	lcdwrite(COMMAND, cmd, 1); 		// chip active; horizontal addressing mode (V = 0); use extended instruction set (H = 1)

	cmd[0] = CONTRAST;
	lcdwrite(COMMAND, cmd, 1);		// 0xA0 (for 3.3V blue SparkFun)

	cmd[0] = 0x04;
	lcdwrite(COMMAND, cmd, 1);		// set temp coefficient

	cmd[0] = 0x14;
	lcdwrite(COMMAND, cmd, 1);		// LCD bias mode 1:48: try 0x13 or 0x14

	cmd[0] = 0x20;
	lcdwrite(COMMAND, cmd, 1);		// we must send 0x20 before modifying the display control mode

	cmd[0] = 0x0C;
	lcdwrite(COMMAND, cmd, 1);		// set display control to normal mode: 0x0D for inverse
}

//********Nokia5110_Outchar*****************
// Print a uint8_tacter to the Nokia 5110 48x84 LCD.  The
// uint8_tacter will be printed at the current cursor position,
// the cursor will automatically be updated, and it will
// wrap to the next row or back to the top if necessary.
// One blank column of pixels will be printed on either side
// of the uint8_tacter for readability.  Since uint8_tacters are 8
// pixels tall and 5 pixels wide, 12 uint8_tacters fit per row,
// and there are six rows.
// inputs: data  uint8_tacter to print
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_Outchar(uint8_t character) {
	uint8_t data[1];
	data[0] = 0x00;
	lcdwrite(DATA, data, 1);                 // blank vertical line padding
	for (uint8_t i = 0; i < 5; i++) {
		lcdwrite(DATA, &ASCII[character - 0x20][i], 1);
	}
	data[0] = 0x00;
	lcdwrite(DATA, data, 1);                 // blank vertical line padding
}

//********Nokia5110_OutString*****************
// Print a string of uint8_tacters to the Nokia 5110 48x84 LCD.
// The string will automatically wrap, so padding spaces may
// be needed to make the output look optimal.
// inputs: ptr  pointer to NULL-terminated ASCII string
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutString(char *ptr) {
	while (*ptr) {
		Nokia5110_Outchar((uint8_t) *ptr);
		ptr++;
	}
}

//********Nokia5110_OutUDec*****************
// Output a 16-bit number in unsigned decimal format with a
// fixed size of five right-justified digits of output.
// Inputs: n  16-bit unsigned number
// Outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutUDec(uint16_t n) {
	if (n < 10) {
		Nokia5110_OutString("    ");
		Nokia5110_Outchar(n + '0'); 		// n is between 0 and 9
	} else if (n < 100) {
		Nokia5110_OutString("   ");
		Nokia5110_Outchar(n / 10 + '0'); 	// tens digit
		Nokia5110_Outchar(n % 10 + '0'); 	// ones digit
	} else if (n < 1000) {
		Nokia5110_OutString("  ");
		Nokia5110_Outchar(n / 100 + '0'); 	// hundreds digit
		n = n % 100;
		Nokia5110_Outchar(n / 10 + '0'); 	// tens digit
		Nokia5110_Outchar(n % 10 + '0'); 	// ones digit
	} else if (n < 10000) {
		Nokia5110_Outchar(' ');
		Nokia5110_Outchar(n / 1000 + '0'); 	// thousands digit
		n = n % 1000;
		Nokia5110_Outchar(n / 100 + '0'); 	// hundreds digit
		n = n % 100;
		Nokia5110_Outchar(n / 10 + '0'); 	// tens digit
		Nokia5110_Outchar(n % 10 + '0'); 	// ones digit
	} else {
		Nokia5110_Outchar(n / 10000 + '0'); // ten-thousands digit
		n = n % 10000;
		Nokia5110_Outchar(n / 1000 + '0'); 	// thousands digit
		n = n % 1000;
		Nokia5110_Outchar(n / 100 + '0'); 	// hundreds digit
		n = n % 100;
		Nokia5110_Outchar(n / 10 + '0'); 	// tens digit
		Nokia5110_Outchar(n % 10 + '0'); 	// ones digit
	}
}

//********Nokia5110_SetCursor*****************
// Move the cursor to the desired X- and Y-position.  The
// next uint8_tacter will be printed here.  X=0 is the leftmost
// column.  Y=0 is the top row.
// inputs: newX  new X-position of the cursor (0<=newX<=11)
//         newY  new Y-position of the cursor (0<=newY<=5)
// outputs: none
void Nokia5110_SetCursor(uint8_t newX, uint8_t newY) {
	if ((newX > 11) || (newY > 5)) {        // bad input
		return;                             // do nothing
	}

	uint8_t cmd[1];
	cmd[0] = 0x80 | (newX * 7);		// multiply newX by 7 because each uint8_tacter is 7 columns wide
	lcdwrite(COMMAND, cmd, 1);    	// setting bit 7 updates X-position
	cmd[0] = 0x40 | newY;
	lcdwrite(COMMAND, cmd, 1);      // setting bit 6 updates Y-position
}

//********Nokia5110_Clear*****************
// Clear the LCD by writing zeros to the entire screen and
// reset the cursor to (0,0) (top left corner of screen).
// inputs: none
// outputs: none
void Nokia5110_Clear(void) {
	memset(screen, 0, SCREENW * SCREENH / 8);
	lcdwrite(DATA, screen, SCREENW * SCREENH / 8);
	Nokia5110_SetCursor(0, 0);
}

//********Nokia5110_DrawFullImage*****************
// Fill the whole screen by drawing a 48x84 bitmap image.
// inputs: ptr  pointer to 504 byte bitmap
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_DrawFullImage(uint8_t *ptr) {
	Nokia5110_SetCursor(0, 0);
	lcdwrite(DATA, ptr, SCREENW * SCREENH / 8);
}

//********Nokia5110_PrintBMP*****************
// Bitmaps defined above were created for the LM3S1968 or
// LM3S8962's 4-bit grayscale OLED display.  They also
// still contain their header data and may contain padding
// to preserve 4-byte alignment.  This function takes a
// bitmap in the previously described format and puts its
// image data in the proper location in the buffer so the
// image will appear on the screen after the next call to
//   Nokia5110_DisplayBuffer();
// The interface and operation of this process is modeled
// after RIT128x96x4_BMP(x, y, image);
// inputs: xpos      horizontal position of bottom left corner of image, columns from the left edge
//                     must be less than 84
//                     0 is on the left; 82 is near the right
//         ypos      vertical position of bottom left corner of image, rows from the top edge
//                     must be less than 48
//                     2 is near the top; 47 is at the bottom
//         ptr       pointer to a 16 color BMP image
//         threshold grayscale colors above this number make corresponding pixel 'on'
//                     0 to 14
//                     0 is fine for ships, explosions, projectiles, and bunkers
// outputs: none
void Nokia5110_PrintBMP(uint8_t xpos, uint8_t ypos, const uint8_t *ptr, uint8_t threshold) {
	uint16_t width = ptr[18], height = ptr[22], j;
	uint16_t screenx, screeny;
	uint8_t mask;

	// check for clipping
	if ((height <= 0) || 					// bitmap is unexpectedly encoded in top-to-bottom pixel order
			((width % 2) != 0) ||           // must be even number of columns
			((xpos + width) > SCREENW) || 	// right side cut off
			(ypos < (height - 1)) ||      	// top cut off
			(ypos > SCREENH)) { 			// bottom cut off
		return;
	}

	if (threshold > 14) {
		threshold = 14;             // only full 'on' turns pixel on
	}

	// bitmaps are encoded backwards, so start at the bottom left corner of the image
	screeny = ypos / 8;
	screenx = xpos + SCREENW * screeny;
	mask = ypos % 8;                		// row 0 to 7
	mask = 0x01 << mask;            		// now stores a mask 0x01 to 0x80
	j = ptr[10];    						// byte 10 contains the offset where image data can be found

	for (uint16_t i = 1; i <= (width * height / 2); i++) {
		// the left pixel is in the upper 4 bits
		if (((ptr[j] >> 4) & 0xF) > threshold) {
			screen[screenx] |= mask;
		} else {
			screen[screenx] &= ~mask;
		}
		screenx = screenx + 1;

		// the right pixel is in the lower 4 bits
		if ((ptr[j] & 0xF) > threshold) {
			screen[screenx] |= mask;
		} else {
			screen[screenx] &= ~mask;
		}

		screenx = screenx + 1;
		j = j + 1;
		if ((i % (width / 2)) == 0) {     // at the end of a row
			if (mask > 0x01) {
				mask = mask >> 1;
			} else {
				mask = 0x80;
				screeny = screeny - 1;
			}
			screenx = xpos + SCREENW * screeny;

			// bitmaps are 32-bit word aligned
			switch ((width / 2) % 4) {      // skip any padding
			case 0:
				j = j + 0;
				break;
			case 1:
				j = j + 3;
				break;
			case 2:
				j = j + 2;
				break;
			case 3:
				j = j + 1;
				break;
			}
		}
	}
}

//********Nokia5110_ClearBuffer*****************
// There is a buffer in RAM that holds one screen
// This routine clears this buffer
void Nokia5110_ClearBuffer(void) {
	for (uint16_t i = 0; i < SCREENW * SCREENH / 8; i++) {
		screen[i] = 0;              // clear buffer
	}
}

//********Nokia5110_SetPixel*****************
// Turn on the specified pixel in the screen buffer.
// inputs: x - horizontal coordinate of the pixel, must be less than 84
//         y - vertical coordinate of the pixel, must be less than 48
//         Nokia5110_SetPixel(0, 0); turns on the upper left pixel
// outputs: none
// Pixel level functions by Tomas
void Nokia5110_SetPixel(uint8_t x, uint8_t y) {
	uint16_t PixelByte;            // byte# in screen buffer
	uint8_t PixelBit;              // bit# in byte
	if ((x < 84) && (y < 48)) {    // check screen boundaries
		PixelByte = ((y / 8) * 84) + x;
		PixelBit = y % 8;
		screen[PixelByte] |= 1U << PixelBit;
	}
}

//********Nokia5110_ClearPixel*****************
// Turn off the specified pixel in the screen buffer.
// inputs: x - horizontal coordinate of the pixel, must be less than 84
//         y - vertical coordinate of the pixel, must be less than 48
//         Nokia5110_ClearPixel(0, 0); turns off the upper left pixel
// outputs: none
// Pixel level functions by Tomas
void Nokia5110_ClearPixel(uint8_t x, uint8_t y) {
	uint16_t PixelByte;            // byte# in screen buffer
	uint8_t PixelBit;              // bit# in byte
	if ((x < 84) && (y < 48)) {    // check screen boundaries
		PixelByte = ((y / 8) * 84) + x;
		PixelBit = y % 8;
		screen[PixelByte] &= ~(1U << PixelBit);
	}
}

//********Nokia5110_DisplayBuffer*****************
// Fill the whole screen by drawing a 48x84 screen image.
// inputs: none
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_DisplayBuffer(void) {
	Nokia5110_DrawFullImage(screen);
}
