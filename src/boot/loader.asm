global start

MAGIC equ 0x1BADB002
FLAGS equ 0x0
CHECKSUM equ -MAGIC

section .text
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

start:
    mov eax, 0xCAFEBABE
    extern main
    call main
.loop:
    jmp .loop


