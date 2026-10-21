#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
as --32 boot.S -o boot.o
ld -m elf_i386 -Ttext 0x7c00 --oformat binary -o boot.bin boot.o
gcc -m32 -Os -ffreestanding -fno-pic -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -nostdlib -c kernel.c -o kernel.o
as --32 entry.S -o entry.o
ld -m elf_i386 -T linker.ld --oformat binary -o kernel.bin entry.o kernel.o
test "$(stat -c %s boot.bin)" -eq 512
test "$(stat -c %s kernel.bin)" -le 27648
truncate -s 1474560 meplay.img
dd if=boot.bin of=meplay.img conv=notrunc status=none
dd if=kernel.bin of=meplay.img bs=512 seek=1 conv=notrunc status=none
printf 'Built meplay.img (%s-byte kernel)\n' "$(stat -c %s kernel.bin)"
