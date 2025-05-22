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

; FUNC fib START_IMPL
fib:
	push rbp
	mov rbp, rsp
	sub rsp, 67
	
	mov qword [rbp - 8], rdi
	; 'result' allocation of 8 bytes
	mov rax, 0
	mov qword [rbp - 16], rax
	mov rax, qword [rbp - 8]
	mov rbx, 1
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 24], rax
	
	mov rax, qword [rbp - 24]
	cmp rax, 0
	je .br_after1
	mov rax, 1
	mov qword [rbp - 16], rax
.br_after1:
	mov rax, qword [rbp - 8]
	mov rbx, 2
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 32], rax
	
	mov rax, qword [rbp - 32]
	cmp rax, 0
	je .br_else2
	mov rax, 1
	mov qword [rbp - 16], rax
	jmp .br_end2
.br_else2:
	mov rax, qword [rbp - 8]
	mov rbx, 1
	cmp rax, rbx
	setne al
	movzx rax, al
	mov qword [rbp - 40], rax
	
	mov rax, qword [rbp - 40]
	cmp rax, 0
	je .br_after3
	mov rax, qword [rbp - 8]
	mov rbx, 1
	sub rax, rbx
	mov qword [rbp - 48], rax
	
	mov rdi, qword [rbp - 48]
	call fib
	mov qword [rbp - 56], rax
	mov rax, qword [rbp - 8]
	mov rbx, 2
	sub rax, rbx
	mov qword [rbp - 64], rax
	
	mov rdi, qword [rbp - 64]
	call fib
	mov qword [rbp - 72], rax
	mov rax, qword [rbp - 56]
	mov rbx, qword [rbp - 72]
	add rax, rbx
	mov qword [rbp - 80], rax
	
	mov rax, qword [rbp - 80]
	mov qword [rbp - 16], rax
.br_after3:
.br_end2:
	mov rax, qword [rbp - 16]

.end_fib:
	mov rsp, rbp
	pop rbp
	ret
; FUNC fib END_IMPL

; FUNC foo START_IMPL
foo:
	push rbp
	mov rbp, rsp
	sub rsp, 34
	
	; 'iter' allocation of 8 bytes
	mov rax, 0
	mov qword [rbp - 8], rax
	; 'xs' allocation of 8 bytes
	mov rax, 1
	mov qword [rbp - 16], rax
.br_loop_cnt4:
	mov rax, qword [rbp - 8]
	mov rbx, 3
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 24], rax
	
	mov rax, qword [rbp - 24]
	cmp rax, 0
	je .br_after5
	mov rdi, 45
	call putchar
	mov rdi, 45
	call putchar
	mov rdi, 45
	call putchar
	mov rdi, 10
	call putchar
	jmp .br_loop_brk4
.br_after5:
	mov rdi, 45
	call putchar
	mov rdi, 45
	call putchar
	mov rdi, 45
	call putchar
	mov rdi, 10
	call putchar
	mov rax, 1
	mov qword [rbp - 16], rax
.br_loop_cnt6:
	mov rax, qword [rbp - 16]
	mov rbx, 10
	cmp rax, rbx
	setg al
	movzx rax, al
	mov qword [rbp - 32], rax
	
	mov rax, qword [rbp - 32]
	cmp rax, 0
	je .br_after7
	jmp .br_loop_brk6
.br_after7:
	mov rdi, qword [rbp - 16]
	call putnum
	mov rax, qword [rbp - 16]
	mov rbx, 1
	add rax, rbx
	mov qword [rbp - 40], rax
	
	mov rax, qword [rbp - 40]
	mov qword [rbp - 16], rax
	jmp .br_loop_cnt6
.br_loop_brk6:
	mov rax, qword [rbp - 8]
	mov rbx, 1
	add rax, rbx
	mov qword [rbp - 48], rax
	
	mov rax, qword [rbp - 48]
	mov qword [rbp - 8], rax
	jmp .br_loop_cnt4
.br_loop_brk4:

.end_foo:
	mov rsp, rbp
	pop rbp
	ret
; FUNC foo END_IMPL

; FUNC main START_IMPL
main:
	push rbp
	mov rbp, rsp
	
	call foo

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
