; A86F
zFuncInitRand::
	ld hl, zSeed ; load DAB8 to HL
	
	ldi a, [hl] ; load val at HL to A, inc HL
	xor d ; xor A and D, store in A
	ld d, a ; load A into D
	
	ldi a, [hl] ; load val at HL to A, inc HL
	xor e ; xor A and E, store in A
	ld e, a ; load A into E
	
	ldi a, [hl] ; load val at HL to A, inc HL
	xor b ; xor A and B, store in A
	ld b, a ; load A into B
	
	ldi a, [hl] ; load val at HL to A, inc HL
	xor c ; xor A and C, store in A
	
	ld [hl], d ; load D into val at HL (DABC)
	inc hl ; inc HL
	ld [hl], e ; load E into val at HL (DABD)
	inc hl ; inc HL
	ld [hl], b ; load B into val at HL (DABE)
	inc hl ; inc HL
	ld [hl], a ; load A into val at HL (DABF)
	
	ld c, $10 ; load 0x10 into C
	
	; run this loop 16 times?
.A886
	call zFuncScrambleRand
	dec c
	jr nz, .A886
	
	ret ; return