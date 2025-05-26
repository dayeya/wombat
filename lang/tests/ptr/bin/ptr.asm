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

; FUNC swap START_IMPL
swap:
	push rbp
	mov rbp, rsp
	sub rsp, 80
	
	mov qword [rbp - 8], rdi
	mov qword [rbp - 16], rsi
	mov rax, qword [rbp - 16]
	mov rax, qword [rax]
	mov qword [rbp - 24], rax
	
	mov rax, qword [rbp - 8]
	mov rax, qword [rax]
	mov qword [rbp - 32], rax
	
	mov rax, qword [rbp - 24]
	mov rbx, qword [rbp - 32]
	xor rax, rbx
	mov qword [rbp - 40], rax
	
	mov rax, qword [rbp - 40]
	mov rbx, qword [rbp - 16]
	mov qword [rbx], rax
	mov rax, qword [rbp - 16]
	mov rax, qword [rax]
	mov qword [rbp - 48], rax
	
	mov rax, qword [rbp - 8]
	mov rax, qword [rax]
	mov qword [rbp - 56], rax
	
	mov rax, qword [rbp - 48]
	mov rbx, qword [rbp - 56]
	xor rax, rbx
	mov qword [rbp - 64], rax
	
	mov rax, qword [rbp - 64]
	mov rbx, qword [rbp - 8]
	mov qword [rbx], rax
	mov rax, qword [rbp - 8]
	mov rax, qword [rax]
	mov qword [rbp - 72], rax
	
	mov rax, qword [rbp - 16]
	mov rax, qword [rax]
	mov qword [rbp - 80], rax
	
	mov rax, qword [rbp - 72]
	mov rbx, qword [rbp - 80]
	xor rax, rbx
	mov qword [rbp - 88], rax
	
	mov rax, qword [rbp - 88]
	mov rbx, qword [rbp - 16]
	mov qword [rbx], rax

.end_swap:
	mov rsp, rbp
	pop rbp
	ret
; FUNC swap END_IMPL

; FUNC main START_IMPL
main:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	
	; 'xs' allocation of 8 bytes
	mov rax, 42
	mov qword [rbp - 8], rax
	; 'ys' allocation of 8 bytes
	mov rax, 84
	mov qword [rbp - 16], rax
	mov rdi, qword [rbp - 8]
	call putnum
	mov rdi, qword [rbp - 16]
	call putnum
	mov rdi, 10
	call putchar
	lea rdi, [rbp - 16]
	lea rsi, [rbp - 8]
	call swap
	mov rdi, qword [rbp - 8]
	call putnum
	mov rdi, qword [rbp - 16]
	call putnum
	; 'p' allocation of 8 bytes
	lea rax, [rbp - 8]
	mov qword [rbp - 24], rax
	mov rax, 3
	mov rbx, qword [rbp - 24]
	mov qword [rbx], rax
	mov rdi, qword [rbp - 8]
	call putnum
	mov rax, 0
	jmp .end_main

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
