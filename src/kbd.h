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

#pragma once

typedef enum __attribute__((__packed__)) {
	COMMAND_REDRAW,
	COMMAND_OSMENU,
	COMMAND_CURSOR_UP,
	COMMAND_CURSOR_DOWN,
	COMMAND_CURSOR_LEFT,
	COMMAND_CURSOR_RIGHT,
	COMMAND_SELECT,
	COMMAND_CANCEL,
	COMMAND_ROTATE_CCW,
	COMMAND_ROTATE_CW,
	COMMAND_PUZZLE_NEXT,
	COMMAND_PUZZLE_PREV,
	COMMAND_HELP
} command_t;

void kbd_init(void);
unsigned int kbd_getkey(void);
command_t kbd_game(void);
command_t kbd_help(void);
void kbd_error(void);
