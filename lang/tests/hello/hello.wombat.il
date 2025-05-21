@putcharln:
	pop @local(out) < |1 bytes|
	push out
	_ = call putchar, 1
	push 10
	_ = call putchar, 1

@putcharws:
	pop @local(out) < |1 bytes|
	push out
	_ = call putchar, 1
	push 32
	_ = call putchar, 1

@main:
	push 104
	_ = call putchar, 1
	push 101
	_ = call putchar, 1
	push 108
	_ = call putchar, 1
	push 108
	_ = call putchar, 1
	push 111
	_ = call putcharws, 1
	push 119
	_ = call putchar, 1
	push 111
	_ = call putchar, 1
	push 114
	_ = call putchar, 1
	push 108
	_ = call putchar, 1
	push 100
	_ = call putcharln, 1

