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
	sub rsp, 34
	
	; 'iter' allocation of 8 bytes
	mov rax, 0
	mov qword [rbp - 8], rax
	; 'xs' allocation of 8 bytes
	mov rax, 1
	mov qword [rbp - 16], rax
.br_loop_cnt1:
	mov rax, qword [rbp - 8]
	mov rbx, 3
	cmp rax, rbx
	sete al
	movzx rax, al
	mov qword [rbp - 24], rax
	
	mov rax, qword [rbp - 24]
	cmp rax, 0
	je .br_after2
	mov rdi, 45
	call putchar
	mov rdi, 45
	call putchar
	mov rdi, 45
	call putchar
	mov rdi, 10
	call putchar
	jmp .br_loop_brk1
.br_after2:
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
.br_loop_cnt3:
	mov rax, qword [rbp - 16]
	mov rbx, 10
	cmp rax, rbx
	setg al
	movzx rax, al
	mov qword [rbp - 32], rax
	
	mov rax, qword [rbp - 32]
	cmp rax, 0
	je .br_after4
	jmp .br_loop_brk3
.br_after4:
	mov rdi, qword [rbp - 16]
	call putnum
	mov rax, qword [rbp - 16]
	mov rbx, 1
	add rax, rbx
	mov qword [rbp - 40], rax
	
	mov rax, qword [rbp - 40]
	mov qword [rbp - 16], rax
	jmp .br_loop_cnt3
.br_loop_brk3:
	mov rax, qword [rbp - 8]
	mov rbx, 1
	add rax, rbx
	mov qword [rbp - 48], rax
	
	mov rax, qword [rbp - 48]
	mov qword [rbp - 8], rax
	jmp .br_loop_cnt1
.br_loop_brk1:

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
