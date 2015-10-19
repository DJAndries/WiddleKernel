#define FB_GREEN     2
#define FB_DARK_GREY 8
#include "io.h"

#define SERIAL_COM1_BASE                0x3F8      /* COM1 base port */

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

/* The I/O port commands */

/* SERIAL_LINE_ENABLE_DLAB:
* Tells the serial port to expect first the highest 8 bits on the data port,
* then the lowest 8 bits will follow
*/
#define SERIAL_LINE_ENABLE_DLAB         0x80

/* The I/O ports */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

unsigned short currPos = 0;
/** fb_move_cursor:
*  Moves the cursor of the framebuffer to the given position
*
*  @param pos The new position of the cursor
*/
void fb_move_cursor(unsigned short pos)
{
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
	outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
	outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
	outb(FB_DATA_PORT,    pos & 0x00FF);
}
char *fb = (char *) 0x000B8000;
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);
/** fb_write_cell:
     *  Writes a character with the given foreground and background to position i
     *  in the framebuffer.
     *
     *  @param i  The location in the framebuffer
     *  @param c  The character
     *  @param fg The foreground color
     *  @param bg The background color
     */

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
	fb[i] = c;
	fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

int write(char *buf) {
	while(*buf != 0) {
		fb_move_cursor(currPos);
		fb_write_cell(2 * currPos++, *buf, FB_GREEN, FB_DARK_GREY);
		buf++;
	}
	return currPos;
}

void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
{
	outb(SERIAL_LINE_COMMAND_PORT(com),
	     SERIAL_LINE_ENABLE_DLAB);
	outb(SERIAL_DATA_PORT(com),
	     (divisor >> 8) & 0x00FF);
	outb(SERIAL_DATA_PORT(com),
	     divisor & 0x00FF);
}
void serial_configure_line(unsigned short com)
{
/* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
 * Content: | d | b | prty  | s | dl  |
 * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
 */
	outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}
void kmain() {
	char bing[34] = "wowowow";
	write(bing);
}
