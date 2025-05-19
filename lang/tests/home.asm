 ; global entry point
global _start
 ; external linkage
extern write
extern exit

section .data
	hello_msg db "Hello, world!", 10
	hello_len equ $ - hello_msg

section .text
_start:
 ; syscall write(stdout, hello_msg, hello_len)
	mov rax, 1
	mov rdi, 1
	mov rsi, hello_msg
	mov rdx, hello_len
	syscall
	
 ; syscall exit(0)
	mov rax, 60
	mov rdi, 0
	syscall

