pdp:
	mov	pc,r4
	tst	-(r4)
	sub	$9,r4
	mov	r4,0f
	mov	$1, r0
	sys	4; 0:0; 9
	mov	$1000, r2
1:
	sys	55
	sob	r2, 1b
	br	pdp
