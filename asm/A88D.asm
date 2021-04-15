; A88D
zFuncA88D::
	push de ; push DE to stack
	push hl ; push HL to stack
	
	; chunkX in DE (0001)
	; chunkY in HL (0002)
	
	ld a, e ; load E to A
	and a, $03 ; and A and 0x03
	ld b, a ; load A to B
	; clamps lower byte to 00-03
	; essentially mod 4
	
	ld a, l ; load L to A
	and a, $03 ; and A and 0x03
	ld c, a ; load A to C
	; clamps lower byte to 00-03
	
	; BC now has clamped XY
	; BC = 0102
	
	push bc ; push BC to stack
	
	
	ld b, h ; load H to B
	
	ld a, l ; load L to A
	and a, $FC ; and A and 0xFC
	; divides lower Y by 4 and multiplies by 4 (00, 00, 00, 00, 04, 04, 04, 04, 08...)
	ld c, a ; load A to C
	
	; BC = 0000
	
	ld a, e ; load E to A
	and a, $FC ; and A and 0xFC
	; divides lower X by 4 and multiplies by 4 (00, 00, 00, 00, 04, 04, 04, 04, 08...)
	ld e, a ; load A to E
	
	; E = 00
	
	
	call zFuncInitRand
	call zFuncScrambleRand
	
	and a, $07 ; clamp A to 00-07? (mod 8)
	ld d, $00 ; load 0x00 into d (clear d?)
	swap a ; swap nibbles of A
	ld e, a ; load A into E
	pop bc ; pop BC from stack (getting back our clamped XY)
	ld a, c ; load C into A (load clamped Y into A)
	add a ; double
	add a ; double again
	add b ; add B to A
	ld c, a ; load A into C
	ld b, $00 ; load 0x00 into B
	ld hl, $DD0C ; load 0xDD0C into HL
	add hl, bc ; add BC to HL
	add hl, de ; add DE to HL
	
	call zFuncBankSwitchy ; DB63
	ld c, a ; load A into C
	call zFuncScrambleRand ; A847
	and a, $30 ; do A & 0x30, == 20
	or c ; A | C, store in A
	ldh [hNPCSpriteOffset], a ; load A into hNPCSpriteOffset (FF95)
	
	pop hl ; pop HL from stack
	pop de ; pop DE from stack
	
	ld a, e ; load E into A
	
	or l ; A | L, store in A
	and a, $FC ; and A and 0xFC
	; divides lower Y by 4 and multiplies by 4 (00, 00, 00, 00, 04, 04, 04, 04, 08...)
	or d ; A | D, store in A
	or h ; A | H, store in A
	; i think this would be a way to get big hi byte? not sure
	ret nz ; return if non-zero
	
	ld a, c ; load C into A
	and a, $0F ; A |= 0x0F
	ldh [hNPCSpriteOffset], a ; load A into hNPCSpriteOffset (FF95)
	ret