section .data
    buffer      db 20 dup(0)      ; buffer for digits
    newline     db 10             ; newline character
    minus_sign  db '-'            ; minus sign

section .text
global putnum
global putchar

putnum:
    push    rbp
    mov     rbp, rsp
    push    rdi                     ; save original input number (rdi_original) at [rbp - 8]

    mov     rax, rdi                ; rax = rdi_original
    test    rax, rax
    jns     .convert                ; jump if non-negative

    ; Print minus sign
    mov     rax, 1                  ; syscall number for write
    mov     rdi, 1                  ; file descriptor: stdout
    lea     rsi, [minus_sign]       ; pointer to minus sign character
    mov     rdx, 1                  ; length: 1 byte
    syscall                         ; Execute write syscall. This clobbers rax and rdi.

    ; Get absolute value in a temp register
    ; BUG FIX: Retrieve the original input number from the stack,
    ; as rdi was clobbered by the previous syscall.
    mov     rax, qword [rbp - 8]    ; Load the original input number from where it was saved
    neg     rax                     ; rax now has the absolute value (e.g., 8 for -8)

.convert:
    lea     rsi, [buffer + 20]      ; rsi points to one byte past the end of the buffer
    mov     rcx, 10                 ; Divisor (for base 10 conversion)

.convert_loop:
    xor     rdx, rdx                ; Clear rdx before division (rdx:rax is dividend)
    div     rcx                     ; Divide rax by 10. Quotient in rax, remainder in rdx.
    dec     rsi                     ; Move rsi to the left (towards start of buffer)
    add     dl, '0'                 ; Convert remainder digit to its ASCII character
    mov     [rsi], dl               ; Store the character in the buffer
    test    rax, rax                ; Check if quotient (rax) is zero
    jnz     .convert_loop           ; If not zero, continue loop

    ; Calculate string length
    mov     rdx, buffer + 20        ; End of the buffer
    sub     rdx, rsi                ; Subtract current rsi position to get length

    ; Write number string
    mov     rax, 1                  ; syscall number for write
    mov     rdi, 1                  ; file descriptor: stdout
    ; rsi already points to the number string in the buffer
    ; rdx already contains the calculated length
    syscall                         ; Execute write syscall

    ; Print newline
    mov     rax, 1                  ; syscall number for write
    mov     rdi, 1                  ; file descriptor: stdout
    lea     rsi, [newline]          ; pointer to newline character
    mov     rdx, 1                  ; length: 1 byte
    syscall                         ; Execute write syscall

    pop     rdi                     ; Restore original rdi before returning (good practice)
    mov     rsp, rbp                ; Restore stack pointer
    pop     rbp                     ; Restore base pointer
    ret                             ; Return from function

putchar:
    sub     rsp, 8              ; Allocate space on stack for the character
    mov     byte [rsp], dil     ; Store the character (lower byte of rdi) at the allocated space
    
    mov     eax, 1              ; syscall number: write
    mov     edi, 1              ; file descriptor: stdout
    mov     rsi, rsp            ; pointer to the character on the stack
    mov     edx, 1              ; length: 1 byte
    syscall                     ; Execute write syscall

    add     rsp, 8              ; Clean up stack space
    ret                         ; Return from function