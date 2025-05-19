global _start
; #[extern(linkage)]
extern putchar
extern putnum

section .data

section .text
_start:
	call main

	; wombat --> exit(0)
	mov rax, 60 ; emit syscall: exit
	mov rdi, 0 ; exit code of 0. (success)
	syscall

main:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	
	; #[alloc(8 bytes for x)]
	mov rax, 69
	mov qword [rbp - 8], rax

	mov rdi, qword [rbp - 8]
	call putnum
	
	mov rsp, rbp
	pop rbp
	ret
