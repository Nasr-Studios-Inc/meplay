ENTRY(_kernel_entry)
SECTIONS { . = 0x10000; .text : { KEEP(*(.text.entry)) *(.text*) } .rodata : { *(.rodata*) } .data : { *(.data*) } .bss : { *(.bss*) *(COMMON) } }
