; A847
zFuncScrambleRand::
	; please stop with the functions
	ld a, [zRandMaybe] ; load val at DABC into A
	inc a ; increment A
	ld [zRandMaybe], a ; put it back
	
	ld b, a ; load A into B
	ld a, [$DABF] ; load val at DABF into A
	xor b ; xor A with B
	
	ld b, a ; load A into B
	ld a, [$DABD] ; load val at DABD into A
	xor b ; xor A with B
	ld [$DABD], a ; put result back
	
	ld b, a ; load A into B
	ld a, [$DABE] ; load val at DABE into A
	add b ; add A to B
	ld [$DABE], a ; put result back
	
	ret ; return
	