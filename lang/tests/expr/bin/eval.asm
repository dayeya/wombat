global _start
; #[extern(linkage)]
extern putchar
extern putnum
extern quit
extern readnum
extern readchar

section .data

section .text
_start:
	call main
	mov rax, 60 ; emit syscall: exit
	mov rdi, 0 ; exit code of 0 (success)
	syscall

; FUNC main START_IMPL
main:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	
	mov rax, 100
	neg rax
	mov qword [rbp - 8], rax
	
	mov rax, qword [rbp - 8]
	mov rbx, 4
	add rax, rbx
	mov qword [rbp - 16], rax
	
	mov rdi, qword [rbp - 16]
	call putnum
	mov rax, 0
	jmp .end_main

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
