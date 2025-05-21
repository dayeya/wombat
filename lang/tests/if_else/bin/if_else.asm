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
	sub rsp, 1
	
	mov rax, 1
	mov rbx, 1
	cmp rax, rbx
	setg al
	movzx rax, al
	mov qword [rbp - 8], rax
	
	mov rax, qword [rbp - 8]
	cmp rax, 0
	je .br_else0
	mov rdi, 2
	call putnum
	jmp .br_end0
.br_else0:
	mov rdi, 110
	call putchar
	mov rdi, 111
	call putchar
	mov rdi, 10
	call putchar
.br_end0:

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
