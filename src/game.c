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

#include "file.h"
#include "game.h"

#define NO_SELECTION -1

typedef struct {
	uint8_t id;
	uint8_t targets_req;
	uint8_t targets_extra;
	uint8_t targets_hit;
	token_t grid[GRID_SIZE];
} puzzle_t;

static char puzzles[PUZZLE_BYTES];
static int puzzle_i;
static puzzle_t puzzle;
static char solved[PUZZLE_COUNT];
static int is_winner;

static int cursor_row;
static int cursor_col;
static int selection;
static int path_count;
static path_t beam[BEAM_MAX];

/*
For 60 puzzles:
	1 byte
	------
	ID

	1 byte
	------
	7 6 5 4 3 | 2 1
	RESERVED  | TARGETS

	11 pieces = 22 bytes
	--------------------
		1 byte: cell location
		---------------------
		7 6 5 | 4 3 2 1 0
		RSVD  | LOC

		1 byte: data
		------------
		7    | 6   | 5   | 4 3 | 2 1 0
		MOVE | ROT | REQ | DIR | TYPE
	SUBTOTAL: 24 bytes per puzzle
TOTAL: 60 * 24 = 1440 bytes in the file
*/
static void load_puzzle(void)
{
	char *p = puzzles + BYTES_PER_PUZZLE * puzzle_i;

	// Init fields
	selection = NO_SELECTION;
	path_count = 0;
	puzzle.id = *p++;
	puzzle.targets_req = *p++;
	puzzle.targets_hit = 0;
	token_t *grid = puzzle.grid;
	for (int i = 0; i < GRID_SIZE; ++i)
		puzzle.grid[i].type = TOKEN_NONE;

	// Add tokens
	for (int i = 0; i < TOKEN_COUNT; ++i) {
		int loc = *p++;
		token_t *token = &grid[loc];
		int data = *p++;
		int type = data & 0x07;
		if (type != TOKEN_NONE) {
			token->type = type;
			data >>= 3;
			token->dir = data & 0x03;
			data >>= 2;
			token->req_target = data & 0x01;
			data >>= 1;
			token->can_rotate = data & 0x01;
			data >>= 1;
			token->can_move = data;
		}
	}


	// Find laser and set cursor
	int cell = 0;
	for (int i = 0; i < GRID_SIZE; ++i) {
		token_type_t type = puzzle.grid[i].type;
		if (type == TOKEN_LASER)
			cell = i;
	}
	cursor_row = cell / GRID_HEIGHT;
	cursor_col = cell % GRID_HEIGHT;

	// Determine number of extra targets
	int req = 0;
	for (int i = 0; i < GRID_SIZE; ++i)
		if (grid[i].type == TOKEN_TARGET && grid[i].req_target)
			++req;
	puzzle.targets_extra = puzzle.targets_req - req;
}

static int find_unsolved_puzzle(void)
{
	int unsolved = -1;
	for (int i = 0; i < PUZZLE_COUNT; ++i)
		if (solved[i] != '1') {
			unsolved = i;
			break;
		}
	if (unsolved == -1) {
		unsolved = PUZZLE_COUNT - 1;
		is_winner = 1;
	} else {
		is_winner = 0;
	}
	return unsolved;
}

int game_init(void)
{
	// Read puzzles
	int rc = file_read_puzzles(puzzles);
	if (rc)
		return rc;

	// Read completion status
	rc = file_read_completion(is_complete);
	if (rc)
		for (int i = 0; i < PUZZLE_COUNT; ++i)
			solved[i] = '0';

	// Find first unsolved puzzle
	puzzle_i = find_unsolved_puzzle();

	// Load puzzle
	load_puzzle();

	return 0;
}

int game_is_cursor(int row, int col)
{
	return (row == cursor_row) && (col == cursor_col);
}

int game_is_selection(int row, int col)
{
	return (GRID_WIDTH * row + col) == selection;
}

int game_is_solved(void)
{
	return solved[puzzle_i] == '1';
}

int game_is_total_winner(void)
{
	return is_winner;
}

int game_get_puzzle_id(void)
{
	return puzzle.id;
}

token_t *game_get_token(int row, int col)
{
	return &puzzle.grid[GRID_WIDTH * row + col];
}

int game_get_path_count(void)
{
	return path_count;
}

path_t *game_get_path(int i)
{
	return &beam[i];
}

int get_targets_req(void)
{
	return puzzle.targets_req;
}

int get_targets_hit(void)
{
	return puzzle.targets_hit;
}

void game_cursor_row(int dir)
{
	cursor_row += dir;
	if (cursor_row < 0)
		cursor_row = GRID_HEIGHT - 1;
	else if (cursor_row >= GRID_HEIGHT)
		cursor_row = 0;
}

void game_cursor_col(int dir)
{
	cursor_col += dir;
	if (cursor_col < 0)
		cursor_col = GRID_WIDTH - 1;
	else if (cursor_col >= GRID_WIDTH)
		cursor_col = 0;
}

void game_select_token(void)
{
	int i = GRID_WIDTH * cursor_row + cursor_col;
	token_t *token = &(puzzle.grid[i]);
	if (selection == NO_SELECTION) {
		if (token->type != TOKEN_NONE && token->can_move)
			selection = i;
	} else if (token->type == TOKEN_NONE) {
		*token = puzzle.grid[selection];
		puzzle.grid[selection].type = TOKEN_NONE;
		selection = NO_SELECTION;
	} else {
		selection = NO_SELECTION;
	}
}

void game_deselect_token(void)
{
	selection = NO_SELECTION;
}

void game_rotate_token(int dir)
{
	int i = GRID_WIDTH * cursor_row + cursor_col;
	token_t *token = &(puzzle.grid[i]);
	if (token->type == TOKEN_NONE || !(token->can_rotate))
		return;
	int new_dir = (int)(token->dir) + dir;
	if (new_dir < DIR_NORTH)
		new_dir = DIR_WEST;
	else if (new_dir > DIR_WEST)
		new_dir = DIR_NORTH;
	token->dir = new_dir;
}

static void add_path(int cell, loc_t entry, loc_t exit)
{
	if (path_count >= BEAM_MAX)
		return;

	int row = cell / GRID_HEIGHT;
	int col = cell % GRID_HEIGHT;
	for (int i = 0; i < path_count; ++i) {
		path_t *path_i = &beam[i];
		if (row == path_i->row && col == path_i->col &&
				entry == path_i->entry && exit == path_i->exit)
			return;
	}

	path_t *path = &beam[path_count++];
	path->row = row;
	path->col = col;
	path->entry = entry;
	path->exit = exit;
}

static loc_t loc_across(loc_t loc)
{
	switch (loc) {
	case LOC_NORTH:
		return LOC_SOUTH;
	case LOC_EAST:
		return LOC_WEST;
	case LOC_SOUTH:
		return LOC_NORTH;
	case LOC_WEST:
		return LOC_EAST;
	case LOC_STOP:
		break;
	}
	return LOC_STOP;
}

static loc_t loc_reflect(dir_t dir, loc_t loc)
{
	if (dir == DIR_NORTH || dir == DIR_SOUTH)
		switch (loc) {
		case LOC_NORTH:
			return LOC_EAST;
		case LOC_EAST:
			return LOC_NORTH;
		case LOC_SOUTH:
			return LOC_WEST;
		case LOC_WEST:
			return LOC_SOUTH;
		case LOC_STOP:
			return LOC_STOP;
		}
	switch (loc) {
	case LOC_NORTH:
		return LOC_WEST;
	case LOC_EAST:
		return LOC_SOUTH;
	case LOC_SOUTH:
		return LOC_EAST;
	case LOC_WEST:
		return LOC_NORTH;
	case LOC_STOP:
		break;
	}
	return LOC_STOP;
}

int game_laser(void)
{
	// Find laser and count tokens (except block)
	int cell = -1;
	int tokens_req = 0;
	for (int i = 0; i < GRID_SIZE; ++i) {
		puzzle.grid[i].hit = 0;
		token_type_t type = puzzle.grid[i].type;
		if (type == TOKEN_LASER)
			cell = i;
		if (type != TOKEN_NONE && type != TOKEN_BLOCK &&
			type != TOKEN_LASER)
			++tokens_req;
	}
	if (cell == -1)
		return 0;

	// Trace beam
	int tokens_hit = 0;
	int req_hit = 0;
	int extra_hit = 0;
	path_count = 0;
	add_path(cell, LOC_STOP, (int)(puzzle.grid[cell].dir));
	for (cell = 0; cell < path_count; ++cell) {
		// Move to next cell
		path_t *path = &beam[cell];
		loc_t entry = loc_across(path->exit);
		int row = path->row;
		int col = path->col;
		switch (path->exit) {
		case LOC_NORTH:
			if (row <= 0)
				continue;
			--row;
			break;
		case LOC_EAST:
			if (col >= GRID_WIDTH - 1)
				continue;
			++col;
			break;
		case LOC_SOUTH:
			if (row >= GRID_HEIGHT - 1)
				continue;
			++row;
			break;
		case LOC_WEST:
			if (col <= 0)
				continue;
			--col;
			break;
		case LOC_STOP:
			continue;
		}
		int cell = GRID_WIDTH * row + col;

		// Process cell
		token_t *token = &puzzle.grid[cell];
		if (!token->hit && token->type != TOKEN_NONE &&
				token->type != TOKEN_BLOCK &&
				token->type != TOKEN_LASER) {
			++tokens_hit;
			token->hit = 1;
		}
		loc_t exit = LOC_STOP;
		switch (token->type) {
		case TOKEN_NONE:
		case TOKEN_BLOCK:
			exit = loc_across(entry);
			break;
		case TOKEN_CHECKPOINT:
			switch (entry) {
			case LOC_NORTH:
			case LOC_SOUTH:
				if (token->dir == DIR_NORTH ||
						token->dir == DIR_SOUTH)
					exit = loc_across(entry);
				break;
			case LOC_EAST:
			case LOC_WEST:
				if (token->dir == DIR_EAST ||
						token->dir == DIR_WEST)
					exit = loc_across(entry);
				break;
			case LOC_STOP:
				break;
			}
			break;
		case TOKEN_LASER:
			break;
		case TOKEN_MIRROR:
			exit = loc_reflect(token->dir, entry);
			break;
		case TOKEN_SPLITTER:
			add_path(cell, entry, loc_across(entry));
			exit = loc_reflect(token->dir, entry);
			break;
		case TOKEN_TARGET:
			; loc_t dir = (int)(token->dir);
			loc_t dir2 = dir + 1;
			if (dir2 == LOC_STOP)
				dir2 = LOC_NORTH;
			if (entry == dir) {
				if (token->req_target)
					++req_hit;
				else if (extra_hit < puzzle.targets_extra)
					++extra_hit;
				exit = LOC_STOP;
			} else if (entry == dir2) {
				exit = LOC_STOP;
			} else {
				exit = loc_reflect(token->dir, entry);
			}
			break;
		}
		add_path(cell, entry, exit);
	}

	// Determine whether puzzle has been solved
	puzzle.targets_hit = req_hit + extra_hit;
	if (!game_is_solved() && puzzle.targets_hit >= puzzle.targets_req &&
			tokens_hit >= tokens_req) {
		solved[puzzle_i] = '1';
		find_unsolved_puzzle();
		int rc = file_write_completion(is_complete);
		if (rc)
			return rc;
	}

	return 0;
}

void game_next_puzzle(void)
{
	++puzzle_i;
	if (puzzle_i >= PUZZLE_COUNT)
		puzzle_i = 0;
	load_puzzle();
}

void game_previous_puzzle(void)
{
	--puzzle_i;
	if (puzzle_i < 0)
		puzzle_i = PUZZLE_COUNT - 1;
	load_puzzle();
}
