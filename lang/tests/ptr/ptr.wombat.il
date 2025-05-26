@main:
	#[stack_allocation(8 bytes)]
	alloc xs, 8
	xs = 84
	#[stack_allocation(8 bytes)]
	alloc p, 8
	p = &xs
	#[stack_allocation(8 bytes)]
	alloc inner, 8
	%t0 = deref p
	inner = %t0
	push inner
	_ = call putnum, 1

