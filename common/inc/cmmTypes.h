/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __CMMTYPES_H
#define __CMMTYPES_H

#ifdef __KERNEL__

#include <linux/string.h>/*memcmp, memcpy, string functions*/
#include <linux/types.h> /*>uint32_t, uint16_t, uint8_t, int8_t, int16_t, int32_t, uint64_t, int64_t*/
#include <linux/vmalloc.h>

#define malloc vmalloc
#define free vfree
#define ASSERT(x) (void(0))
#else

#include <assert.h>
#include <stddef.h> /*>NULL, size_t, offsetof()*/
#include <stdint.h> /*>uint32_t, uint16_t, uint8_t, int8_t, int16_t, int32_t, uint64_t, int64_t*/
#include <stdio.h> /*printf, sprintf*/
#include <stdlib.h> /*malloc, free*/
#include <string.h> /*memcmp, memcpy, string functions*/
#include <unistd.h>

#define ASSERT(expr) /*lint -e(506) */                                                                                 \
    if (!(expr)) {                                                                                                     \
        printf("EXCEPTION:(%s)failed at %s:%i, A segment fault triggered for debugging\n", #expr, __FILE__, __LINE__); \
        (*(uint32_t*)0) = 0;                                                                                           \
        exit(1);                                                                                                       \
    }
#endif

/* Constants ------------------------------------------------------------- */

#ifndef __cplusplus

#ifndef __TYPE_BOOL__
#define __TYPE_BOOL__
typedef enum {
    false = 0,
    true = 1
} CmmBool_t;
typedef CmmBool_t bool;
#endif /*__TYPE_BOOL__*/
#else
#ifndef __TYPE_BOOL__
#define __TYPE_BOOL__
typedef int CmmBool_t;
#endif
#endif /*__cplusplus*/

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifndef MAX_UINT32_VALUE
#define MAX_UINT32_VALUE 0xFFFFFFFF
#endif

#ifndef MAX_UINT64_VALUE
#define MAX_UINT64_VALUE 0xFFFFFFFFFFFFFFFF
#endif

#define STATICINLINE inline

/*Oct String Types*/

typedef struct
    {
    unsigned octCnt;
    uint8_t* data;
} OctString_s;

typedef struct
    {
    unsigned bitCnt;
    uint8_t* data;
} BitString_s;

typedef struct
    {
    unsigned octCnt;
    uint8_t data[28];
} OctArray28_s;

typedef struct
    {
    unsigned bitCnt;
    uint8_t data[8];
} BitArray64_s;

typedef struct
    {
    unsigned bitCnt;
    uint8_t data[32];
} BitArray256_s;

typedef union {
    unsigned u;
    int i;
    void* data;
} CmmCtxt_u;

/**
* Structure to construct a byte stream in a buffer.
*/
typedef struct
    {
    unsigned octCnt; /** Total buffer lenth*/
    unsigned offset; /** Current offset, before the offset, data is valid*/
    uint8_t* data; /** Pointer to a data*/
} CmmBuf_s;

/**Structure to show the PDU transfter between different modules.
* the PDU means...
*
*/
typedef struct
    {
    unsigned octCnt; /** PDU length in Bytes*/
    unsigned pduType; /** PDU Type, used in PDU trace, see the types in CMM Trace*/
    uint8_t* data; /** PDU Bytestream*/
} CmmPdu_s;

#define cpyCmmPdu(tgt, src)                              \
    do {                                                 \
        (tgt)->data = malloc((src)->octCnt);             \
        memcpy((tgt)->data, (src)->data, (src)->octCnt); \
        (tgt)->octCnt = (src)->octCnt;                   \
        (tgt)->pduType = (src)->pduType;                 \
    } while (0)

#define cmpOctStrArray(a, b) \
    ((((a)->octCnt == ((b)->octCnt)) && (0 == memcmp((a)->data, (b)->data, (a)->octCnt))) ? 0 : 1)

#define cmpBitStrArray(a, b) \
    ((((a)->bitCnt == ((b)->bitCnt)) && (0 == memcmp((a)->data, (b)->data, ((a)->bitCnt + 7) / 8))) ? 0 : 1)

#define cpyOctStrArray(tgt, src)                         \
    do {                                                 \
        (tgt)->octCnt = (src)->octCnt;                   \
        memcpy((tgt)->data, (src)->data, (src)->octCnt); \
    } while (0)

#ifndef ALIGN_4BYTE_ADDR
#define ALIGN_4BYTE_ADDR(addr) (((addr + 3) >> 2) << 2)
#endif

#ifndef ALIGN_2BYTE_ADDR
#define ALIGN_2BYTE_ADDR(addr) (((addr + 1) >> 1) << 1)
#endif

#ifndef __BYTE_CONVERT_
#define __BYTE_CONVERT_

/*uint64 -> high 32*/
#define UINT64_TO_UINT32_H(u) ((uint32_t)((u) >> 32))
#define UINT64_TO_UINT32_L(u) ((uint32_t)((u) & 0xffffffff))

#define UINT32_TO_UINT64(s1, s2) (((uint64_t)(s1) << 32) + (s2))

/*Conve*/
#define UINT8_TO_UINT32(s1, s2, s3, s4) (((uint32_t)(s1) << 24) + ((uint32_t)(s2) << 16) + ((uint32_t)(s3) << 8) + (s4))
#define UINT8_TO_UINT24(s1, s2, s3, s4) ((0 << 24) + ((uint32_t)(s1) << 16) + ((uint32_t)(s2) << 8) + (s3))
/*UINT32-->UCHAR*/
#define UINT32_TO_UINT8_HH(u) ((uint8_t)((u) >> 24))
#define UINT32_TO_UINT8_MH(u) ((uint8_t)((u) >> 16))
#define UINT32_TO_UINT8_ML(u) ((uint8_t)((u) >> 8))
#define UINT32_TO_UINT8_LL(u) ((uint8_t)((u) & 0xFF))

#define UINT8_TO_UINT32_MEM(p) UINT8_TO_UINT32(((p)[0]), ((p)[1]), ((p)[2]), ((p)[3]))
#define UINT8_TO_UINT24_MEM(p) UINT8_TO_UINT24(((p)[0]), ((p)[1]), ((p)[2]), ((p)[3]))
#define UINT8_TO_UINT16_MEM(p) UINT8_TO_UINT16(((p)[0]), ((p)[1]))

#define GET_MEM_24(p) UINT8_TO_UINT32((0), ((p)[0]), ((p)[1]), ((p)[2]))

#define UINT32_TO_UINT8_MEM(p, u)    \
    do {                             \
        (p)[0] = ((u) >> 24) & 0xFF; \
        (p)[1] = ((u) >> 16) & 0xFF; \
        (p)[2] = ((u) >> 8) & 0xFF;  \
        (p)[3] = (u) & 0xFF;         \
    } while (0)

#define UINT16_TO_UINT8_MEM(p, u)   \
    do {                            \
        (p)[0] = ((u) >> 8) & 0xFF; \
        (p)[1] = (u) & 0xFF;        \
    } while (0)

/*UCHAR-->USHORT*/
#define UINT8_TO_UINT16(s1, s2) (((uint16_t)(s1) << 8) + (s2))
/*USHORT-->UCHAR*/
#define UINT16_TO_UINT8_H(us) ((uint8_t)((us) >> 8))
#define UINT16_TO_UINT8_L(us) ((uint8_t)((us) & 0xFF))

#endif /*__BYTE_CONVERT_*/
typedef enum {
    /*success state*/
    STATUS_SUCCESS = 0,
    /*failure state*/
    STATUS_FAILURE = 1
} NR_GNB_API_Status_e;

#define DUMMY_ARG __attribute__((unused))

#endif /* __CMMTYPES_H */
/* End of cmmTypes.h */
