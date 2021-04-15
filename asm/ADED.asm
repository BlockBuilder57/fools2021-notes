; ADED
zFuncADED::
	push hl ; push HL to stack
	push bc ; push BC to stack
	call zFuncRegistersToCurChunk ; AE0E
	pop bc ; pop BC back
	pop hl ; pop HL back
	
	push bc ; push BC to stack haha there it goes again
	call $AA7E ; AA7E