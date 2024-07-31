/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef DPDK_SUPPORT
#include <rte_debug.h>
#include <rte_malloc.h>
#endif

#include "cmmTypes.h"
#define CMM_CACHE_LINE_SIZE 64

#define _MEM_SELFTEST_
#ifdef _MEM_SELFTEST_
#define MEM_TRACE printf
#else
#include "cmmTrace.h"
#define MEM_TRACE TraceMemAct
#endif

/* 
 * Function for memory malloc debug
 */
void* MallocDebug(unsigned nBytes, const char* pFileName, int nLineNum)
{
    void* pNew;
#ifdef DPDK_SUPPORT
    pNew = rte_zmalloc(NULL, nBytes, CMM_CACHE_LINE_SIZE);
#else
    pNew = malloc(nBytes);
#endif

    if (pNew == NULL) {
        return NULL;
    }

    MEM_TRACE("[file %s][line %d] malloc mem[%p],size[%d]\n", pFileName, nLineNum, pNew, nBytes);

    return pNew;
}

/*
 *  function for memory free debug
 * 
 */
void FreeDebug(void* p, const char* pFileName, int nLineNum)
{
    if (NULL == p) {
        printf("error@@@@@ FreeDebug p =NULL, filename=%s,linenumber=%d!\n",
               pFileName, nLineNum);
        return;
    }

    MEM_TRACE("[%s][line %d] free mem[%p]\n", pFileName, nLineNum, p);

#ifdef DPDK_SUPPORT
    rte_free(p);
#else

    free(p);
#endif

    return;
}

/* 
 *  function for memory realloc debug
 */
void* ReallocDebug(void* p, unsigned nBytes, const char* pFileName, int nLineNum)
{
    void* pNew;
    void* pReal;

    if (NULL == p) {
        printf("error@@@@@ ReallocDebug p =NULL, filename=%s,linenumber=%d!\n",
               pFileName, nLineNum);
    }

    pNew = NULL;
    pReal = p;
#ifdef DPDK_SUPPORT
    pNew = rte_realloc(pReal, nBytes, CMM_CACHE_LINE_SIZE);
#else
    pNew = realloc(pReal, nBytes);
#endif
    /* if call realloc failed, return NULL */
    if (pNew == NULL) {
        return NULL;
    }

    MEM_TRACE("[%s][line %d] realloc newp[%p],size[%d],oldp[%p]\n", pFileName, nLineNum, pNew, nBytes, p);

    return pNew;
}

/*
 * Function to allocate a sized type.
 */
void* MallocSafe(unsigned int nBytes)
{
    assert(nBytes);
    void* p = NULL;
#ifdef DPDK_SUPPORT
    p = rte_zmalloc(NULL, nBytes, CMM_CACHE_LINE_SIZE);
#else
    p = malloc(nBytes);
#endif
    if (p == NULL) {
        return NULL;
    }
    memset(p, 0, nBytes);
    return p;
}

/*
 * Function to reallocate a sized type.
 */
void* ReallocSafe(void* p, unsigned int nBytes)
{
    void* pNew;
    assert(nBytes);
    assert(p);

    pNew = realloc(p, nBytes);
    if (NULL == pNew) {
    }
    return pNew;
}

/*lint -save -e438 */
/*
 * Function to free a memory.
 */
void FreeSafe(void* p)
{
    if (p == NULL) {
        return;
    }
#ifdef DPDK_SUPPORT
    rte_free(p);
#else

    free(p);
#endif
    /* set pointer to NULL to avoid double free */
    p = NULL;
    return;
}

/*lint -restore */
