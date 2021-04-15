zFuncCalcPass:: ; B773
	ld a, [wPlayTimeHours] ; pretend wPlayTimeHours 0x05
	and a ; still 05, though f goes from C0 to 20?
	; f is flags (zero, carry, etc) so it can be ignored
	jr z, .wasDoneQuick ; if a is zero (ie 0 hrs) it was done quick so we don't need to find the rest of the time
	ld b, a ; load a into b (b is now wPlayTimeHours)
	ld c, $3C ; load 3C into c (c is now 3C)
	; 0x3C = 60 (mins)
	ld a, [wPlayTimeMinutes] ; load wPlayTimeMinutes into a
	
.add60mins
	add c ; add value of c to a (add 60min to a)
	jr c, .clampPlayTime ; if carry is set, (ie if a overflows) jump to clampPlayTime
	dec b ; decrement b, subtract flag (n) is always set
	jr nz, .add60mins ; if b is non-zero, loop back
	jr .B78E ; else, go to .B78E

.wasDoneQuick
	ld a, [wPlayTimeMinutes] ; load wPlayTimeMinutes into a
	jr .B78E ; jump to .B78E

.clampPlayTime
	ld a, $FF ; load 0xFF into a
	; essentially clamps wPlayTimeMinutes
	
.B78E
	ld e, $00 ; load 0x00 into e
	ld [zPasswordPlayTime], a ; load a into zPasswordPlayTime
	; if wPlayTimeHours == 0, this will be wPlayTimeMinutes
	; else, this will be min((wPlayTimeMinutes + (wPlayTimeHours * 60)), 255) i think
	
	ld hl, $DACF ; load $DACF into hl
	ldi a, [hl] ; load val at hl (0xDACF) into a, inc hl
	; a is now 0, hl is now DAD0
	and a, $FE ; and A with 0xFE, is 0
	cp a, $FE ; compare A to 0xFE (A - 0xFE)
	; N is set due to subtraction, H is set due to no bit 4 borrow, C is set because 0 < 0xFE
	jr nz, .checkMonHealth ; if A-0xFE != 0, jump to .checkMonHealth
	
	ldi a, [hl] ; load val at hl (0xDAD0) into a, inc hl
	cp a, $FF ; A - 0xFF
	jr nz, .checkMonHealth ; if A-0xFF != 0, jump to .checkMonHealth
	
	ldi a, [hl] ; load val at hl (0xDAD1) into a, inc hl
	and a, $1F ; A & 0x1F, is 0
	cp a, $1F ; A - 0x1F
	jr nz, .checkMonHealth ; if A-0x1F != 0, jump to .checkMonHealth
	set 0, e ; set bit0 of e

.checkMonHealth
	ld a, [zMonMaxHP] ; load pika's max HP into A
	cp a, $64 ; maxHP - 100
	jr c, .checkDaredevil ; if we needed to carry, we didn't get the achievement (fail)
	set 1, e ; set bit1 of e
	
.checkDaredevil
	ld hl, zGameState ; load DB38 into hl
	bit 7, [hl] ; test bit 7 of 0xDB38
	jr z, .B7BD ; if bit 7 is 0, jump to .B7BD
	set 2, e ; set bit2 of e

.B7BD
	res 3, e ; reset (clear) bit3 of e
	
	ld a, e ; load value of e into a
	ld [zPasswordAchievements], a ; load a into zPasswordAchievements
	ld a, [zGuardiansDefeated] ; load value fo 0xDB39 into a
	ld [zPasswordGuardians], a ; load a into zPasswordDB39
	call Random
	ld [zPasswordRand], a ; load a into zPasswordRand
	call zFuncCalcPassEncrypt
	ret