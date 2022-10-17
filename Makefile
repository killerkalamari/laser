# Laser Logic
# Copyright (C) 2022  Jeffry Johnston
#
# This file is part of Laser Logic.
#
# Laser Logic is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Laser Logic is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Laser Logic.  If not, see <https://www.gnu.org/licenses/>.

name := Laser
version := 1.0

headers :=			\
 	src/display.h		\
 	src/file.h		\
 	src/game.h		\
 	src/kbd.h		\

srcs :=				\
	display.c		\
	file.c			\
	game.c			\
	kbd.c			\
	main.c			\

images :=			\
	background.png		\
	can_move.png		\
	can_rotate.png		\
	cursor.png		\
	hit_n.png		\
	hit_e.png		\
	hit_s.png		\
	hit_w.png		\
	logo.png		\
	selection.png		\
	solved.png		\
	target_hit.png		\
	target_missed.png	\
	token_block.png		\
	token_checkpoint_ns.png	\
	token_checkpoint_ew.png	\
	token_laser_n.png	\
	token_laser_e.png	\
	token_laser_s.png	\
	token_laser_w.png	\
	token_mirror_nesw.png	\
	token_mirror_nwse.png	\
	token_splitter_nesw.png	\
	token_splitter_nwse.png	\
	token_target_n.png	\
	token_target_e.png	\
	token_target_s.png	\
	token_target_w.png	\
	token_target_req_n.png	\
	token_target_req_e.png	\
	token_target_req_s.png	\
	token_target_req_w.png	\
	help1.png		\
	help2.png		\
	font_laser.png		\

icon := assets/icon.png

CC := sh-elf-gcc
CFLAGS := -DFX9860G -DTARGET_FX9860G -m3 -mb -ffreestanding -nostdlib \
		-Wa,--dsp -Wall -Wextra -Os -fstrict-volatile-bitfields
LDFLAGS := -nostdlib -Wl,--no-warn-rwx-segments -T fx9860g.ld

libs := $(shell $(CC) -print-file-name=libgint-fx.a) \
	$(shell $(CC) -print-file-name=libc.a) -lopenlibm -lgcc
objs := $(srcs:%=build/%.o) $(images:%=build/%.o)
elf := build/$(name).elf
bin := build/$(name).bin
filename := $(name).g1a
add_in := build/$(filename)

.PHONY: all
all: $(add_in)

$(add_in): $(bin) $(icon) Makefile
	fxgxa --g1a -n $(name) -i $(icon) --version="$(version)" -o $@ $<

$(bin): $(elf)
	sh-elf-objcopy -O binary -R .bss -R .gint_bss $< $@

$(elf): $(objs)
	$(CC) $(LDFLAGS) -o $@ $^ $(libs) $(libs)

build/%.c.o: src/%.c $(headers)
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.png.o: assets/%.png
	fxconv --toolchain=sh-elf --fx -o $@ $<

$(shell mkdir -p build)

.PHONY: install
install: $(add_in)
	@while true; do \
	    DEVICE=`blkid | grep LABEL_FATBOOT=\"CASIO\" | cut -d':' -f1`; \
	    [ -n "$$DEVICE" ] && break; \
	    sleep 0.25; \
	done; \
	while true; do \
	    DEST=`findmnt -nr -o target $$DEVICE`; \
	    [ -n "$$DEST" ] && break; \
	    sleep 0.25; \
	done; \
	rm -f "$$DEST/"$(filename); \
	cp -v $(add_in) "$$DEST"/; \
	eject "$$DEST" 2>/dev/null || true

.PHONY: clean
clean:
	$(RM) -r build/
