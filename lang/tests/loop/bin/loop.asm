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

; FUNC main START_IMPL
main:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	
	; 'x' allocation of 8 bytes
	mov rax, 1
	mov qword [rbp - 8], rax
.br_loop_cnt1:
	mov rax, qword [rbp - 8]
	mov rbx, 5
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 16], rax
	
	mov rax, qword [rbp - 16]
	cmp rax, 0
	je .br_else2
	mov rdi, 104
	call putchar
	mov rdi, 105
	call putchar
	mov rdi, 10
	call putchar
	jmp .br_loop_brk1
	jmp .br_end2
.br_else2:
.br_loop_cnt3:
	mov rax, qword [rbp - 8]
	mov rbx, 5
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 24], rax
	
	mov rax, qword [rbp - 24]
	cmp rax, 0
	je .br_after4
	jmp .br_loop_brk3
.br_after4:
	mov rax, qword [rbp - 8]
	mov rbx, 1
	add rax, rbx
	mov qword [rbp - 32], rax
	
	mov rax, qword [rbp - 32]
	mov qword [rbp - 8], rax
	jmp .br_loop_cnt3
.br_loop_brk3:
.br_end2:
	jmp .br_loop_cnt1
.br_loop_brk1:

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
