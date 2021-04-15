; A5BE
zFuncSetSafeSpawn::
	ld hl, wEnemyMon1Attack ; load D8C8 into HL (top left tile of current chunk!)
	ld bc, NULL ; clear BC

; A5C4
.horizontalLoop
	ldi a, [hl] ; load val at D8C8 into A, inc HL
	cp a, $0A ; do A - 0x0A
	jr z, .foundSafe ; if 0 (ie if A is 0x0A), we found the safe one
	
	inc c ; increment c
	ld a, c ; load C into A
	cp a, $08 ; do A - 0x08
	jr nz, .horizontalLoop ; if non-zero, loop back
	
	ld de, $0010 ; load 0010 into DE
	add hl, de ; add DE to HL ()
	inc b ; increment B
	ld c, $00 ; load 0x00 into C
	ld a, b ; load B into A
	cp a, $08 ; do A - 0x08
	jr nz, .horizontalLoop ; if non-zero, loop back
	ld b, b ; wat, is this load B into A?
	ld bc, $0404 ; fall back to 04, 04 as a ultimate fuckywucky solution
	
; A5DF
.foundSafe
	ld a, c ; load C (x-pos) into A
	add a ; double the position! (we were in block space, now we're in tile space)
	add a, $10 ; add 16 to a, so we stay in the "middle" of the fake map
	ld [wXCoord], a ; write to wXCoord
	ld a, b ; load D (y-pos) into A
	add a ; double the position
	add a, $10 ; add 16 to a
	ld [wYCoord], a ; write to wYCoord
	ret ; return