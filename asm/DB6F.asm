; assume:
; zPasswordPlayTime     0x01
; zPasswordAchievements 0xFF
; zPasswordGuardians    0x05
; zPasswordRand         0x00
; zPasswordChecksum     0x00

zFuncCalcPassEncrypt::
	ld hl, zPasswordPlayTime ; load DAC0 into hl
	ld c, $04 ; load 0x04 into C (password length - 1)
	ld de, $F9D4 ; load 0xF9D4 into DE

.encryptPassword
	ld a, d ; load D (0xF9) into A
	xor e ; xor A (0xF9) with E (0xD4), store in A, A is now 0x2D
	xor [hl] ; xor A (0x2D) with val at hl (0x01), A is now 0x2C
	ld d, e ; load E (0xD4) into D
	ld e, a ; load A (0x2C) into E
	ldi [hl], a ; load A (0x2C) into value at hl (DAC0), increment hl
	dec c ; decrement C, C is now 0x03
	jr nz, .encryptPassword ; loop back
	
	ld a, $3B ; load 0x3B into A
	ld hl, zPasswordPlayTime ; load DAC0 into hl
	
	add [hl] ; add value at hl (0x2C) to A, A is now 0x67
	inc hl ; increment hl (now 0xDAC1)
	
	xor [hl] ; xor value at hl (0x07) to A, A is now 0x60
	inc hl ; increment hl (now 0xDAC2)
	
	sub [hl] ; subtract value at hl (0x2E) from A, A is now 0x32
	inc hl ; increment hl (now 0xDAC3)
	
	xor [hl] ; xor value at hl (0x29) to A, A is now 0x1B
	inc hl ; increment hl (now 0xDAC4)
	
	ld [hl], a ; load A into hl
	ret