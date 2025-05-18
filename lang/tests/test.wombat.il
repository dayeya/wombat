@f1:
	ret 1
@main:
	%t0 = bit_not: 4
	%t1 = mul: 3, %t0
	%t2 = neg: 1
	%t3 = add: %t1, %t2
	%t4 = bit_and: 5, %t3
	push %t4
	%none = call putnum, 1
	%t5 = sub: c, b
	push %t5
	%none = call putchar, 1
