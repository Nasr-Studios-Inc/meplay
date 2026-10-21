# MePlay Native OS V1 — Shell Edition

## Changes
- VGA BIOS mode 03h: native 80x25 text console instead of the old 320x200 pixel-tile home screen.
- Full browser viewport is dedicated to the emulator, without the blue frame.
- The OS now starts in a real native C kernel command interpreter.

## Commands
`help`, `clear`, `cls`, `version`, `uname`, `about`, `echo TEXT`, `ls`, `pwd`, `whoami`, `apps`, `settings`, `mem`, `uptime`, `reboot`, `shutdown`, `color blue`, `color black`.

Commands run inside the native x86 guest kernel, not in the browser or on the host. This is a basic built-in command interpreter, not yet a POSIX shell; there is no filesystem driver or executable loader.

## Run
Keep your working v86 files in `vendor/`: `libv86.js`, `v86.wasm`, `seabios.bin`, `vgabios.bin`.
Deploy the entire folder over HTTP(S). The `meplay.img` must be replaced with the new image. For QEMU:
`qemu-system-i386 -drive file=meplay.img,format=raw,if=floppy -boot a -m 32`

To rebuild on Linux, run `bash build.sh` (requires 32-bit capable GCC, binutils).
The cosmic boot controller is unchanged. Browser runtime files are not included.

## Limitations
Native emulator boot has not been tested in this environment. Native keyboard uses PS/2 set 1 scancodes. Text-mode rendering scaling is dependent on v86 runtime CSS. The original boot loader reads a fixed 54-sector kernel region.
