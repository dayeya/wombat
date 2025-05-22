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
	sub rsp, 32
	
	; 't' allocation of 1 bytes
	mov rax, 1
	mov byte [rbp - 1], al
	; 'f' allocation of 1 bytes
	mov rax, 0
	mov byte [rbp - 2], al
	movzx rax, byte [rbp - 1]
	movzx rbx, byte [rbp - 1]
	cmp rax, 0
	setne al
	movzx rax, al
	cmp rbx, 0
	setne bl
	movzx rbx, bl
	or rax, rbx
	mov qword [rbp - 10], rax
	
	mov rax, qword [rbp - 10]
	cmp rax, 0
	sete al
	movzx rax, al
	mov qword [rbp - 18], rax
	
	mov rdi, qword [rbp - 18]
	call putbool
	movzx rax, byte [rbp - 2]
	movzx rbx, byte [rbp - 2]
	cmp rax, 0
	setne al
	movzx rax, al
	cmp rbx, 0
	setne bl
	movzx rbx, bl
	or rax, rbx
	mov qword [rbp - 26], rax
	
	mov rax, qword [rbp - 26]
	cmp rax, 0
	sete al
	movzx rax, al
	mov qword [rbp - 34], rax
	
	mov rdi, qword [rbp - 34]
	call putbool
	movzx rax, byte [rbp - 1]
	movzx rbx, byte [rbp - 1]
	cmp rax, 0
	setne al
	movzx rax, al
	cmp rbx, 0
	setne bl
	movzx rbx, bl
	and rax, rbx
	mov qword [rbp - 42], rax
	
	mov rdi, qword [rbp - 42]
	call putbool
	movzx rax, byte [rbp - 1]
	movzx rbx, byte [rbp - 2]
	cmp rax, 0
	setne al
	movzx rax, al
	cmp rbx, 0
	setne bl
	movzx rbx, bl
	and rax, rbx
	mov qword [rbp - 50], rax
	
	mov rdi, qword [rbp - 50]
	call putbool
	movzx rax, byte [rbp - 1]
	movzx rbx, byte [rbp - 2]
	cmp rax, 0
	setne al
	movzx rax, al
	cmp rbx, 0
	setne bl
	movzx rbx, bl
	or rax, rbx
	mov qword [rbp - 58], rax
	
	mov rdi, qword [rbp - 58]
	call putbool
	movzx rax, byte [rbp - 1]
	cmp rax, 0
	sete al
	movzx rax, al
	mov qword [rbp - 66], rax
	
	mov rdi, qword [rbp - 66]
	call putbool
	mov rax, 0
	jmp .end_main

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
