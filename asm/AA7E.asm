; AA7E
zFuncAA7E::
	; oh dios mio
	; opening state:
	; chunkX in DE
	; chunkY in HL
	push de ; push DE to stack
	push hl ; push HL to stack
	call zFuncA88D ; A88D
	
	ld hl, zChunkBlockMap ; load 0xD170 into HL (starting address)
	ld bc, $0040 ; load 0x0040 into BC (length)
	ld a, $0F ; load 0x0F (tree block!) into A (fill value)
	call FillMemory
	
	ldh a, [$FF95] ;  load value at FF95 into A
	ld b, a ; load A into B
	bit 0, b ; bittest? yeah
	jr z, .AA99 ; if zero, jump to AA99
	
	ld a, $74 ; load 0x74 into A
	ldh [$FF96], a ; load A into FF96
.AA99
	bit 1,b ; bittest b1
	jr z, .AAA1 ; if zero, jump to AAA1
	
	ld a, $04 ; load 0x04 into A
	ldh [$FF96], a ; load A into FF96
.AAA1
	bit 2,b ; bittest b2
	jr z, .AAA9 ; if zero, jump to AAA9
	
	ld a, $47 ; load 0x47 into A
	ldh [$FF96], a ; load A into FF96
.AAA9
	bit 3,b ; bittest b3
	jr z, .AAB1 ; if zero, jump to AAB1
	
	ld a, $40 ; load 0x40 into A
	ldh [$FF96], a ; load A into FF96

.AAB1
	pop hl
	pop de
	push de
	push hl ; stack go wee? oh no wait they swap spots
	
	ld b, h ;
	ld c, l ; load HL into BC
	call zFuncInitRand ; init rand again?
	
	ldh a, [$FF96] ; load value at FF96 into A
	ld b, a ; load A into B
	ldh a, [$FF95] ; load value at FF95 into A
	ld d, a ; load A into D
	
	ld e, $0A ; load 0x0A into E
	bit 0,d ; bittest d0
	ld c, $74 ; load 0x74 into C
	call nz, $A963
	
	
	