@mul:
	pop |> x
	pop |> y
	%t0 = mul: x, y
	ret %t0

@main:
	#[stack_allocation(8 bytes)]
	alloc g, 8
	g = 6.0
	#[stack_allocation(8 bytes)]
	alloc xs, 8
	%t1 = div: g, 2.0
	%t2 = add: 5.0, %t1
	xs = %t2
