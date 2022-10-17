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

#include <stdint.h>

#define GRID_WIDTH 5
#define GRID_HEIGHT 5
#define GRID_SIZE (GRID_WIDTH * GRID_HEIGHT)

#define BLOCK_COUNT 1
#define CHECKPOINT_COUNT 1
#define LASER_COUNT 1
#define MIRROR_COUNT 1
#define SPLITTER_COUNT 2
#define TARGET_COUNT 5
#define TOKEN_COUNT (BLOCK_COUNT + CHECKPOINT_COUNT + LASER_COUNT + \
			MIRROR_COUNT + SPLITTER_COUNT + TARGET_COUNT)

#define BEAM_MAX 2 * GRID_SIZE * SPLITTER_COUNT

#define BYTES_PER_PUZZLE (2 + 2 * TOKEN_COUNT)
#define PUZZLE_COUNT 60 /* Must be even */
#define PUZZLE_BYTES (PUZZLE_COUNT * BYTES_PER_PUZZLE)
#define PUZZLE_FILENAME "LASER.dat"
#define SOLVED_FILENAME "LASER.cfg"

typedef enum __attribute__((__packed__)) {
	DIR_NORTH,
	DIR_EAST,
	DIR_SOUTH,
	DIR_WEST
} dir_t;

typedef enum __attribute__((__packed__)) {
	TOKEN_NONE,
	TOKEN_BLOCK,
	TOKEN_CHECKPOINT,
	TOKEN_LASER,
	TOKEN_MIRROR,
	TOKEN_SPLITTER,
	TOKEN_TARGET
} token_type_t;

typedef enum __attribute__((__packed__)) {
	LOC_NORTH,
	LOC_EAST,
	LOC_SOUTH,
	LOC_WEST,
	LOC_STOP
} loc_t;

typedef struct {
	token_type_t type;
	dir_t dir;
	uint8_t can_move;
	uint8_t can_rotate;
	uint8_t req_target;
	uint8_t hit;
} token_t;

typedef struct {
	int8_t row;
	int8_t col;
	loc_t entry;
	loc_t exit;
} path_t;

int game_init(int init_solved);
int game_is_cursor(int row, int col);
int game_is_selection(int row, int col);
int game_is_solved(void);
int game_is_total_winner(void);
char *game_get_puzzles(void);
char *game_get_solved(void);
int game_get_puzzle_id(void);
token_t *game_get_token(int row, int col);
int game_get_path_count(void);
path_t *game_get_path(int i);
int get_targets_req(void);
int get_targets_hit(void);
void game_cursor_row(int dir);
void game_cursor_col(int dir);
void game_select_token(void);
void game_deselect_token(void);
void game_rotate_token(int dir);
int game_laser(void);
void game_next_puzzle(void);
void game_previous_puzzle(void);
