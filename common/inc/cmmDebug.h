/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __CMM_DEBUG_H
#define __CMM_DEBUG_H
#include <errno.h>

#include "cmmLog.h"

#define DBG_MOD(mod) mod

#define DEBUG_MODULE(mod) \
    static DbgModule_e DBG_MOD __attribute__((unused)) = (mod);

/** Enumerates the MAC modules for use with the DBG() macro. */
typedef enum {
#ifdef DBG_MOD_NAME
#undef DBG_MOD_NAME
#endif
#define DBG_MOD_NAME(mod) mod,
#include "cmmDebug.dat"
#undef DBG_MOD_NAME
    N_DBG_MODS /**< special value- number of modules */
} DbgModule_e;

/**  Logging for debug messages.
*
*
*
*@param[in]
*@param[in]
*@param[in]
*@param[in]
*@param[in]
*@param[in]
*@return void
*/
void dbgToLog(
    LogLevel_e lev,
    DbgModule_e mod,
    const char* fileName,
    unsigned lineNo,
    const char* s);

/**  Do nothing logging.
*
*
*
*@param[in]
*@param[in]
*@return void
*/
void noDbgToLog(const char* fmt, ...)
    __attribute__((format(printf, 1, 2)));

/** Called by DBG() to format the printf-like arguments ready for output.
* The printf-like arguments are converted to a string in a static buffer,
* and a pointer to the string is returned.
*
*@param[in] fmt printf-like format string
*@param[in] ... printf-like values corresponding to the format string
*@return a pointer to the appropriate string
*/
const char*
dbgFmt(const char* fmt, ...)
    __attribute__((format(printf, 1, 2)));

/** Setup globe debug level.
*
*
*@param[in] lev The specified level for globle debug usage.
*@return Return true if it setup successfully,  and the false the same reasoning.
*/
bool setupgDbgLevGlb(LogLevel_e lev);

/** Setup module debug level separately.
*
*
*@param[in] lev The specified module been setup.
*@param[in] mod The specified level for the module.
*@return Return true if it setup successfully,  and the false the same reasoning.
*/
bool setupgDbgLevMod(LogLevel_e lev,
                     DbgModule_e mod);

/** Setup module debug level for all modus.
*
*
*@param[in] lev The specified module been setup.
*@param[in] mod The specified level for the module.
*@return Return true if it setup successfully,  and the false the same reasoning.
*/
void setupgDbgLevSet(uint8_t level[32]);

/** The global debug level.
* If the level value in a DBG() statement is greater than the value of this
* variable then the statement will not generate output.
* \note This value is global instead of accessed via functions for efficiency,
* because it is accessed within the DBG() macro.
*/
extern LogLevel_e gDbgLevelGlb;

/** The module debug levels.
* If the level value in a DBG() statement is greater than the module's value
* in this array then the statement will not generate output.
* The values are indexed by DbgModule_e.
* \note This array is global instead of accessed via functions for efficiency,
* because it is accessed within the DBG() macro.
*/
extern LogLevel_e gDbgLevelMod[N_DBG_MODS];

#define FAP_DEBUG_SWITCH 1

/** Conditional debug macro.
* The specified printf-like arguments are output to the debug stream if
* the \c lev level argument is less than or equal to the current global and
* module debug level values. Valid level values are:
* - 1 highly significant event eg. DBG(1, ("new service flow SFID=%d", sfId));
* - 2 medium significance event
* - 3 low significance event eg. DBG(3, ("starting loop i=%u", i));
*
* \note A carriage return is automatically added to the output.
* \note Use the DBG1(), DBG2() and DBG3() macros instead of this macro.
*
* Arguments:
* \arg \c lev debug level, must be <= global and module levels for output
* \arg \c printf_args bracketed printf-like arguments
*/
#if FAP_DEBUG_SWITCH
/* switch debug to enabled */
#define DBG(lev, fmt, ...)                                                                   \
    {                                                                                        \
        if ((lev <= gDbgLevelGlb) && (lev <= gDbgLevelMod[DBG_MOD(_MODULE)])) {              \
            /*Logging specified debug level message to stderr. */                            \
            dbgToLog(lev, DBG_MOD(_MODULE), __FILE__, __LINE__, dbgFmt(fmt, ##__VA_ARGS__)); \
        } else {                                                                             \
            ;                                                                                \
        }                                                                                    \
    }
#else
/* switch debug to disabled */
#define DBG(lev, fmt, ...)              \
    {                                   \
        noDbgToLog(fmt, ##__VA_ARGS__); \
    }
#endif

/** Abbreviation for DBG(logType, 1, printf_args).
* See DBG(). */
#define PR_ERROR(fmt, ...) \
    DBG(LOG_OF_ERROR, fmt, ##__VA_ARGS__)

/** Abbreviation for DBG(logType, 2, printf_args).
* See DBG(). */
#define PR_WARNING(fmt, ...) \
    DBG(LOG_OF_WARNING, fmt, ##__VA_ARGS__)

/** Abbreviation for DBG(logType, 4, printf_args).
* See DBG(). */
#define PR_INFO(fmt, ...) \
    DBG(LOG_OF_INFO, fmt, ##__VA_ARGS__)

/** Abbreviation for DBG(logType, 3, printf_args).
* See DBG(). */
#define PR_DEBUG(fmt, ...) \
    DBG(LOG_OF_DEBUG, fmt, ##__VA_ARGS__)

/**
 * Macro to function trace.
 */
#define CMM_FUNC_TRACE() \
    DBG(LOG_OF_WARNING, "%s%s%s", "Function ", __func__, " entry")

#define RUN_IN_DBG_LEVEL(lev) if (lev <= gDbgLevelMod[DBG_MOD(_MODULE)])

void traceStrInfo(char* str);

void printHexData(uint8_t* start, uint32_t len, const char* flag);

#endif /* __CMM_DEBUG_H */
