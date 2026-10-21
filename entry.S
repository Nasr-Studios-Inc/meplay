.code32
.section .text.entry,"ax"
.global _kernel_entry
_kernel_entry:
 call kernel_main
1: hlt
 jmp 1b
