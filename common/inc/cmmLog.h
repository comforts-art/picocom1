/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __CMM_LOG_H__
#define __CMM_LOG_H__
#include <stdarg.h>
#include <syslog.h>
#include <time.h>

#include "cmmTypes.h"

/** Enumerates the log level for use log function. */
typedef enum {
    LOG_OF_ERROR = 0,
    LOG_OF_WARNING,
    LOG_OF_DEBUG,
    LOG_OF_INFO,
    N_LOG_LEVELS
} LogLevel_e;

/** Enumerates the log type for use log function. */
typedef enum {
    LOG_TO_STDERR = 0,
    LOG_TO_SYSLOG,
    LOG_TO_FILELOG,
    N_LOG_TYPES
} LogType_e;

#define MAX_FILE_SIZE (10 << 10)

/** Initialize the names of two log files.
* It innitialize the names of two log files, base on the input name, 
* if the input name is "TEST", the two log file will be "TEST_A", "TEST_B".
* @param[in] point of the file name
* @return returns void.
*/
void logInit(const char* logFileName);

/** Output log information to standard output.
* It output debug information according to the level of debug and the module it within.
* it could output the debug details like timestamp, module name, file name, line number and so on.
* @param[in] 
* @param[in]
* @param[in] 
* @param[in] 
* @param[in] 
* @return returns void.
*/
void logToStderr(LogLevel_e lev,
                 const char* modName,
                 const char* file,
                 unsigned lineNo,
                 const char* s);

/** Output log information to UNIX-like syslog.
* It output debug information according to the level of debug and the module it within.
* it could output the debug details like module name, file name, line number and so on.
* @param[in] 
* @param[in]
* @param[in] 
* @param[in] 
* @param[in] 
* @return returns void.
*/
void logToSyslog(LogLevel_e lev,
                 const char* modName,
                 const char* file,
                 unsigned lineNo,
                 const char* s);

/** Output log information to 2 files alternately.
* It output debug information according to the level of debug and the module it within.
* it could output the debug details like timestamp, module name, file name, line number and so on.
* Two files will be written alternately.
* @param[in] 
* @param[in] 
* @param[in] 
* @param[in] 
* @param[in] 
* @return returns void.
*/
void logToFilelog(LogLevel_e lev,
                  const char* modName,
                  const char* fileName,
                  unsigned lineNo,
                  const char* s);

#endif /*__CMM_LOG_H__*/

/** @} */
