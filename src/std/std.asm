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
    push    rdi                 ; save original input number

    mov     rax, rdi
    test    rax, rax
    jns     .convert            ; jump if non-negative

    ; Handle negative numbers
    neg     rax

    ; Print minus sign
    mov     rax, 1              ; syscall: write
    mov     rdi, 1              ; stdout
    lea     rsi, [minus_sign]
    mov     rdx, 1              ; length
    syscall

.convert:
    lea     rsi, [buffer + 20]  ; point to end of buffer
    mov     rcx, 10

.convert_loop:
    xor     rdx, rdx
    div     rcx                 ; divide rax by 10 → quotient in rax, remainder in rdx
    dec     rsi
    add     dl, '0'
    mov     [rsi], dl
    test    rax, rax
    jnz     .convert_loop

    ; Write number string
    mov     rdx, buffer + 20
    sub     rdx, rsi            ; rdx = length of number string
    mov     rax, 1              ; syscall: write
    mov     rdi, 1              ; stdout
    ; rsi already points to start of number string
    syscall

    ; Print newline
    mov     rax, 1              ; syscall: write
    mov     rdi, 1              ; stdout
    lea     rsi, [newline]
    mov     rdx, 1              ; length
    syscall

    pop     rdi                 ; restore rdi
    mov     rsp, rbp
    pop     rbp
    ret

putchar:
    sub     rsp, 8              ; allocate space on stack
    mov     byte [rsp], dil     ; store the character (lower byte of rdi)
    
    mov     eax, 1              ; syscall: write
    mov     edi, 1              ; fd: stdout
    mov     rsi, rsp            ; pointer to character
    mov     edx, 1              ; length: 1 byte
    syscall

    add     rsp, 8              ; clean up
    ret