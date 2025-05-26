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

; FUNC put_fizz START_IMPL
put_fizz:
	push rbp
	mov rbp, rsp
	
	mov rdi, 102
	call putchar
	mov rdi, 105
	call putchar
	mov rdi, 122
	call putchar
	mov rdi, 122
	call putchar

.end_put_fizz:
	mov rsp, rbp
	pop rbp
	ret
; FUNC put_fizz END_IMPL

; FUNC put_buzz START_IMPL
put_buzz:
	push rbp
	mov rbp, rsp
	
	mov rdi, 98
	call putchar
	mov rdi, 117
	call putchar
	mov rdi, 122
	call putchar
	mov rdi, 122
	call putchar

.end_put_buzz:
	mov rsp, rbp
	pop rbp
	ret
; FUNC put_buzz END_IMPL

; FUNC process_integer START_IMPL
process_integer:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	
	mov qword [rbp - 8], rdi
	mov rax, qword [rbp - 8]
	mov rbx, 3
	; sign-extend rax. 
	cqo
	idiv rbx
	mov qword [rbp - 16], rdx
	
	mov rax, qword [rbp - 16]
	mov rbx, 0
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 24], rax
	
	mov rax, qword [rbp - 24]
	cmp rax, 0
	je .br_else1
	mov rax, qword [rbp - 8]
	mov rbx, 5
	; sign-extend rax. 
	cqo
	idiv rbx
	mov qword [rbp - 32], rdx
	
	mov rax, qword [rbp - 32]
	mov rbx, 0
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 40], rax
	
	mov rax, qword [rbp - 40]
	cmp rax, 0
	je .br_else2
	call put_fizz
	call put_buzz
	mov rdi, 10
	call putchar
	jmp .br_end2
.br_else2:
	call put_fizz
	mov rdi, 10
	call putchar
.br_end2:
	jmp .br_end1
.br_else1:
	mov rax, qword [rbp - 8]
	mov rbx, 5
	; sign-extend rax. 
	cqo
	idiv rbx
	mov qword [rbp - 48], rdx
	
	mov rax, qword [rbp - 48]
	mov rbx, 0
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 56], rax
	
	mov rax, qword [rbp - 56]
	cmp rax, 0
	je .br_else3
	call put_buzz
	mov rdi, 10
	call putchar
	jmp .br_end3
.br_else3:
	mov rdi, qword [rbp - 8]
	call putnum
.br_end3:
.br_end1:

.end_process_integer:
	mov rsp, rbp
	pop rbp
	ret
; FUNC process_integer END_IMPL

; FUNC main START_IMPL
main:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	; 'i' allocation of 8 bytes
	mov rax, 1
	mov qword [rbp - 8], rax
	; 'LIMIT' allocation of 8 bytes
	call readnum
	mov qword [rbp - 24], rax
	mov rax, qword [rbp - 24]
	mov qword [rbp - 16], rax
.br_loop_cnt4:
	mov rax, qword [rbp - 8]
	mov rbx, qword [rbp - 16]
	cmp rax, rbx
	setg al
	movzx rax, al
	mov qword [rbp - 32], rax
	
	mov rax, qword [rbp - 32]
	cmp rax, 0
	je .br_after5
	jmp .br_loop_brk4
.br_after5:
	mov rdi, qword [rbp - 8]
	call process_integer
	mov rax, qword [rbp - 8]
	mov rbx, 1
	add rax, rbx
	mov qword [rbp - 40], rax
	
	mov rax, qword [rbp - 40]
	mov qword [rbp - 8], rax
	jmp .br_loop_cnt4
.br_loop_brk4:

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
