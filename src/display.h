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

#pragma once

#include <stdbool.h>

void display_init(void);
void display_init_mono(const bool clear);
void display_menu_return(void);
void display_game(void);
void display_help1(void);
void display_help2(void);
void display_file_error(int rc, const char *op, const char *filename);
bool display_is_using_gray_engine(void);
