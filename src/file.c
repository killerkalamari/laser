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

#include <gint/bfile.h>
#include <gint/gint.h>
#include "game.h"

#define FLASH u"\\\\fls0\\"

static int read_file(const uint16_t *path, char *buf, const int size)
{
	const int fd = BFile_Open(path, BFile_ReadOnly);
	if (fd < 0)
		return 10;
	if (BFile_Read(fd, buf, size, 0) < 0)
		return 11;
	if (BFile_Close(fd) < 0)
		return 12;
	return 0;
}

static int write_file(const uint16_t *path, const char *buf, int size)
{
	BFile_Remove(path);
	if (BFile_Create(path, BFile_File, &size) < 0)
		return 20;
	int fd = BFile_Open(path, BFile_WriteOnly);
	if (fd < 0)
		return 21;
	if (BFile_Write(fd, buf, size) < 0)
		return 22;
	if (BFile_Close(fd) < 0)
		return 23;
	return 0;
}

int file_read_puzzles(char *buf)
{
	return gint_world_switch((gint_call_t) {
		.function = (void *)read_file,
		.args = {
			GINT_CALL_ARG(FLASH PUZZLE_FILENAME),
			GINT_CALL_ARG(buf),
			GINT_CALL_ARG(PUZZLE_BYTES)
		}
	});
}

int file_read_solved(char *buf)
{
	return gint_world_switch((gint_call_t) {
		.function = (void *)read_file,
		.args = {
			GINT_CALL_ARG(FLASH SOLVED_FILENAME),
			GINT_CALL_ARG(buf),
			GINT_CALL_ARG(PUZZLE_COUNT)
		}
	});
}

int file_write_solved(char *buf)
{
	return gint_world_switch((gint_call_t) {
		.function = (void *)write_file,
		.args = {
			GINT_CALL_ARG(FLASH SOLVED_FILENAME),
			GINT_CALL_ARG(buf),
			GINT_CALL_ARG(PUZZLE_COUNT)
		}
	});
}
