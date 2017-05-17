/**
 * @file
 *
 * @date Aug 15, 2012
 * @author: Anton Bondarev
 */

#include <stdint.h>
#include <hal/reg.h>
#include "context.h"

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	ctx->ar[0] = (uint32_t) routine_fn;
	ctx->ar[1] = (uint32_t) sp;
	for (int i = 2; i < 16; i++)
		ctx->ar[i] = 0;
	
	SREG_READ("sar", ctx->sar);
	SREG_READ("ps", ctx->ps);
	SREG_READ("epc1", ctx->epc1);
	RSYNC();
}
