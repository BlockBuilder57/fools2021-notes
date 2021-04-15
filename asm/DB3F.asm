; DB3F
zFuncLoadSRAMBankA::
	ld [$4000], a ; load A into value at LavenderMart_Blocks ??
	; this for MBC1 this functions as a RAM bank switch
	; in this case, we write A to 0x4000 to switch to that specific bank (so long as A is 00-03)
	; https://gbdev.io/pandocs/#_4000-5fff-ram-bank-number-or-rtc-register-select-write-only
	ld a, $0A ; load 0x0A into A
	ld [$0000], a
	; "a value of 0Ah will enable reading and writing to external RAM - and to the RTC Registers! A value of 00h will disable either"
	ret ; return