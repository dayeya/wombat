@main:
	#[stack_allocation(8 bytes)]
	alloc xs, 8
	xs = 42
	#[stack_allocation(8 bytes)]
	alloc p, 8
	p = &xs
	push xs
	_ = call putnum, 1
	store(p) = 84
	push xs
	_ = call putnum, 1
	#[main] ret 0

