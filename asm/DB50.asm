; DB50
zFuncLoadSRAMBank02::
	ld a, [$DB4C] ; load val at 0xDB4C into A
	jr $DB3C ; jump to DB3C (does busy work? seems to lead into zFuncLoadSRAMBankA)
	; due to the way this is set up it functions as loading SRAM bank 2
	; the rest is zFuncLoadSRAMBankA, which has a return