/**
 * @file
 *
 * @date 15.04.2017
 * @author: Ilya Nozhkin
 */

	.text
	
_context_store:
	.global _context_store
	
	s32i a15, a2, 60
	
	isync
	
	rsr.sar  a15
	s32i     a15, a2, 64
	rsr.ps   a15
	s32i     a15, a2, 68
	rsr.epc1 a15
	s32i     a15, a2, 72
	
	isync
	
	l32i a15, a1, 0
	s32i a15, a2, 0
	
	isync
	
	l32i a15, a1, 4
	s32i a15, a2, 4
	
	isync
	
	l32i a15, a1, 8
	s32i a15, a2, 8
	
	isync
	
	s32i a3,  a2, 12
	s32i a4,  a2, 16
	s32i a5,  a2, 20
	s32i a6,  a2, 24
	s32i a7,  a2, 28
	s32i a8,  a2, 32
	s32i a9,  a2, 36
	s32i a10, a2, 40
	s32i a11, a2, 44
	s32i a12, a2, 48
	s32i a13, a2, 52
	s32i a14, a2, 56
	
	isync
	
	ret
	
_context_load:
	.global _context_load
	
	l32i a14, a2, 56
	l32i a13, a2, 52
	l32i a12, a2, 48
	l32i a11, a2, 44
	l32i a10, a2, 40
	l32i a9,  a2, 36
	l32i a8,  a2, 32
	l32i a7,  a2, 28
	l32i a6,  a2, 24
	l32i a5,  a2, 20
	l32i a4,  a2, 16
	l32i a3,  a2, 12
	
	isync
	
	l32i a1, a2, 4
	addi a1, a1, -4
	
	isync
	
	l32i a15, a2, 0
	s32i a15, a1, 0
	
	isync
	
	l32i     a15, a2, 72
	wsr.epc1 a15
	l32i     a15, a2, 68
	wsr.ps   a15
	l32i     a15, a2, 64
	wsr.sar  a15
	
	isync
	
	l32i a15, a2, 60
	
	isync
	
	ret

context_switch:
	.global context_switch
	
	s32i    a0, a2, 0
	s32i    a1, a2, 4
	s32i    a2, a2, 8
	s32i    a3, a2, 12
	s32i    a4, a2, 16
	s32i    a5, a2, 20
	s32i    a6, a2, 24
	s32i    a7, a2, 28
	s32i    a8, a2, 32
	s32i    a9, a2, 36
	s32i    a10, a2, 40
	s32i    a11, a2, 44
	s32i    a12, a2, 48
	s32i    a13, a2, 52
	s32i    a14, a2, 56
	s32i    a15, a2, 60
	
	rsr.sar  a4
	s32i     a4, a2, 64
	rsr.ps   a4
	s32i     a4, a2, 68
	rsr.epc1 a4
	s32i     a4, a2, 72
	
	isync
	
	l32i     a4, a3, 72
	wsr.epc1 a4
	l32i     a4, a3, 68
	wsr.ps   a4
	l32i     a4, a3, 64
	wsr.sar  a4
	
	l32i    a15, a3, 60
	l32i    a14, a3, 56
	l32i    a13, a3, 52
	l32i    a12, a3, 48
	l32i    a11, a3, 44
	l32i    a10, a3, 40
	l32i    a9, a3, 36
	l32i    a8, a3, 32
	l32i    a7, a3, 28
	l32i    a6, a3, 24
	l32i    a5, a3, 20
	l32i    a4, a3, 16
	l32i    a2, a3, 8
	l32i    a1, a3, 4
	l32i    a0, a3, 0
	
	isync
	
	l32i    a3, a3, 12
	
	isync
	ret
