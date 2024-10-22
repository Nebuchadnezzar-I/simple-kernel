bits 32 ; NASM directive for 32-bit mode
section .text
    align 4
    dd 0x1BADB002            ; Multiboot header magic number
    dd 0x00000003            ; Flags (bit 0 = align modules, bit 1 = request memory info)
    dd -(0x1BADB002 + 0x00000003) ; Checksum

global start
extern k_main

start:
    cli                     ; Clear interrupts
    mov esi, [esp + 8]      ; Get the Multiboot information structure address (passed by GRUB)
    call parse_multiboot

    call k_main             ; Call the C kernel function
    hlt                     ; Halt the CPU

parse_multiboot:
    ; ESI points to the multiboot information structure
    ; We will extract the video information here

    mov ebx, [esi + 44]     ; Get framebuffer width (offset 44 in Multiboot info structure)
    mov ecx, [esi + 48]     ; Get framebuffer height (offset 48 in Multiboot info structure)

    ; Store the values in our screen_cols and screen_rows
    mov [screen_cols], bl   ; Store lower byte of EBX (width)
    mov [screen_rows], cl   ; Store lower byte of ECX (height)

    ret

section .data
    screen_cols db 0        ; Variable to store screen columns
    screen_rows db 0        ; Variable to store screen rows

