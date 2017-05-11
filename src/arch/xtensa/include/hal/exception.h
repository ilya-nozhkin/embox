/**
 * @file exception.h
 * @brief Different causes of exception
 *
 * @author  Egor Shitov
 * @date    11.05.2017
 */

#ifndef HAL_EXCEPTION_H_
#define HAL_EXCEPTION_H_

#define EXCCAUSE_ILLEGAL		0	/* Illegal Instruction */
#define EXCCAUSE_SYSCALL		1	/* System Call (SYSCALL instruction) */
#define EXCCAUSE_INSTR_ERROR		2	/* Instruction Fetch Error */
# define EXCCAUSE_IFETCHERROR		2	/* (backward compatibility macro, deprecated, avoid) */
#define EXCCAUSE_LOAD_STORE_ERROR	3	/* Load Store Error */
# define EXCCAUSE_LOADSTOREERROR	3	/* (backward compatibility macro, deprecated, avoid) */
#define EXCCAUSE_LEVEL1_INTERRUPT	4	/* Level 1 Interrupt */
# define EXCCAUSE_LEVEL1INTERRUPT	4	/* (backward compatibility macro, deprecated, avoid) */
#define EXCCAUSE_ALLOCA			5	/* Stack Extension Assist (MOVSP instruction) for alloca */
#define EXCCAUSE_DIVIDE_BY_ZERO		6	/* Integer Divide by Zero */
#define EXCCAUSE_SPECULATION		7	/* Use of Failed Speculative Access (not implemented) */
#define EXCCAUSE_PRIVILEGED		8	/* Privileged Instruction */
#define EXCCAUSE_UNALIGNED		9	/* Unaligned Load or Store */
/* Reserved				10..11 */
#define EXCCAUSE_INSTR_DATA_ERROR	12	/* PIF Data Error on Instruction Fetch (RB-200x and later) */
#define EXCCAUSE_LOAD_STORE_DATA_ERROR	13	/* PIF Data Error on Load or Store (RB-200x and later) */
#define EXCCAUSE_INSTR_ADDR_ERROR	14	/* PIF Address Error on Instruction Fetch (RB-200x and later) */
#define EXCCAUSE_LOAD_STORE_ADDR_ERROR	15	/* PIF Address Error on Load or Store (RB-200x and later) */
#define EXCCAUSE_ITLB_MISS		16	/* ITLB Miss (no ITLB entry matches, hw refill also missed) */
#define EXCCAUSE_ITLB_MULTIHIT		17	/* ITLB Multihit (multiple ITLB entries match) */
#define EXCCAUSE_INSTR_RING		18	/* Ring Privilege Violation on Instruction Fetch */
/* Reserved				19 */	/* Size Restriction on IFetch (not implemented) */
#define EXCCAUSE_INSTR_PROHIBITED	20	/* Cache Attribute does not allow Instruction Fetch */
/* Reserved				21..23 */
#define EXCCAUSE_DTLB_MISS		24	/* DTLB Miss (no DTLB entry matches, hw refill also missed) */
#define EXCCAUSE_DTLB_MULTIHIT		25	/* DTLB Multihit (multiple DTLB entries match) */
#define EXCCAUSE_LOAD_STORE_RING	26	/* Ring Privilege Violation on Load or Store */
/* Reserved				27 */	/* Size Restriction on Load/Store (not implemented) */
#define EXCCAUSE_LOAD_PROHIBITED	28	/* Cache Attribute does not allow Load */
#define EXCCAUSE_STORE_PROHIBITED	29	/* Cache Attribute does not allow Store */

void exception_init(void);

#endif /* HAL_EXCEPTION_H_ */
