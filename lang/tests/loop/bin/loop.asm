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

; FUNC GCD START_IMPL
GCD:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	
	mov qword [rbp - 8], rdi
	mov qword [rbp - 16], rsi
.br_loop_cnt1:
	mov rax, qword [rbp - 8]
	mov rbx, 0
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 24], rax
	
	mov rax, qword [rbp - 24]
	cmp rax, 0
	je .br_after2
	jmp .br_loop_brk1
.br_after2:
	; 'temp' allocation of 8 bytes
	mov rax, qword [rbp - 8]
	mov qword [rbp - 32], rax
	mov rax, qword [rbp - 16]
	mov rbx, qword [rbp - 8]
	; sign-extend rax. 
	cqo
	idiv rbx
	mov qword [rbp - 40], rdx
	
	mov rax, qword [rbp - 40]
	mov qword [rbp - 8], rax
	mov rax, qword [rbp - 32]
	mov qword [rbp - 16], rax
	jmp .br_loop_cnt1
.br_loop_brk1:
	mov rax, qword [rbp - 16]
	jmp .end_GCD

.end_GCD:
	mov rsp, rbp
	pop rbp
	ret
; FUNC GCD END_IMPL

; FUNC main START_IMPL
main:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	; 'gcd' allocation of 8 bytes
	mov rdi, 32
	mov rsi, 48
	call GCD
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
