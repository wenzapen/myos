global _disable_lapic

_disable_lapic:
    mov ecx, 0x1b
    rdmsr
    and eax, 0xf7ff
    wrmsr
    ret
