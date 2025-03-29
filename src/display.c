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
#include <gint/gray.h>
#include <gint/hardware.h>
#include "game.h"

#define CORNER_NE 0
#define CORNER_SE 1
#define CORNER_SW 2
#define CORNER_NW 3

extern bopti_image_t img_background;
extern bopti_image_t img_background_cg100;
extern bopti_image_t img_can_move;
extern bopti_image_t img_can_rotate;
extern bopti_image_t img_cursor;
extern bopti_image_t img_hit_n;
extern bopti_image_t img_hit_e;
extern bopti_image_t img_hit_s;
extern bopti_image_t img_hit_w;
extern bopti_image_t img_logo;
extern bopti_image_t img_selection;
extern bopti_image_t img_solved;
extern bopti_image_t img_target_hit;
extern bopti_image_t img_target_missed;
extern bopti_image_t img_token_block;
extern bopti_image_t img_token_checkpoint_ns;
extern bopti_image_t img_token_checkpoint_ew;
extern bopti_image_t img_token_laser_n;
extern bopti_image_t img_token_laser_e;
extern bopti_image_t img_token_laser_s;
extern bopti_image_t img_token_laser_w;
extern bopti_image_t img_token_mirror_nesw;
extern bopti_image_t img_token_mirror_nwse;
extern bopti_image_t img_token_splitter_nesw;
extern bopti_image_t img_token_splitter_nwse;
extern bopti_image_t img_token_target_n;
extern bopti_image_t img_token_target_e;
extern bopti_image_t img_token_target_s;
extern bopti_image_t img_token_target_w;
extern bopti_image_t img_token_target_req_n;
extern bopti_image_t img_token_target_req_e;
extern bopti_image_t img_token_target_req_s;
extern bopti_image_t img_token_target_req_w;
extern bopti_image_t img_help1;
extern bopti_image_t img_help1_cg100;
extern bopti_image_t img_help2;
extern bopti_image_t img_help2_cg100;
extern font_t font_laser;

bool display_is_gray;
uint8_t debug_display = 0;
#ifdef FX9860G_G3A
uint8_t debug_r;
uint8_t debug_g;
uint8_t debug_b;
#else
uint16_t debug_light;
uint16_t debug_dark;
#endif

#ifdef FX9860G_G3A
static uint16_t rgb565(void)
{
	return ((debug_r << 11) | (debug_g << 5) | debug_b);
}
#endif

void display_init(void)
{
#ifdef FX9860G_G3A
	debug_r = 28;
	debug_g = 0;
	debug_b = 5;
	dgray_setcolors(DGRAY_BLACK_DEFAULT, DGRAY_DARK_DEFAULT, rgb565(), DGRAY_WHITE_DEFAULT);
#else
	if (gint[HWCALC] == HWCALC_G35PE2) {
		debug_light = 1006;
		debug_dark = 834;
	} else {
		debug_light = 1742;
		debug_dark = 927;
	}
#endif
}

void display_init_mono(const bool clear)
{
	dgray(DGRAY_OFF);
	dfont(&font_laser);
	if (clear)
		dclear(C_WHITE);
	display_is_gray = false;
}

static void display_init_gray(void)
{
#ifdef FX9860G_G3A
	dgray_setcolors(DGRAY_BLACK_DEFAULT, DGRAY_DARK_DEFAULT, rgb565(), DGRAY_WHITE_DEFAULT);
#else
	if (gint[HWCALC] == HWCALC_G35PE2)
		dgray_setdelays(debug_light, debug_light);
	else
		dgray_setdelays(debug_light, debug_dark);
#endif
	dfont(&font_laser);
	dgray(DGRAY_ON);
	display_is_gray = true;
}

void display_menu_return(void)
{
	display_init_mono(true);
	dimage(24, 13, &img_logo);
	dprint(20, 45, C_BLACK, "PRESS ANY KEY TO CONTINUE");
}

static void debug(void)
{
	if (!debug_display)
		return;

#ifdef FX9860G_G3A
	drect(95, 37, 127, 63, C_DARK);
	dtext(96, 38, C_WHITE, "RGB565");
	dtext(96, 52, C_WHITE, "R");
	dtext(107, 52, C_WHITE, "G");
	dtext(118, 52, C_WHITE, "B");
	dprint(96, 44, C_WHITE, "%04X", rgb565());
	dprint(96, 58, C_WHITE, "%u", debug_r);
	dprint(107, 58, C_WHITE, "%u", debug_g);
	dprint(118, 58, C_WHITE, "%u", debug_b);
#else
	drect(0, 18, 31, 24, C_WHITE);
	drect(95, 18, 127, 24, C_WHITE);
	dprint(1, 19, C_BLACK, "%u", debug_light);
	dprint(96, 19, C_BLACK, "%u", debug_light);
	drect(0, 25, 31, 46, C_LIGHT);
	drect(95, 25, 127, 46, C_DARK);
#endif
}

static void draw_loc(loc_t loc, int x, int y)
{
	switch (loc) {
	case LOC_NORTH:
		dline(x + 6, y, x + 6, y + 5, C_LIGHT);
		break;
	case LOC_EAST:
		dline(x + 12, y + 6, x + 7, y + 6, C_LIGHT);
		break;
	case LOC_SOUTH:
		dline(x + 6, y + 12, x + 6, y + 7, C_LIGHT);
		break;
	case LOC_WEST:
		dline(x, y + 6, x + 5, y + 6, C_LIGHT);
		break;
	case LOC_STOP:
		break;
	}
}

static void draw_simple(loc_t entry, loc_t exit, int x, int y)
{
	draw_loc(entry, x, y);
	draw_loc(exit, x, y);
	if (!((entry + exit) & 0x01))
		dpixel(x + 6, y + 6, C_LIGHT);
}

static void draw_laser(void)
{
	int count = game_get_path_count();
	for (int i = 0; i < count; ++i) {
		path_t *path = game_get_path(i);
		int row = path->row;
		int col = path->col;
		int y = 12 * row + 1;
		int x = 12 * col + 33;

		token_t *token = game_get_token(row, col);
		switch (token->type) {
		case TOKEN_NONE:
		case TOKEN_BLOCK:
		case TOKEN_MIRROR:
		case TOKEN_SPLITTER:
			draw_simple(path->entry, path->exit, x, y);
			break;
		case TOKEN_CHECKPOINT:
			if (path->exit != LOC_STOP)
				draw_simple(path->entry, path->exit, x, y);
			break;
		case TOKEN_LASER:
			if (path->entry == LOC_STOP)
				switch (token->dir) {
				case DIR_NORTH:
					dline(x + 6, y, x + 6, y + 4, C_LIGHT);
					break;
				case DIR_EAST:
					dline(x + 12, y + 6, x + 8, y + 6,
						C_LIGHT);
					break;
				case DIR_SOUTH:
					dline(x + 6, y + 12, x + 6, y + 8,
						C_LIGHT);
					break;
				case DIR_WEST:
					dline(x, y + 6, x + 4, y + 6, C_LIGHT);
					break;
				}
			else
				switch (path->entry) {
				case LOC_NORTH:
					dline(x + 6, y, x + 6, y + 2, C_LIGHT);
					break;
				case LOC_EAST:
					dline(x + 12, y + 6, x + 10, y + 6,
						C_LIGHT);
					break;
				case LOC_SOUTH:
					dline(x + 6, y + 12, x + 6, y + 10,
						C_LIGHT);
					break;
				case LOC_WEST:
					dline(x, y + 6, x + 2, y + 6, C_LIGHT);
					break;
				case LOC_STOP:
					break;
				}
			break;
		case TOKEN_TARGET:
			if (path->exit == LOC_STOP) {
				if ((int)path->entry == (int)token->dir)
					switch (token->dir) {
					case DIR_NORTH:
						dimage(x + 3, y + 1,
							&img_hit_n);
						break;
					case DIR_EAST:
						dimage(x + 8, y + 3,
							&img_hit_e);
						break;
					case DIR_SOUTH:
						dimage(x + 3, y + 8,
							&img_hit_s);
						break;
					case DIR_WEST:
						dimage(x + 1, y + 3,
							&img_hit_w);
						break;
					}
			} else {
				draw_simple(path->entry, path->exit, x, y);
			}
			break;
		}
	}
}

void display_game()
{
	// Prepare gray engine
	display_init_gray();

	// Draw background image
	dimage(0, 0, (gint[HWCALC] == HWCALC_FXCG100) ? &img_background_cg100 : &img_background);

	// Draw each token
	for (int row = 0; row < GRID_HEIGHT; ++row) {
		int y = 12 * row + 2;
		for (int col = 0; col < GRID_WIDTH; ++col) {
			int x = 12 * col + 34;
			token_t *token = game_get_token(row, col);
			bopti_image_t *img = NULL;
			int corner = CORNER_NE;
			int invert = 0;
			switch (token->type) {
			case TOKEN_NONE:
				break;
			case TOKEN_BLOCK:
				img = &img_token_block;
				invert = 1;
				break;
			case TOKEN_CHECKPOINT:
				switch (token->dir) {
				case DIR_NORTH:
				case DIR_SOUTH:
					img = &img_token_checkpoint_ns;
					break;
				case DIR_EAST:
				case DIR_WEST:
					img = &img_token_checkpoint_ew;
					break;
				}
				invert = 1;
				break;
			case TOKEN_LASER:
				switch (token->dir) {
				case DIR_NORTH:
					img = &img_token_laser_n;
					break;
				case DIR_EAST:
					img = &img_token_laser_e;
					break;
				case DIR_SOUTH:
					img = &img_token_laser_s;
					break;
				case DIR_WEST:
					img = &img_token_laser_w;
					break;
				}
				break;
			case TOKEN_MIRROR:
				switch (token->dir) {
				case DIR_NORTH:
				case DIR_SOUTH:
					img = &img_token_mirror_nwse;
					break;
				case DIR_EAST:
				case DIR_WEST:
					img = &img_token_mirror_nesw;
					corner = CORNER_NW;
					break;
				}
				break;
			case TOKEN_SPLITTER:
				switch (token->dir) {
				case DIR_NORTH:
				case DIR_SOUTH:
					img = &img_token_splitter_nwse;
					break;
				case DIR_EAST:
				case DIR_WEST:
					img = &img_token_splitter_nesw;
					corner = CORNER_NW;
					break;
				}
				break;
			case TOKEN_TARGET:
				if (token->req_target)
					switch (token->dir) {
					case DIR_NORTH:
						img = &img_token_target_req_n;
						corner = CORNER_SW;
						break;
					case DIR_EAST:
						img = &img_token_target_req_e;
						corner = CORNER_NW;
						break;
					case DIR_SOUTH:
						img = &img_token_target_req_s;
						break;
					case DIR_WEST:
						img = &img_token_target_req_w;
						corner = CORNER_SE;
						break;
					}
				else
					switch (token->dir) {
					case DIR_NORTH:
						img = &img_token_target_n;
						corner = CORNER_SW;
						break;
					case DIR_EAST:
						img = &img_token_target_e;
						corner = CORNER_NW;
						break;
					case DIR_SOUTH:
						img = &img_token_target_s;
						break;
					case DIR_WEST:
						img = &img_token_target_w;
						corner = CORNER_SE;
						break;
					}
				break;
			}

			if (img) {
				dimage(x, y, img);

				img = NULL;
				if (token->can_move)
					img = &img_can_move;
				else if (token->can_rotate)
					img = &img_can_rotate;
			}

			if (img) {
				int y2 = y;
				int x2 = x;
				switch (corner) {
				case CORNER_NE:
					x2 += 8;
					break;
				case CORNER_SE:
					x2 += 8;
					y2 += 8;
					break;
				case CORNER_SW:
					y2 += 8;
					break;
				case CORNER_NW:
					break;
				}
				dimage(x2, y2, img);
				if (invert)
					drect(x2, y2, x2 + 2, y2 + 2, C_INVERT);
			}

			if (game_is_selection(row, col))
				dimage(x - 1, y - 1, &img_selection);
			else if (game_is_cursor(row, col))
				dimage(x - 1, y - 1, &img_cursor);
		}
	}

	// Draw puzzle ID, target completion, and solve/win status
	dprint(114, 1, C_BLACK, "%i", game_get_puzzle_id());
	for (int i = 0; i < get_targets_req(); ++i) {
		bopti_image_t *img = i < get_targets_hit() ? &img_target_hit :
							&img_target_missed;
		dimage(8 * i + 100, 9, img);
	}
	if (game_is_solved())
		dimage(101, 1, &img_solved);
	if (game_is_total_winner())
		dprint(98, 29, C_LIGHT, "YOU WIN!");

	// Draw laser beam
	draw_laser();

	// Draw VRAM to display
	debug();
	dupdate();
}

void display_help1()
{
	display_init_gray();
	dimage(0, 0, (gint[HWCALC] == HWCALC_FXCG100) ? &img_help1_cg100 : &img_help1);
	debug();
	dupdate();
}

void display_help2()
{
	display_init_gray();
	dclear(C_WHITE);
	dprint(2, 2, C_BLACK, "HOW TO PLAY");
	dline(2, 8, 41, 8, C_BLACK);
	dprint(1, 12, C_BLACK, "*MOVE/ROTATE TOKENS TO HIT TARGETS;");
	dprint(5, 18, C_BLACK, "TARGETS LIGHT UP WHEN HIT. NOT ALL");
	dprint(5, 24, C_BLACK, "TOKENS CAN MOVE/ROTATE.");
	dprint(1, 32, C_BLACK, "*HIT THE INDICATED # OF TARGETS,");
	dprint(5, 38, C_BLACK, "INCLUDING ALL REQUIRED TARGETS.");
	dprint(1, 46, C_BLACK, "*USE EVERY TOKEN (BLOCK EXCEPTED).");
	dimage(0, 55, (gint[HWCALC] == HWCALC_FXCG100) ? &img_help2_cg100 : &img_help2);
	dupdate();
}

void display_file_error(int rc, const char *op, const char *filename)
{
	dgray(DGRAY_OFF);
	dfont(NULL);
	dclear(C_WHITE);
	dprint(0, 0, C_BLACK, "Error %i %s", rc, op);
	dprint(0, 8, C_BLACK, "%s", filename);
	dprint(0, 28, C_BLACK, "Press any key to exit");
	dupdate();
}

bool display_is_using_gray_engine(void)
{
	return display_is_gray;
}
