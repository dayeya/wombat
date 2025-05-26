@put_fizz:
	push 102
	_ = call putchar, 1
	push 105
	_ = call putchar, 1
	push 122
	_ = call putchar, 1
	push 122
	_ = call putchar, 1

@put_buzz:
	push 98
	_ = call putchar, 1
	push 117
	_ = call putchar, 1
	push 122
	_ = call putchar, 1
	push 122
	_ = call putchar, 1

@process_integer:
	pop |i, 8 bytes|
	%t0 = mod: i, 3
	%t1 = eq: %t0, 0
	jmp_false %t1, .br_else1
	%t2 = mod: i, 5
	%t3 = eq: %t2, 0
	jmp_false %t3, .br_else2
	_ = call put_fizz, 0
	_ = call put_buzz, 0
	jmp .br_end2
.br_else2:
	_ = call put_fizz, 0
.br_end2:
	jmp .br_end1
.br_else1:
	%t4 = mod: i, 5
	%t5 = eq: %t4, 0
	jmp_false %t5, .br_else3
	_ = call put_buzz, 0
	jmp .br_end3
.br_else3:
	push i
	_ = call putnum, 1
.br_end3:
.br_end1:

@main:
	#[stack_allocation(8 bytes)]
	alloc i, 8
	i = 1
.br_loop_cnt4:
	%t6 = gt: i, 100
	jmp_false %t6, .br_after5
	jmp .br_loop_brk4
.br_after5:
	push i
	_ = call process_integer, 1
	%t7 = add: i, 1
	i = %t7
	jmp .br_loop_cnt4
.br_loop_brk4:

