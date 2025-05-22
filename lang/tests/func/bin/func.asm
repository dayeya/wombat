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

; FUNC fib START_IMPL
fib:
	push rbp
	mov rbp, rsp
	sub rsp, 96
	
	mov qword [rbp - 8], rdi
	mov rax, qword [rbp - 8]
	mov rbx, 0
	cmp rax, rbx
	setl al
	movzx rax, al
	mov qword [rbp - 16], rax
	
	mov rax, qword [rbp - 16]
	cmp rax, 0
	je .br_after1
	mov rdi, 110
	call putchar
	mov rdi, 111
	call putchar
	mov rdi, 112
	call putchar
	mov rdi, 10
	call putchar
	mov rax, 0
	jmp .end_fib
.br_after1:
	mov rax, qword [rbp - 8]
	mov rbx, 0
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 24], rax
	
	mov rax, qword [rbp - 24]
	cmp rax, 0
	je .br_after2
	mov rax, 0
	jmp .end_fib
.br_after2:
	mov rax, qword [rbp - 8]
	mov rbx, 2
	cmp rax, rbx
	setle al
	movzx rax, al
	mov qword [rbp - 32], rax
	
	mov rax, qword [rbp - 32]
	cmp rax, 0
	je .br_after3
	mov rax, 1
	jmp .end_fib
.br_after3:
	mov rax, qword [rbp - 8]
	mov rbx, 1
	sub rax, rbx
	mov qword [rbp - 40], rax
	
	mov rdi, qword [rbp - 40]
	call fib
	mov qword [rbp - 48], rax
	mov rax, qword [rbp - 8]
	mov rbx, 2
	sub rax, rbx
	mov qword [rbp - 56], rax
	
	mov rdi, qword [rbp - 56]
	call fib
	mov qword [rbp - 64], rax
	mov rax, qword [rbp - 48]
	mov rbx, qword [rbp - 64]
	add rax, rbx
	mov qword [rbp - 72], rax
	
	mov rax, qword [rbp - 72]
	jmp .end_fib

.end_fib:
	mov rsp, rbp
	pop rbp
	ret
; FUNC fib END_IMPL

; FUNC main START_IMPL
main:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	
	mov rdi, 6
	call fib
	mov qword [rbp - 8], rax
	mov rdi, qword [rbp - 8]
	call putnum

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
