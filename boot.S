.code16
.section .text
.global _start
_start:
 cli
 xor %ax,%ax
 mov %ax,%ds
 mov %ax,%ss
 mov $0x7c00,%sp
 mov %dl,drive
 # BIOS VGA text mode 03h (80x25)
 mov $0x0003,%ax
 int $0x10
 # Load 54 sectors from floppy, beginning CHS 0/0/2, to 1000:0000
 mov $0x1000,%ax
 mov %ax,%es
 xor %bx,%bx
 mov $0,%ch
 mov $0,%dh
 mov $2,%cl
 mov $54,%si
load_sector:
 mov $0x02,%ah
 mov $1,%al
 mov drive,%dl
 int $0x13
 jc disk_error
 add $512,%bx
 jnc no_wrap
 mov %es,%ax
 add $0x1000,%ax
 mov %ax,%es
no_wrap:
 inc %cl
 cmp $19,%cl
 jne next_sector
 mov $1,%cl
 xor $1,%dh
 jnz next_sector
 inc %ch
next_sector:
 dec %si
 jnz load_sector
 cli
 lgdt gdt_desc
 mov %cr0,%eax
 or $1,%eax
 mov %eax,%cr0
 ljmp $0x08,$protected
 disk_error:
 hlt
 jmp disk_error
.code32
protected:
 mov $0x10,%ax
 mov %ax,%ds
 mov %ax,%es
 mov %ax,%fs
 mov %ax,%gs
 mov %ax,%ss
 mov $0x90000,%esp
 mov $0x10000,%eax
 jmp *%eax
.align 8
gdt:
 .quad 0
 .quad 0x00cf9a000000ffff
 .quad 0x00cf92000000ffff
gdt_end:
gdt_desc:
 .word gdt_end-gdt-1
 .long gdt
drive: .byte 0
.org 510
.word 0xaa55
