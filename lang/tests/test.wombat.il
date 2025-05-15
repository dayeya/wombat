@main:
	%t0 = bitwise(not): 4
	%t1 = mul: 3, %t0
	%t2 = neg: 1
	%t3 = add: %t1, %t2
	%t4 = bitwise(and): 5, %t3
	push %t4
	call putnum, 1
