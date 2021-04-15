; DB63
zFuncBankSwitchy::
	ld a, $03 ; load 0x03 into A
	call zFuncLoadSRAMBankA
	ld a, [hl] ; load value at HL to A
	push af ; push AF to stack
	call zFuncLoadSRAMBank02 ; zFuncLoadSRAMBankA but bank 02
	pop af ; pop AF from stack
	ret ;