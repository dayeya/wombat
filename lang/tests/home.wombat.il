@main:
	#[stack_allocation(8 bytes)]
	alloc num, 8
	num = 8
	push num
	_ = call putnum, 1