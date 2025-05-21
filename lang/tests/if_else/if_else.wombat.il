@main:
	%t0 = eq: 1, 1
	jmp_false %t0, .br_else0
	push 1
	_ = call putnum, 1
	jmp .br_end0
.br_else0:
	push 110
	_ = call putchar, 1
	push 9
	_ = call putchar, 1
	push 111
	_ = call putchar, 1
.br_end0:

