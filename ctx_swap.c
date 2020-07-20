#include <string.h>
#include "ctx_swap.h"

static void routineFunc(stroutine *co)
{
	if (co->pfn)
	{
		co->pfn(co->arg);
	}
}

int ctx_make(coctx_t *ctx, pfn_co_routine_t pfn)
{
	char *sp = ctx->ss_sp + ctx->ss_size;
	sp = (char *)((unsigned long)sp & -16LL);
	memset(ctx->regs, 0x0, sizeof(ctx->regs));
	void **ret_addr = (void **)(sp);
	*ret_addr = (void *)pfn;
	ctx->regs[kRSP] = sp;
	ctx->regs[kRETAddr] = (char *)pfn;
	return 0;
}


stroutine *co_init(pfn_co_routine_t pfn, int stack_size)
{
	stroutine *pc = (stroutine *)malloc(sizeof(stroutine));
	memset(pc, 0, sizeof(stroutine));
	pc->pfn = pfn;
	pc->ctx.ss_size = stack_size;
	pc->ctx.ss_sp = (char *)malloc(stack_size);
	ctx_make(&(pc->ctx), pfn);
	return pc;
}

extern "C"
{
	extern void coctx_swap( coctx_t *,coctx_t* ) asm("coctx_swap");
};

void co_resume(stroutine *cur, stroutine *next)
{
	coctx_swap(&(cur->ctx), &(next->ctx));
}
