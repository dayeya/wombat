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
	
	mov rdi, 69420
	call putnum
	
	mov rsp, rbp
	pop rbp
	ret
