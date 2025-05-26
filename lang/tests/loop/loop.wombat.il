@GCD:
	pop |b, 8 bytes|
	pop |a, 8 bytes|
.br_loop_cnt1:
	%t0 = eq: b, 0
	jmp_false %t0, .br_after2
	jmp .br_loop_brk1
.br_after2:
	#[stack_allocation(8 bytes)]
	alloc temp, 8
	temp = b
	#[unhandled(mod)]
	b = %t1
	a = temp
	jmp .br_loop_cnt1
.br_loop_brk1:
	#[GCD] ret a

@main:
	#[stack_allocation(8 bytes)]
	alloc gcd, 8
	push 32
	push 48
	%t2 = call GCD, 2
	gcd = %t2
	push gcd
	_ = call putnum, 1
	#[main] ret 0

