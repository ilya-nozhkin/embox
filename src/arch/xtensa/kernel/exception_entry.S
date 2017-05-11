.global savedRegs

	.text
.literal_position

	.text
	.align	4

	.global save_extra_sfrs_for_exception
	.align 4
//The Xtensa OS HAL does not save all the special function register things. This bit of assembly
//fills the savedRegs struct with them.
save_extra_sfrs_for_exception:
	movi	a2, savedRegs
	rsr		a3, 5
	s32i	a3, a2, 0x4C
	rsr		a3, 176
	s32i	a3, a2, 0x50
	rsr		a3, 208
	s32i	a3, a2, 0x54
	rsr		a3, EXCCAUSE
	s32i	a3, a2, 0x5C
	rsr		a3, EXCVADDR
	s32i	a3, a2, 0x60
	ret
