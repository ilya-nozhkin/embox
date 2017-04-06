/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    06.04.2017
 */

#include <asm/setjmp.h>


.global setjmp
setjmp:
	s32i.n	a0, a2, 0
	s32i.n	a1, a2, 4
	s32i.n	a12, a2, 8
	s32i.n	a13, a2, 12
	s32i.n	a14, a2, 16
	s32i.n	a15, a2, 20
	movi.n	a2, 0
	ret.n

.global longjmp
longjmp:
	l32i.n	a0, a2, 0
	l32i.n	a12, a2, 8
	l32i.n	a13, a2, 12
	l32i.n	a14, a2, 16
	l32i.n	a15, a2, 20
	l32i.n	a1, a2, 4
	movi.n	a2, 1
	movnez	a2, a3, a3
	ret.n
