bits 32
section .text
    align 4
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 0x00000003)

global start
extern k_main

start:
    cli
    mov esi, [esp + 8]
    call parse_multiboot

    call k_main
    hlt

parse_multiboot:
    mov ebx, [esi + 44]
    mov ecx, [esi + 48]

    mov [screen_cols], bl
    mov [screen_rows], cl

    ret

section .data
    screen_cols db 0mns
    screen_rows db 0

