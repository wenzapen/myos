global start

MAGIC equ 0x1BADB002
FLAGS equ 0x1
CHECKSUM equ -MAGIC - FLAGS

section .text
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

start:
    mov eax, 0xCAFEBABE
    extern main
    push ebx
    call main
.loop:
    jmp .loop


