/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include "cmmDebug.h"
#include "cmmApiInit.h"

LogLevel_e gDbgLevelGlb = LOG_OF_INFO;

LogLevel_e gDbgLevelMod[N_DBG_MODS] = {
        [0 ... N_DBG_MODS - 1] = LOG_OF_INFO
};

static const char* const mod_name[N_DBG_MODS] = {
#ifdef DBG_MOD_NAME
#undef DBG_MOD_NAME
#endif
#define DBG_MOD_NAME(mod) #mod,
#include "cmmDebug.dat"
#undef DBG_MOD_NAME
};

/** The number of debug output buffers.
* \note Must be a power of 2, so that bitwise-AND may be used when incrementing
* the buffer index to find the modulo, instead of % which involves a
* computationally expensive division operation. */
#define N_DBG_BUFS (1 << 2)

/** The length of the debug output buffers. */
#define MAX_DBG_LEN 4096

/* For user space demonstration only. */
#define ATOMIC_T unsigned
#define ATOMIC_INC(i) i++

/*---------------------------------------------------------------------------*/

/**
 * Format functions for printf and alike.
 *
 */
const char* dbgFmt(const char* fmt, ...)
{
    /* Debug output buffers. We use more than one to allow for re-entrancy. */
    static struct
    {
        char a[N_DBG_BUFS][MAX_DBG_LEN];
        ATOMIC_T dbgBufIndex;
    } buf;

    va_list ap;
    char* s = buf.a[ATOMIC_INC(buf.dbgBufIndex) & (N_DBG_BUFS - 1)];
    va_start(ap, fmt);
    vsnprintf(s, MAX_DBG_LEN, fmt, ap);
    va_end(ap);
    return s;
}

/*
 * Logging for debug messages.
 */
void dbgToLog(
    LogLevel_e lev,
    DbgModule_e mod,
    const char* fileName,
    unsigned lineNo,
    const char* s)
{
    if (lev < N_LOG_LEVELS && mod < N_DBG_MODS) {
        /* judge log type, */
        switch (platformCfg.debugCfg.type) {
        case TRACE_TERMINAL: {
            logToStderr(lev, mod_name[mod], fileName, lineNo, s);
            break;
        }
        case TRACE_SYS_LOG: {
            logToSyslog(lev, mod_name[mod], fileName, lineNo, s);
            break;
        }
        case TRACE_FILE: {
            logToFilelog(lev, mod_name[mod], fileName, lineNo, s);
            break;
        }
        default:
            break;
        }
    }

    return;
}

void noDbgToLog(const char* fmt, ...)
{
    ASSERT(fmt);
}

bool setupgDbgLevGlb(LogLevel_e lev)
{
    if (lev < N_LOG_LEVELS) {
        gDbgLevelGlb = lev;
        return true;
    } else {
        return false;
    }
}

bool setupgDbgLevMod(LogLevel_e lev,
                     DbgModule_e mod)
{
    if (lev < N_LOG_LEVELS && mod < N_DBG_MODS) {
        gDbgLevelMod[mod] = lev;
        return true;
    } else {
        return false;
    }
}

void setupgDbgLevSet(uint8_t level[32])
{
    int i;
    ASSERT(N_DBG_MODS < 32);
    for (i = 0; i < N_DBG_MODS; i++) {
        gDbgLevelMod[i] = level[i];
    }
}

void printHexData(uint8_t* start, uint32_t len, const char* flag)
{
    uint32_t i;
    uint8_t* data = start;
    printf("-----%s len[%u]\n", flag, len);
    for (i = 0; i < len; i++) {
        printf(" %02x", data[i]);
        if (0xf == (i & 0xf)) {
            printf("\n");
        }
    }
    printf("-----end\n");
}

/** @} */
