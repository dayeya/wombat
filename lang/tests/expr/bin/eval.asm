global _start
; #[extern(linkage)]
extern putchar
extern putnum
extern quit
extern assert

section .data

section .text
_start:
	call main
	mov rax, 60 ; emit syscall: exit
	mov rdi, 0 ; exit code of 0 (success)
	syscall

; FUNC evaluate START_IMPL
evaluate:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	mov qword [rbp - 8], rdi
	mov qword [rbp - 16], rsi
	mov qword [rbp - 24], rdx
	mov rax, qword [rbp - 16]
	mov rbx, 3
	add rax, rbx
	mov qword [rbp - 32], rax
	
	mov rax, qword [rbp - 24]
	mov rbx, qword [rbp - 32]
	xor rdx, rdx
	imul rbx
	mov qword [rbp - 40], rax
	
	mov rax, qword [rbp - 8]
	mov rbx, 1
	sub rax, rbx
	mov qword [rbp - 48], rax
	
	mov rax, qword [rbp - 40]
	mov rbx, qword [rbp - 48]
	; sign-extend rax. 
	cqo
	idiv rbx
	mov qword [rbp - 56], rax
	
	mov rax, qword [rbp - 56]
	jmp .end_evaluate

.end_evaluate:
	mov rsp, rbp
	pop rbp
	ret
; FUNC evaluate END_IMPL

; FUNC main START_IMPL
main:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	; 'r' allocation of 8 bytes
	mov rdi, 3
	mov rsi, 2
	mov rdx, 4
	call evaluate
	mov qword [rbp - 16], rax
	mov rax, qword [rbp - 16]
	mov qword [rbp - 8], rax
	mov rdi, qword [rbp - 8]
	call putnum
	mov rax, 0
	jmp .end_main

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
