/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef _CMM_ZLOG_H
#define _CMM_ZLOG_H
#include "zlog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef enum {
    LOG_CMM,
    LOG_APP,
    LOG_MAC,
    LOG_MAX
} LOGCat_e;

typedef struct {
    LOGCat_e type;
    char catName[64];
    zlog_category_t* zc;
} logCatInfo_s;

extern logCatInfo_s sLogCatInfo[LOG_MAX + 1];

#define ZLOG_ERROR zlog_error
#define ZLOG_DEBUG zlog_debug
#define ZLOG_INFO zlog_info
#define ZLOG_NOTICE zlog_notice
#define ZLOG_WARN zlog_warn
#define ZLOG_TRACE(cat, format, args...)                                                \
    zlog(cat, __FILE__, sizeof(__FILE__) - 1, __func__, sizeof(__func__) - 1, __LINE__, \
         ZLOG_LEVEL_TRACE, format, ##args)

#ifdef _DISABLE_LOG_DEBUG

#define CMM_DEBUG(catId, format, args...)
#define CMM_TRACE(catId, format, args...)
#define CMM_INFO(catId, format, args...)

#define CMM_NOTICE(catId, format, args...)                   \
                                                             \
    do {                                                     \
        char FORMAT[256];                                    \
        sprintf(FORMAT, "\033[;34m" "%s" "\033[0m", format); \
        ZLOG_NOTICE(sLogCatInfo[catId].zc, FORMAT, ##args);  \
    } while (0)

#else
#define CMM_DEBUG(catId, format, args...)                    \
    do {                                                     \
        char FORMAT[256];                                    \
        sprintf(FORMAT, "\033[;35m" "%s" "\033[0m", format); \
        ZLOG_DEBUG(sLogCatInfo[catId].zc, FORMAT, ##args);   \
    } while (0)

#define CMM_INFO(catId, format, args...) \
    ZLOG_INFO(sLogCatInfo[catId].zc, format, ##args)

#define CMM_NOTICE(catId, format, args...)                   \
                                                             \
    do {                                                     \
        char FORMAT[256];                                    \
        sprintf(FORMAT, "\033[;34m" "%s" "\033[0m", format); \
        ZLOG_NOTICE(sLogCatInfo[catId].zc, FORMAT, ##args);  \
    } while (0)

#define CMM_TRACE(catId, format, args...) \
    ZLOG_TRACE(sLogCatInfo[catId].zc, format, ##args)

#endif

#define CMM_WARN(catId, format, args...)                     \
    do {                                                     \
        char FORMAT[256];                                    \
        sprintf(FORMAT, "\033[;33m" "%s" "\033[0m", format); \
        ZLOG_WARN(sLogCatInfo[catId].zc, FORMAT, ##args);    \
    } while (0)

#define CMM_ERROR(catId, format, args...)                    \
    do {                                                     \
        char FORMAT[256];                                    \
        sprintf(FORMAT, "\033[;31m" "%s" "\033[0m", format); \
        ZLOG_ERROR(sLogCatInfo[catId].zc, FORMAT, ##args);   \
    } while (0)

enum {
    ZLOG_LEVEL_TRACEDATA = 25,
    ZLOG_LEVEL_TRACE = 30,
};

#define ZLOG_TRACEDATA(cat, format, args...)                                            \
    zlog(cat, __FILE__, sizeof(__FILE__) - 1, __func__, sizeof(__func__) - 1, __LINE__, \
         ZLOG_LEVEL_TRACEDATA, format, ##args)

#define CMM_TRACEDATA(catId, format, args...) \
    ZLOG_TRACEDATA(sLogCatInfo[catId].zc, format, ##args)

int cmmTraceData(int id, int len, const char* data);

int cmmLogInit(char* confFile);
void cmmLogTerminate(void);
int cmmLogPutMdc(char* key, char* value);
#endif
