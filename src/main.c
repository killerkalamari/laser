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

#include <gint/gint.h>
#include <gint/hardware.h>
#include "display.h"
#include "file.h"
#include "game.h"
#include "kbd.h"

static int help2(void)
{
	while (1) {
		display_help2();
		switch(kbd_help()) {
		case COMMAND_CANCEL:
			return 1;
		case COMMAND_HELP:
			return 0;
		default:
			break;
		}
	}
}

static void help1(void)
{
	while (1) {
		display_help1();
		switch(kbd_help()) {
		case COMMAND_CANCEL:
			return;
		case COMMAND_HELP:
			if (help2())
				return;
			break;
		default:
			break;
		}
	}
}

static void play_game(void)
{
	// Read puzzles
	int rc = file_read_puzzles(game_get_puzzles());
	if (rc) {
		display_file_error(rc, "reading", PUZZLE_FILENAME);
		kbd_error();
		return;
	}

	// Read solved status
	rc = file_read_solved(game_get_solved());

	rc = game_init(rc);
	if (rc) {
		display_file_error(rc, "reading", SOLVED_FILENAME);
		kbd_error();
		return;
	}

	display_init_mono(true);
	while (1) {
		if (game_laser()) {
			int rc = file_write_solved(game_get_solved());
			if (rc) {
				display_file_error(rc, "writing",
							SOLVED_FILENAME);
				kbd_error();
				return;
			}
			display_init_mono(true);;
		}

		display_game();

		switch(kbd_game()) {
		case COMMAND_OSMENU:
			if (gint[HWCALC] == HWCALC_FXCG100)
				return;
			break;
		case COMMAND_CURSOR_UP:
			game_cursor_row(-1);
			break;
		case COMMAND_CURSOR_DOWN:
			game_cursor_row(1);
			break;
		case COMMAND_CURSOR_LEFT:
			game_cursor_col(-1);
			break;
		case COMMAND_CURSOR_RIGHT:
			game_cursor_col(1);
			break;
		case COMMAND_SELECT:
			game_select_token();
			break;
		case COMMAND_CANCEL:
			game_deselect_token();
			break;
		case COMMAND_ROTATE_CCW:
			game_rotate_token(-1);
			break;
		case COMMAND_ROTATE_CW:
			game_rotate_token(1);
			break;
		case COMMAND_PUZZLE_NEXT:
			game_next_puzzle();
			break;
		case COMMAND_PUZZLE_PREV:
			game_previous_puzzle();
			break;
		case COMMAND_HELP:
			help1();
			break;
		default:
			break;
		}
	}
}

int main(void)
{
	kbd_init();
	display_init();
	play_game();
	return 0;
}
