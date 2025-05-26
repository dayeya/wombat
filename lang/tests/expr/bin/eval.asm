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
	
	mov rdi, 10
	call putchar

.end_main:
	mov rsp, rbp
	pop rbp
	ret
; FUNC main END_IMPL
