/**
 * @file
 *
 * @date 15.04.2017
 * @author: Ilya Nozhkin
 */
 
 
	.extern vfork_body
	.text
	
	.global vfork
vfork:
	addi a1, a1, -76
	
	s32i    a0, a1, 0
	s32i    a1, a1, 4
	s32i    a2, a1, 8
	s32i    a3, a1, 12
	s32i    a4, a1, 16
	s32i    a5, a1, 20
	s32i    a6, a1, 24
	s32i    a7, a1, 28
	s32i    a8, a1, 32
	s32i    a9, a1, 36
	s32i    a10, a1, 40
	s32i    a11, a1, 44
	s32i    a12, a1, 48
	s32i    a13, a1, 52
	s32i    a14, a1, 56
	s32i    a15, a1, 60
	
	rsr.sar  a2
	s32i     a2, a1, 64
	rsr.epc1 a2
	s32i     a2, a1, 68

	l32i     a2, a1, 0
	
	
	j vfork_body
