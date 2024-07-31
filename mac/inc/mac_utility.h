/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef _MAC_UTILITY_H
#define _UTILITY_H

#include <stdio.h>
#include <sys/types.h>

typedef unsigned long long cycles_t;

#if defined(__i386__)
static __inline__ unsigned long long currentcycles(void)
{
    unsigned long long int x;
    __asm__ volatile(".byte 0x0f, 0x31"
                     : "=A"(x));
    return x;
}

#elif defined(__x86_64__)

static __inline__ unsigned long long currentcycles(void)
{
    unsigned hi, lo;
    __asm__ __volatile__("rdtsc"
                         : "=a"(lo), "=d"(hi));
    return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}
#else
static __inline__ unsigned long long currentcycles(void)
{

    unsigned long long val;

    asm volatile("mrs %0, cntvct_el0"
                 : "=r"(val));

    return val;
}

#endif
#endif
