; AE1B
zFuncAE1B::
	call zFuncCurChunkToRegisters ; AE03
	; DE has zChunkX
	; HL has zChunkY
	push hl ; push HL (chunkY) onto stack
	push de ; push DE (chunkX) onto stack
	
	ld bc, wEnemyMon1Attack ; load D8C8 into BC
	call $ADED ; ADED