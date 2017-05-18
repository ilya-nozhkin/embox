/**
 * @file
 *
 * @date 17.05.2017
 * @author: Ilya Nozhkin
 */
 
.section .iram1.text.vector_base, "x"

	.org 0
	.global exceptions_vector_base
	.long 0
exceptions_vector_base:
	
	.literal_position
	
	.org exceptions_vector_base + 0x10
debug_exception_vector:
	.type debug_exception_vector, @function
	
	rfi 2
	
	.org exceptions_vector_base + 0x20
nmi_exception_vector:
	.type nmi_exception_vector, @function
	
	rfi 3
	
	.org exceptions_vector_base + 0x30
kernel_exception_vector:
	.type kernel_exception_vector, @function
	
	j exception_handler
	
	.org exceptions_vector_base + 0x50
user_exception_vector:
	.type user_exception_vector, @function
	
	j exception_handler
	
	.org exceptions_vector_base + 0x70
double_exception_vector:
	.type double_exception_vector, @function
	
	j exception_handler

exception_handler:
	.type exception_handler, @function
	
	//76 - context
	//12 - parameters for _context_store
	//4 - a0 passed from _context_load
	addi a1, a1, -92
	s32i a0, a1, 0
	s32i a2, a1, 8
	addi a2, a1, 92
	s32i a2, a1, 4
	addi a2, a1, 12
	call0 _context_store
	addi a1, a1, 12
	
	mov a2, a1
	call0 exception_c_handler
	
	mov a2, a1
	call0 _context_load
	l32i a0, a1, 0
	addi a1, a1, 4
	
	rfe
