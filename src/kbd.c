/*
Laser Logic
Copyright (C) 2022, 2025  Jeffry Johnston

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

#include <gint/display.h>
#include <gint/gint.h>
#include <gint/hardware.h>
#include <gint/keyboard.h>
#ifndef FX9860G_G3A
#include <gint/usb-ff-bulk.h>
#include <gint/drivers/t6k11.h>
#endif
#include "kbd.h"
#include "display.h"

#define KEY_REDRAW -1
#define KEY_NONE -2

extern uint8_t debug_display;
#ifdef FX9860G_G3A
extern uint8_t debug_r;
extern uint8_t debug_g;
extern uint8_t debug_b;
#else
extern uint16_t debug_light;
extern uint16_t debug_dark;
#endif

bool ignore_keypress;

void kbd_init(void)
{
#ifndef FX9860G_G3A
	usb_interface_t const *interfaces[] = { &usb_ff_bulk, NULL };
	usb_open(interfaces, GINT_CALL_NULL);
	ignore_keypress = false;
#endif
}

#ifndef FX9860G_G3A
/*
How to take an in-game screenshot:
1. Run: fxlink -iw
2. Connect the calculator via USB then press EXIT (don't press F1 - F3)
3. Press "7" in the game
*/
static void take_screenshot(void)
{
	if (usb_is_open()) {
		if (display_is_using_gray_engine())
			usb_fxlink_screenshot_gray(1);
		else
			usb_fxlink_screenshot(1);
	}
}
#endif

static unsigned int kbd_getkey(void)
{
	key_event_t event = getkey_opt(GETKEY_BACKLIGHT, NULL);
	if (ignore_keypress) {
		ignore_keypress = false;
		return KEY_NONE;
	}

	uint key = event.key;
	switch (key) {
#ifndef FX9860G_G3A
	case KEY_7:
		take_screenshot();
		break;
#endif
	case KEY_ACON:
		gint_poweroff(true);
#ifdef FX9860G_G3A
		dupdate();
#endif
		break;
	case KEY_COMMA:
		debug_display = 1 - debug_display;
		return KEY_REDRAW;
#ifndef FX9860G_G3A
	case KEY_OPTN:
		if (!isSlim())
			t6k11_backlight(-1);
		break;
#endif
	}
	if (debug_display)
		switch (key) {
#ifdef FX9860G_G3A
		case KEY_1:
			if (debug_r < 31)
				++debug_r;
			return KEY_REDRAW;
		case KEY_2:
			if (debug_g < 63)
				++debug_g;
			return KEY_REDRAW;
		case KEY_3:
			if (debug_b < 31)
				++debug_b;
			return KEY_REDRAW;
		case KEY_0:
			if (debug_r > 0)
				--debug_r;
			return KEY_REDRAW;
		case KEY_DOT:
			if (debug_g > 0)
				--debug_g;
			return KEY_REDRAW;
		case KEY_EXP:
			if (debug_b > 0)
				--debug_b;
			return KEY_REDRAW;
#else
		case KEY_8:
			debug_light += 10;
			return KEY_REDRAW;
		case KEY_5:
			++debug_light;
			return KEY_REDRAW;
		case KEY_2:
			--debug_light;
			return KEY_REDRAW;
		case KEY_DOT:
			debug_light -= 10;
			return KEY_REDRAW;
		case KEY_9:
			debug_dark += 10;
			return KEY_REDRAW;
		case KEY_6:
			++debug_dark;
			return KEY_REDRAW;
		case KEY_3:
			--debug_dark;
			return KEY_REDRAW;
		case KEY_EXP:
			debug_dark -= 10;
			return KEY_REDRAW;
#endif
		}
	return key;
}

command_t kbd_game(void)
{
	while (1) {
		switch (kbd_getkey()) {
		case KEY_REDRAW:
			return COMMAND_REDRAW;
		case KEY_MENU:
#ifndef FX9860G_G3A
			display_menu_return();
			ignore_keypress = true;
#endif
			gint_osmenu();
#ifdef FX9860G_G3A
			dupdate();
#endif
			return COMMAND_OSMENU;
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
			if (gint[HWCALC] != HWCALC_FXCG100)
				return COMMAND_HELP;
			break;
		case KEY_SETTINGS:
			if (gint[HWCALC] == HWCALC_FXCG100)
				return COMMAND_HELP;
			break;
		case KEY_F3:
			if (gint[HWCALC] == HWCALC_FXCG100)
				return COMMAND_ROTATE_CCW;
			break;
		case KEY_F5:
			if (gint[HWCALC] != HWCALC_FXCG100)
				return COMMAND_ROTATE_CCW;
			else
				return COMMAND_ROTATE_CW;
			break;
		case KEY_F6:
			if (gint[HWCALC] != HWCALC_FXCG100)
				return COMMAND_ROTATE_CW;
			break;
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
			if (gint[HWCALC] != HWCALC_FXCG100)
				return COMMAND_HELP;
			break;
		case KEY_SETTINGS:
			if (gint[HWCALC] == HWCALC_FXCG100)
				return COMMAND_HELP;
			break;
		}
	}
}

void kbd_error(void)
{
	kbd_getkey();
}
