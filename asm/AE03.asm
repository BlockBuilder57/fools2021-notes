; AE03
zFuncCurChunkToRegisters::
	ld hl, zChunkX ; load DAB4 into HL
	ld e, [hl] ; load value of DAB4 (chunkX lo) into E
	inc hl ; increment HL
	ld d, [hl] ; load value of DAB5 (chunkX hi) into D
	inc hl ; increment HL
	; we've loaded zChunkX into DE backwards essentially (endian fixing?)
	
	ldi a, [hl] ; load value of DAB6 (chunkY lo) into A, increment HL
	ld h, [hl] ; load value of DAB7 (chunkY hi) into H
	ld l, a ; load A into L
	
	; ohh okay
	; DE has zChunkX
	; HL has zChunkY
	
	ret ; return