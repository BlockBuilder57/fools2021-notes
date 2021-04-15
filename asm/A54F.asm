; A54F
zFuncInitMap::
	xor a ; xor a to itself? this just clears it
	ld [wNumberOfWarps], a ; set wNumberOfWarps to 0
	ld [wNumSprites], a ; set wNumSprites to 0
	ld [wNumSigns], a ; set wNumSigns to 0
	
	ld hl, zBlockMap ; load D800 into HL
	ld bc, $0240 ; load 0240 into BC (length of blockmap)
	
.fillBlockMapWith54
	ld a, $54 ; load 0x54 into A
	ldi [hl], a ; load A into position of HL, increment HL
	dec bc ; decrement bc
	ld a, c ; load C into A
	or b ; or it with B
	jr nz, .fillBlockMapWith54 ; if non-zero, keep looping
	; strange loop but it's quick
	; essentially checks for any value at all in A, will be at 0 when BC is empty
	
	ld a, $19 ; load 0x19 into A
	call Predef ; call predef 0x19, LoadTilesetHeader
	call $1206 ; call 000E+LoadMapHeader.finishUp
	call $AE1B ; custom func, this does the actual blockmap data populating
	
	ld hl, LoadTileBlockMap ; load 0x09FC into HL
	call $DB55 ; custom func, i think everything past this is tilemap stuff
	xor a ; clear a
	ld [wLinkEnemyTrainerName], a ; set wLinkEnemyTrainerName to 0
	call zFuncSetSafeSpawn ; set the spawn
	... ; i don't care about the rest lol