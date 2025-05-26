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

; FUNC main START_IMPL
main:
	push rbp
	mov rbp, rsp
	sub rsp, 64
	
	mov rax, 32
	mov rcx, 5
	sar rax, cl
	mov qword [rbp - 8], rax
	
	mov rdi, qword [rbp - 8]
	call putnum
	mov rax, 100
	neg rax
	mov qword [rbp - 16], rax
	
	mov rax, qword [rbp - 16]
	mov rcx, 2
	sar rax, cl
	mov qword [rbp - 24], rax
	
	mov rdi, qword [rbp - 24]
	call putnum
	mov rax, 1
	neg rax
	mov qword [rbp - 32], rax
	
	mov rax, qword [rbp - 32]
	mov rcx, 1
	sar rax, cl
	mov qword [rbp - 40], rax
	
	mov rdi, qword [rbp - 40]
	call putnum
	mov rax, 8
	neg rax
	mov qword [rbp - 48], rax
	
	mov rax, qword [rbp - 48]
	mov rcx, 3
	sar rax, cl
	mov qword [rbp - 56], rax
	
	mov rdi, qword [rbp - 56]
	call putnum

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
