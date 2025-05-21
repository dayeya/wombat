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

; FUNC putcharln START_IMPL
putcharln:
	push rbp
	mov rbp, rsp
	
	mov byte [rbp - 1], dil
	movzx rdi, byte [rbp - 1]
	call putchar
	mov rdi, 10
	call putchar

.end_putcharln:
	mov rsp, rbp
	pop rbp
	ret
; FUNC putcharln END_IMPL

; FUNC putcharws START_IMPL
putcharws:
	push rbp
	mov rbp, rsp
	
	mov byte [rbp - 1], dil
	movzx rdi, byte [rbp - 1]
	call putchar
	mov rdi, 32
	call putchar

.end_putcharws:
	mov rsp, rbp
	pop rbp
	ret
; FUNC putcharws END_IMPL

; FUNC main START_IMPL
main:
	push rbp
	mov rbp, rsp
	
	mov rdi, 104
	call putchar
	mov rdi, 101
	call putchar
	mov rdi, 108
	call putchar
	mov rdi, 108
	call putchar
	mov rdi, 111
	call putcharws
	mov rdi, 119
	call putchar
	mov rdi, 111
	call putchar
	mov rdi, 114
	call putchar
	mov rdi, 108
	call putchar
	mov rdi, 100
	call putcharln

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
