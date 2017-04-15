/**
 * @file
 *
 * @date Aug 15, 2012
 * @author: Anton Bondarev
 */

#include <stdint.h>
#include "context.h"

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	//ctx->sp = (uint32_t) sp - sizeof(struct context) - 4;
	//ctx->ra = (uint32_t) routine_fn;
	//ctx->c0_stat = 0;
	ctx->ar[0] = (uint32_t) routine_fn;
	ctx->ar[1] = (uint32_t) sp;
	for (int i = 2; i < 16; i++)
		ctx->ar[i] = 0;
}
