; AE0E
zFuncRegistersToCurChunk::
	ld b, h ; load H into B
	ld c, l ; load L into C
	; essentially copies HL to BC
	
	ld hl, zChunkX ; load DAB4 to HL
	ld [hl], e ; load E into DAB4
	inc hl; increment HL
	ld [hl], d ; load D into DAB5
	inc hl;
	ld [hl], c ; load C into DAB6
	inc hl;
	ld [hl], b ; load B into DAB7
	
	; DAB4-DAB7:
	; zChunkX: ED
	; zChunkY: CB
	
	ret ; return