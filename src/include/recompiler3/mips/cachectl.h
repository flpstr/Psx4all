#ifndef __sys_cachectl_h__
#define __sys_cachectl_h__

#include "cache.h"

static const int ICACHE = (1 << 0);
static const int DCACHE = (1 << 1);
#define BCACHE (ICACHE | DCACHE)

static inline int cacheflush(void* addr, const int size, int op)
{
        (void)addr; /* Not used. */
        (void)size; /* Not used. */
        //if (op != DCACHE) _icache_flush_all();
        if (op != ICACHE) _dcache_flush_all();
}

#endif

