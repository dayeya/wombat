section .data
    buffer db 20 dup(0)    ; temporary buffer to store digits (20 is enough for 64-bit integers)
    newline db 10          ; newline character

section .text
global putnum

; Function: print_number
; Input: rdi = number to print
putnum:
    mov     rax, rdi            ; move number to rax for division
    lea     rsi, [buffer + 20]  ; rsi points to end of buffer
    mov     rcx, 10             ; base 10

.convert_loop:
    xor     rdx, rdx            ; clear rdx before div
    div     rcx                 ; rax = rax / 10, rdx = rax % 10
    dec     rsi
    add     dl, '0'             ; convert remainder to ASCII
    mov     [rsi], dl
    test    rax, rax
    jnz     .convert_loop

    ; write the number
    mov     rax, 1              ; syscall: write
    mov     rdi, 1              ; stdout
    mov     rdx, buffer + 20    ; end of buffer
    sub     rdx, rsi            ; length = end - start
    syscall

    ; write newline
    mov     rax, 1              ; syscall: write
    mov     rdi, 1              ; stdout
    mov     rsi, newline
    mov     rdx, 1
    syscall

    ret

putchar:
    mov     eax, 1          ; syscall number: write
    mov     edi, 1          ; file descriptor: stdout
    mov     rsi, rsp        ; use stack to temporarily hold the char
    push    rdi             ; push character (64 bits, but only lowest byte is used)
    mov     edx, 1          ; length = 1 byte
    syscall
    add     rsp, 8          ; clean up stack
    ret