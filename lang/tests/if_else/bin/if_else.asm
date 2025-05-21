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

; FUNC main START_IMPL
main:
	push rbp
	mov rbp, rsp
	sub rsp, 18
	
	; 'x' allocation of 8 bytes
	mov rax, 1
	mov qword [rbp - 8], rax
	mov rax, qword [rbp - 8]
	mov rbx, 1
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 16], rax
	
	mov rax, qword [rbp - 16]
	cmp rax, 0
	je .br_after1
	; 'y' allocation of 8 bytes
	mov rax, 3
	mov qword [rbp - 24], rax
	mov rax, qword [rbp - 24]
	mov rbx, 2
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 32], rax
	
	mov rax, qword [rbp - 32]
	cmp rax, 0
	je .br_else2
	mov rdi, 69
	call putnum
	jmp .br_end2
.br_else2:
	mov rdi, 96
	call putnum
.br_end2:
	mov rdi, 103
	call putchar
	mov rdi, 10
	call putchar
.br_after1:

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
