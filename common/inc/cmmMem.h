/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef _CMM_MEM_H
#define _CMM_MEM_H

/** 
 * Function for memory malloc debug
 * @param[in] nBytes The number of bytes to allocate.
 * @param[in] pFileName The file name of the caller.
 * @param[in] nLineNum The line number of the caller.
 * @return The pointer to the block of the allocated memory.
 **/
void* MallocDebug(unsigned nBytes, const char* pFileName, int nLineNum);

/** 
 * Function for memory free debug
 * @param[in] p The pointer to start address of a block of memory to 
 * be freed.
 * @param[in] pFileName The file name of the caller.
 * @param[in] nLineNum The line number of the caller.
 */
void FreeDebug(void* p, const char* pFileName, int nLineNum);

/** Function for memory realloc debug
 * @parma[in] p The pointer to the original memory.
 * @param[in] nBytes The number of bytes to allocate.
 * @param[in] pFileName The file name of the caller.
 * @param[in] nLineNum The line number of the caller.
 */
void* ReallocDebug(void* p, unsigned nBytes, const char* pFileName, int nLineNum);

/**
 * Function to allocate a sized type.
 * @param[in] nBytes The number of bytes to allocate.
 * @return The pointer to the block of the allocated memory.
 */
void* MallocSafe(unsigned int nBytes);

/**
 * Function to reallocate a sized type.
 * @param[in,out] p pointer to  rellocate.
 * @param[in] nBytes The number of bytes to allocate address.
 * @return The pointer to the block of the allocated memory.
 */
void* ReallocSafe(void* p, unsigned int nBytes);

/** 
 * Function to free a memory.
 * @param[in,out] p pointer to  memory that will be freed.
 */
void FreeSafe(void* p);

#ifdef CMM_MEM_DBG
#define Malloc(size) MallocDebug((size), __FILE__, __LINE__)
#define Free(p) FreeDebug(p, __FILE__, __LINE__)
#define Realloc(p, size) ReallocDebug(p, size, __FILE__, __LINE__)
#else
#define Malloc(size) MallocSafe(size)
#define Free(p) FreeSafe(p)
#define Realloc(p, size) ReallocSafe(p, size)
#endif

#define strcpySafe(dst, src, dstSize)       \
    do {                                    \
        if (strlen(src) + 1 > dstSize) {    \
            strncpy(dst, src, dstSize - 1); \
            *(dst + dstSize - 1) = '\0';    \
        } else {                            \
            strcpy(dst, src);               \
        }                                   \
    } while (0)

#define strcatSafe(dst, src, dstSize)                  \
    do {                                               \
        if (strlen(dst) + strlen(src) + 1 > dstSize) { \
            strncat(dst, src, dstSize - strlen(dst));  \
        } else {                                       \
            strcat(dst, src);                          \
        }                                              \
    } while (0)

#define strncpySafe(dst, src, n, dstSize)   \
    do {                                    \
        if (n > dstSize - 1) {              \
            strncpy(dst, src, dstSize - 1); \
        } else {                            \
            strncpy(dst, src, n);           \
        }                                   \
    } while (0)

#define strncatSafe(dst, src, n, dstSize)             \
    do {                                              \
        if (strlen(dst) + n > dstSize - 1) {          \
            strncat(dst, src, dstSize - strlen(dst)); \
        } else {                                      \
            strncat(dst, src, n);                     \
        }                                             \
    } while (0)

#define sprintfSafe(dst, dstSize, fmt) \
    do {                               \
        snprintf(dst, dstSize, fmt);   \
    } while (0)

#define snprintfSafe(dst, dstSize, n, fmt) \
    do {                                   \
        if (n >= dstSize) {                \
            snprintf(dst, dstSize, fmt);   \
        } else {                           \
            snprintf(dst, n, fmt);         \
        }                                  \
    } while (0)

#endif
