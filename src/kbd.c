/*
Laser Logic
Copyright (C) 2022  Jeffry Johnston

This file is part of Laser Logic.

Laser Logic is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Laser Logic is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Laser Logic.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <gint/keyboard.h>
#include <gint/usb-ff-bulk.h>
#include "kbd.h"

#define KBD_OPTIONS (GETKEY_BACKLIGHT | GETKEY_MENU)
#define KEY_REDRAW -1
#define KEY_CAPTURE -2

extern int display_debug;
extern unsigned int light;
extern unsigned int dark;

void kbd_init(void)
{
	usb_interface_t const *interfaces[] = { &usb_ff_bulk, NULL };
	usb_open(interfaces, GINT_CALL_NULL);
}

static void power_off(void)
{
	asm volatile(
		"\n	mov	#1,r4"
		"\n	mov.l	poweroff_%=,r0"
		"\n	mov.l	syscall_%=,r2"
		"\n	jmp	@r2"
		"\n	nop"
		"\n	.align 4"
		"\npoweroff_%=:"
		"\n	.long	0x3F4"
		"\nsyscall_%=:"
		"\n	.long	0x80010070"
		:
	);
}

/*
How to take an in-game screenshot:
1. Run: fxlink -iw
2. Connect the calculator via USB then press EXIT (don't press F1 - F3)
3. Press "7" in the game
*/
unsigned int kbd_getkey(void)
{
	key_event_t event = getkey_opt(KBD_OPTIONS, NULL);
	uint key = event.key;
	switch (key) {
	case KEY_7:
		// Take screenshot
		if (usb_is_open())
			usb_fxlink_screenshot_gray(1);
		return KEY_CAPTURE;
	case KEY_ACON:
		// Power off
		gint_world_switch((gint_call_t) {
			.function = (void *)power_off,
			.args = {}
		});
		return KEY_REDRAW;
	case KEY_COMMA:
		display_debug = 1 - display_debug;
		return KEY_REDRAW;
	}
	if (display_debug)
		switch (key) {
		case KEY_8:
			light += 10;
			return KEY_REDRAW;
		case KEY_5:
			++light;
			return KEY_REDRAW;
		case KEY_2:
			--light;
			return KEY_REDRAW;
		case KEY_DOT:
			light -= 10;
			return KEY_REDRAW;
		case KEY_9:
			dark += 10;
			return KEY_REDRAW;
		case KEY_6:
			++dark;
			return KEY_REDRAW;
		case KEY_3:
			--dark;
			return KEY_REDRAW;
		case KEY_EXP:
			dark -= 10;
			return KEY_REDRAW;
		}
	return key;
}

command_t kbd_game(void)
{
	while (1) {
		switch (kbd_getkey()) {
		case KEY_REDRAW:
			return COMMAND_REDRAW;
		case KEY_UP:
			return COMMAND_CURSOR_UP;
		case KEY_DOWN:
			return COMMAND_CURSOR_DOWN;
		case KEY_LEFT:
			return COMMAND_CURSOR_LEFT;
		case KEY_RIGHT:
			return COMMAND_CURSOR_RIGHT;
		case KEY_SHIFT:
		case KEY_ALPHA:
		case KEY_EXE:
			return COMMAND_SELECT;
		case KEY_EXIT:
			return COMMAND_CANCEL;
		case KEY_F1:
		case KEY_HELP:
			return COMMAND_HELP;
		case KEY_F5:
			return COMMAND_ROTATE_CCW;
		case KEY_F6:
			return COMMAND_ROTATE_CW;
		case KEY_ADD:
		case KEY_RIGHTP:
			return COMMAND_PUZZLE_NEXT;
		case KEY_SUB:
		case KEY_LEFTP:
			return COMMAND_PUZZLE_PREV;
		}
	}
}

command_t kbd_help(void)
{
	while (1) {
		switch (kbd_getkey()) {
		case KEY_REDRAW:
			return COMMAND_REDRAW;
		case KEY_EXIT:
			return COMMAND_CANCEL;
		case KEY_F1:
		case KEY_HELP:
			return COMMAND_HELP;
		}
	}
}

void kbd_error(void)
{
	getkey_opt(GETKEY_BACKLIGHT, NULL);
}
