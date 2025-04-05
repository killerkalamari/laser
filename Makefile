# Laser Logic
# Copyright (C) 2022, 2025  Jeffry Johnston
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
version := 01.40

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
	background_cg100.png	\
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
	help1_cg100.png		\
	help2.png		\
	help2_cg100.png		\
	font_laser.png		\

.PHONY: all
all: fx fxg3a

FX_CC := sh-elf-gcc
FX_CFLAGS := -DFX9860G -DTARGET_FX9860G -m3 -mb -ffreestanding -nostdlib \
	-Wa,--dsp -Wall -Wextra -std=c11 -g -Os -fstrict-volatile-bitfields
FX_LDFLAGS := -nostdlib -Wl,--no-warn-rwx-segments -T fx9860g.ld
fx_add_in := build_fx/$(name).g1a
fx_bin := build_fx/$(name).bin
fx_elf := build_fx/$(name).elf
fx_objs := $(srcs:%=build_fx/%.o) $(images:%=build_fx/%.o)
fx_libs := $(shell $(FX_CC) -print-file-name=libgint-fx.a) \
	$(shell $(FX_CC) -print-file-name=libc.a) -lgint-fx -lopenlibm -lc -lgcc
fx_icon := assets/icon.png

.PHONY: fx
fx: $(fx_add_in)

$(fx_add_in): $(fx_bin) $(fx_icon)
	fxgxa --g1a -n $(name) -i $(fx_icon) --version="$(version)" -o $@ $<

$(fx_bin): $(fx_elf)
	sh-elf-objcopy -O binary -R .bss -R .gint_bss $< $@

$(fx_elf): $(fx_objs)
	$(FX_CC) $(FX_LDFLAGS) -o $@ $^ $(fx_libs) $(fx_libs)

build_fx/%.c.o: src/%.c $(headers)
	$(FX_CC) $(FX_CFLAGS) -c -o $@ $<

build_fx/%.png.o: assets/%.png assets/fxconv-metadata.txt
	fxconv --toolchain=sh-elf --fx -o $@ $<

FXG3A_CC := sh-elf-gcc
FXG3A_CFLAGS := -DFXCG50 -DFX9860G_G3A -m4-nofpu -mb -ffreestanding -nostdlib \
	-Wa,--dsp -Wall -Wextra -std=c11 -g -Os -fstrict-volatile-bitfields
FXG3A_LDFLAGS := -nostdlib -Wl,--no-warn-rwx-segments -T fxcg50.ld
fxg3a_add_in := build_fxg3a/$(name).g3a
fxg3a_bin := build_fxg3a/$(name).bin
fxg3a_elf := build_fxg3a/$(name).elf
fxg3a_objs := $(srcs:%=build_fxg3a/%.o) $(images:%=build_fxg3a/%.o)
fxg3a_libs := $(shell $(FXG3A_CC) -print-file-name=libgint-fxg3a.a) \
	$(shell $(FXG3A_CC) -print-file-name=libc.a) -lgint-fxg3a -lopenlibm -lc -lgcc
fxg3a_icon_uns := assets/icon_uns.png
fxg3a_icon_sel := assets/icon_sel.png

.PHONY: fxg3a
fxg3a: $(fxg3a_add_in)

$(fxg3a_add_in): $(fxg3a_bin) $(fxg3a_icon_uns) $(fxg3a_icon_sel)
	fxgxa --g3a -n $(name) --icon-uns $(fxg3a_icon_uns) --icon-sel $(fxg3a_icon_sel) --version="$(version)" -o $@ $<

$(fxg3a_bin): $(fxg3a_elf)
	sh-elf-objcopy -O binary -R .bss -R .gint_bss $< $@

$(fxg3a_elf): $(fxg3a_objs)
	$(FXG3A_CC) $(FXG3A_LDFLAGS) -o $@ $^ $(fxg3a_libs) $(fxg3a_libs)

build_fxg3a/%.c.o: src/%.c $(headers)
	$(FXG3A_CC) $(FXG3A_CFLAGS) -c -o $@ $<

build_fxg3a/%.png.o: assets/%.png assets/fxconv-metadata.txt
	fxconv --toolchain=sh-elf --fx -o $@ $<

$(shell mkdir -p build_fx build_fxg3a)

# Install on Casio fx-9750/9860 GIII
.PHONY: install_fx
install_fx: $(fx_add_in)
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
	rm -f "$$DEST/"$(name).g1a; \
	cp -v $(fx_add_in) "$$DEST"/; \
	eject "$$DEST" 2>/dev/null || true

# Install on Casio fx-CG50
.PHONY: install_cg
install_cg: $(fxg3a_add_in)
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
	rm -f "$$DEST/"$(name).g3a; \
	cp -v $(fxg3a_add_in) "$$DEST"/; \
	eject "$$DEST" 2>/dev/null || true

# Install on Casio fx-9750/9860 G/GII
.PHONY: install_p7
install_p7: $(fx_add_in)
	p7 send $(fx_add_in)

.PHONY: clean
clean:
	$(RM) -r build_fx/ build_fxg3a/
