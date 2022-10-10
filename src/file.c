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

#include <gint/bfile.h>
#include <gint/gint.h>
#include "game.h"

#define FLASH u"\\\\fls0\\"

static int _rc;

static void read_file(const uint16_t *path, char *buf, int size)
{
	int fd = BFile_Open(path, BFile_ReadOnly);
	if (fd < 0) {
		_rc = 10;
		return;
	}
	int rc = BFile_Read(fd, buf, size, 0);
	if (rc < 0) {
		_rc = 11;
		return;
	}
	rc = BFile_Close(fd);
	if (rc < 0) {
		_rc = 12;
		return;
	}
	_rc = 0;
}

static void write_file(const uint16_t *path, char *buf, int size)
{
	BFile_Remove(path);
	int rc = BFile_Create(path, BFile_File, &size);
	if (rc < 0) {
		_rc = 20;
		return;
	}
	int fd = BFile_Open(path, BFile_WriteOnly);
	if (fd < 0) {
		_rc = 21;
		return;
	}
	rc = BFile_Write(fd, buf, size);
	if (rc < 0) {
		_rc = 22;
		return;
	}
	rc = BFile_Close(fd);
	if (rc < 0) {
		_rc = 23;
		return;
	}
	_rc = 0;
}

int file_read_puzzles(char *buf)
{
	gint_world_switch((gint_call_t) {
		.function = (void *)read_file,
		.args = {
			GINT_CALL_ARG(FLASH PUZZLE_FILENAME),
			GINT_CALL_ARG(buf),
			GINT_CALL_ARG(PUZZLE_BYTES)
		}
	});
	return _rc;
}

int file_read_completion(char *buf)
{
	gint_world_switch((gint_call_t) {
		.function = (void *)read_file,
		.args = {
			GINT_CALL_ARG(FLASH COMPLETION_FILENAME),
			GINT_CALL_ARG(buf),
			GINT_CALL_ARG(PUZZLE_COUNT)
		}
	});
	return _rc;
}

int file_write_completion(char *buf)
{
	gint_world_switch((gint_call_t) {
		.function = (void *)write_file,
		.args = {
			GINT_CALL_ARG(FLASH COMPLETION_FILENAME),
			GINT_CALL_ARG(buf),
			GINT_CALL_ARG(PUZZLE_COUNT)
		}
	});
	return _rc;
}
