section .data
    buffer          db 20 dup(0)      ; buffer for digits for putnum
    newline         db 10             ; newline character
    minus_sign      db '-'            ; minus sign
    input_buffer    db 20 dup(0)      ; buffer for readnum input
    input_buffer_len equ 20
    char_input_buffer db 1            ; buffer for readchar input (1 byte)

section .text
global putnum
global putchar
global readnum
global readchar
global quit

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
    ; Retrieve the original input number from the stack,
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
    push    rbp
    mov     rbp, rsp
    sub     rsp, 8              ; Allocate space on stack for the character
    mov     byte [rsp], dil     ; Store the character (lower byte of rdi) at the allocated space
    
    mov     eax, 1              ; syscall number: write
    mov     edi, 1              ; fd: stdout
    mov     rsi, rsp            ; pointer to character
    mov     edx, 1              ; length: 1 byte
    syscall                     ; Execute write syscall

    add     rsp, 8              ; Clean up stack space
    mov     rsp, rbp
    pop     rbp
    ret                         ; Return from function

readnum:
    push    rbp
    mov     rbp, rsp
    
    ; Read input from stdin into input_buffer
    mov     rax, 0                  ; syscall number for read
    mov     rdi, 0                  ; file descriptor: stdin
    lea     rsi, [input_buffer]     ; pointer to input buffer
    mov     rdx, input_buffer_len   ; max bytes to read
    syscall                         ; Execute read syscall. rax will contain bytes read.

    ; rax now holds the number of bytes read.
    ; rdx will be used as a counter for the loop.
    ; rsi will be used as a pointer to the current character.
    mov     rdx, rax                ; rdx = bytes read
    lea     rsi, [input_buffer]     ; rsi points to the start of the buffer

    xor     rax, rax                ; rax will accumulate the number (result)
    mov     rbx, 1                  ; rbx will store the sign (1 for positive, -1 for negative)

    ; Check for negative sign
    cmp     byte [rsi], '-'         ; Check if the first character is '-'
    jne     .start_conversion       ; If not, jump to start conversion

    mov     rbx, -1                 ; Set sign to negative
    inc     rsi                     ; Move past the '-' sign
    dec     rdx                     ; Decrement bytes read counter

.start_conversion:
    ; Loop through characters and convert to number
.convert_char_loop:
    cmp     rdx, 0                  ; Check if all characters have been processed
    je      .end_readnum            ; If yes, end of function

    movzx   rcx, byte [rsi]         ; Load current character into rcx (zero-extended)
    cmp     rcx, 10                 ; Check for newline character (ASCII 10)
    je      .end_readnum            ; If newline, end conversion
    cmp     rcx, 13                 ; Check for carriage return (ASCII 13)
    je      .end_readnum            ; If carriage return, end conversion

    cmp     rcx, '0'                ; Check if character is less than '0'
    jl      .end_readnum            ; If not a digit, stop conversion
    cmp     rcx, '9'                ; Check if character is greater than '9'
    jg      .end_readnum            ; If not a digit, stop conversion

    sub     rcx, '0'                ; Convert ASCII digit to its numeric value
    
    imul    rax, 10                 ; Multiply current result by 10
    add     rax, rcx                ; Add the new digit

    inc     rsi                     ; Move to the next character
    dec     rdx                     ; Decrement bytes read counter
    jmp     .convert_char_loop      ; Continue loop

.end_readnum:
    imul    rax, rbx                ; Apply the sign to the final number

    mov     rsp, rbp
    pop     rbp
    ret                             ; Return the number in rax

readchar:
    push    rbp
    mov     rbp, rsp

    ; Read a single character from stdin
    mov     rax, 0                  ; syscall number for read
    mov     rdi, 0                  ; file descriptor: stdin
    lea     rsi, [char_input_buffer] ; pointer to single-byte buffer
    mov     rdx, 1                  ; length: 1 byte
    syscall                         ; Execute read syscall. rax will contain bytes read (should be 1).

    ; The character read is now in char_input_buffer
    ; We need to return it in rax.
    movzx   rax, byte [char_input_buffer] ; Load the character into rax (zero-extended)

    mov     rsp, rbp
    pop     rbp
    ret                             ; Return the character in rax (specifically, al)

quit:
    ; Terminate the program with the provided exit code
    ; The exit code is passed in rdi (first argument)
    mov     rax, 60                 ; syscall number for exit
    ; rdi already contains the exit code passed by the caller
    syscall                         ; Execute exit syscall. This does not return.