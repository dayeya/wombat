global _start
; #[extern(linkage)]
extern putchar
extern putnum

section .data

section .text
_start:
	call main
	
	mov rax, 60 ; emit syscall: exit
	mov rdi, 0 ; exit code of 0 (success)
	syscall
foo:
	push rbp
	mov rbp, rsp
	
	mov qword [rbp - 8], rdi
	mov qword [rbp - 16], rsi
	mov qword [rbp - 24], rdx
	mov rax, qword [rbp - 24]
	
	mov rsp, rbp
	pop rbp
	ret
main:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	
	mov rdi, 7
	mov rsi, 3
	mov rdx, 1
	call foo
	mov qword [rbp - 8], rax
	mov rdi, qword [rbp - 8]
	call putnum
	
	mov rsp, rbp
	pop rbp
	ret
