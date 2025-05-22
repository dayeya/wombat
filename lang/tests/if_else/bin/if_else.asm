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

; FUNC putbool START_IMPL
putbool:
	push rbp
	mov rbp, rsp
	
	mov byte [rbp - 1], dil
	movzx rax, byte [rbp - 1]
	cmp rax, 0
	je .br_else1
	mov rdi, 116
	call putchar
	mov rdi, 114
	call putchar
	mov rdi, 117
	call putchar
	mov rdi, 101
	call putchar
	jmp .br_end1
.br_else1:
	mov rdi, 102
	call putchar
	mov rdi, 97
	call putchar
	mov rdi, 108
	call putchar
	mov rdi, 115
	call putchar
	mov rdi, 101
	call putchar
.br_end1:
	mov rdi, 10
	call putchar

.end_putbool:
	mov rsp, rbp
	pop rbp
	ret
; FUNC putbool END_IMPL

; FUNC main START_IMPL
main:
	push rbp
	mov rbp, rsp
	sub rsp, 35
	
	; 'MAGIC' allocation of 8 bytes
	mov rax, 69
	mov qword [rbp - 8], rax
	mov rax, qword [rbp - 8]
	mov rbx, 99
	cmp rax, rbx
	setl al
	movzx rax, al
	mov qword [rbp - 16], rax
	
	mov rax, qword [rbp - 16]
	cmp rax, 0
	je .br_else2
	mov rdi, 116
	call putchar
	mov rdi, 117
	call putchar
	mov rdi, 110
	call putchar
	mov rdi, 103
	call putchar
	mov rdi, 10
	call putchar
	jmp .br_end2
.br_else2:
	mov rdi, qword [rbp - 8]
	call putnum
.br_end2:
	; 'xs' allocation of 8 bytes
	mov rax, qword [rbp - 8]
	mov rbx, 2
	; sign-extend rax. 
	cqo
	idiv rbx
	mov qword [rbp - 32], rax
	
	mov rax, qword [rbp - 32]
	mov rbx, 4
	add rax, rbx
	mov qword [rbp - 40], rax
	
	mov rax, qword [rbp - 40]
	mov qword [rbp - 24], rax
	mov rdi, qword [rbp - 24]
	call putnum
	; 'b' allocation of 1 bytes
	mov rax, qword [rbp - 24]
	mov rbx, 38
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 49], rax
	
	mov rax, qword [rbp - 49]
	mov byte [rbp - 41], al
	movzx rdi, byte [rbp - 41]
	call putbool

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
