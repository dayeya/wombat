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
	sub rsp, 16
	
	; 'xs' allocation of 1 bytes
	mov rax, 97
	mov byte [rbp - 1], al
	; 'p' allocation of 8 bytes
	lea rax, [rbp - 1]
	mov qword [rbp - 9], rax
	; 'inner' allocation of 1 bytes
	mov rax, qword [rbp - 9]
	mov rax, qword [rax]
	mov qword [rbp - 18], rax
	
	mov rax, qword [rbp - 18]
	mov byte [rbp - 10], al
	movzx rdi, byte [rbp - 10]
	call putchar

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
