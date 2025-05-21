@main:
	#[stack_allocation(8 bytes)]
	alloc xs, 8
	xs = 1
.br_loop_cnt0:
	%t0 = gt: xs, 10
	jmp_false %t0, .br_after1
	jmp .br_loop_brk0
.br_after1:
	push xs
	_ = call putnum, 1
	%t1 = add: xs, 1
	xs = %t1
	jmp .br_loop_cnt0
.br_loop_brk0:

