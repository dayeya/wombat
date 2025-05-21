@main:
	#[stack_allocation(8 bytes)]
	alloc MAGIC, 8
	MAGIC = 69
	%t104366696400704 = lt: MAGIC, 99
	jmp_false %t104366696400704, .br_else104366696400705
	push 116
	_ = call putchar, 1
	push 117
	_ = call putchar, 1
	push 110
	_ = call putchar, 1
	push 103
	_ = call putchar, 1
	push 10
	_ = call putchar, 1
	jmp .br_end104366696400705
.br_else104366696400705:
	push MAGIC
	_ = call putnum, 1
.br_end104366696400705:
	#[stack_allocation(8 bytes)]
	alloc xs, 8
	%t104366696400705 = div: MAGIC, 2
	%t104366696400706 = add: %t104366696400705, 4
	xs = %t104366696400706
	push xs
	_ = call putnum, 1

