@foo:
	pop |8 bytes|> x
	ret x

@main:
	push 1
	%t0 = call foo, 1
	push %t0
	_ = call putnum, 1

