/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include "cmmLog.h"

/** For file operation*/
static FILE* log_file_handle = NULL;
static char log_file_name[2][256];
static char debug_log_file_name[256];

static int
openFile(int currentFileNo);

static int
closeFile(int currentFileNo);

static int
writeFile(int currentFileNo,
          const char* modName,
          const char* fileName,
          unsigned lineNo,
          const char* s);

/** Initialize the names of two log files.
* It innitialize the names of two log files, base on the input name, 
* if the input name is "TEST", the two log file will be "TEST_A", "TEST_B".
* @param[in] point of the file name
* @return returns void.
*/
void logInit(const char* logFileName)
{
    assert(logFileName != NULL);

    memset(debug_log_file_name, 0x00, sizeof(256));

    if (logFileName != NULL) {
        strcpy(debug_log_file_name, logFileName);
    }

    memset(log_file_name[0], 0x00, sizeof(256));
    memset(log_file_name[1], 0x00, sizeof(256));

    strcat(log_file_name[0], debug_log_file_name);
    strcat(log_file_name[0], "_A");
    strcat(log_file_name[1], debug_log_file_name);
    strcat(log_file_name[1], "_B");
}

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
                 const char* s)
{

    time_t current = time(NULL);
    struct tm* p;
    p = gmtime(&current);

    switch (lev) {
    case LOG_OF_ERROR:
        printf("\033[;31m"
               "[%.4d.%.2d.%.2d %.2d:%.2d:%.2d] %s %s:%u %s\n"
               "\033[0m",
               (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min,
               p->tm_sec,
               modName, file, lineNo, s);
        break;

    case LOG_OF_WARNING:
        printf("\033[;33m"
               "[%.4d.%.2d.%.2d %.2d:%.2d:%.2d] %s %s:%u %s\n"
               "\033[0m",
               (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec,
               modName, file, lineNo, s);
        break;
    case LOG_OF_INFO:
        printf("\033[;35m"
               "[:%.2d:%.2d] %s  %s\n"
               "\033[0m",
               p->tm_min, p->tm_sec,
               modName, s);
        break;

    case LOG_OF_DEBUG:
        printf("[%.2d:%.2d] %s  %s\n",
               p->tm_min, p->tm_sec,
               modName, s);
        break;

    default:
        break;
    }
}

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
                 const char* s)
{
    switch (lev) {
    case LOG_OF_ERROR:
        syslog(LOG_ERR, "%s %s: %u %s\n", modName, file, lineNo, s);
        break;
    case LOG_OF_WARNING:
        syslog(LOG_WARNING, "%s %s: %u %s\n", modName, file, lineNo, s);
        break;
    case LOG_OF_INFO:
        syslog(LOG_INFO, "%s %s: %u %s\n", modName, file, lineNo, s);
        break;
    case LOG_OF_DEBUG:
        syslog(LOG_DEBUG, "%s %s: %u %s\n", modName, file, lineNo, s);
        break;
    default:
        break;
    }
}

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
void logToFilelog(__attribute__((__unused__)) LogLevel_e lev,
                  const char* modName,
                  const char* fileName,
                  unsigned lineNo,
                  const char* s)
{
    /* Index number for log_file_name array, just 0 or 1 valued.*/
    static int currentFileNo = 0;
    int ret = 0;

    if (log_file_handle == NULL) {
        if (openFile(currentFileNo) == 0) {
            ret = writeFile(currentFileNo,
                            modName,
                            fileName,
                            lineNo,
                            s);
        }
    } else {
        ret = writeFile(currentFileNo,
                        modName,
                        fileName,
                        lineNo,
                        s);
    }

    if (ret != 0) {
        //lint -e534
        closeFile(currentFileNo);
        if (ret == 1) {
            currentFileNo += 1;
            currentFileNo &= 1;
        }
    }
}

/** Open file .
* It open and truncate a file to 0 or creat a file by the name stored in log_file_name array.
* 
* @param[in] currentFileNo the number of file which is shall being written.
* @return returns -1, 0; -1: open or create unsuccessfully, 0: open or create successfully.
*/
static int
openFile(int currentFileNo)
{
    if (currentFileNo != 0 && currentFileNo != 1) {
        return -1;
    }

    FILE* new_file;

    new_file = fopen(log_file_name[currentFileNo], "w+");

    if (NULL != new_file) {
        log_file_handle = new_file;
        printf("Debug log file - %s created/opened!\n",
               log_file_name[currentFileNo]);
        return 0;
    } else {
        log_file_handle = NULL;
        syslog(LOG_ERR, "Debug Log Create/Open Error!");
        return -1;
    }
}

/** Close file .
* It close the file through operating file handle.
* 
* @param[in] currentFileNo the number of file which has be written.
* @return returns -1, 0; -1: close unsuccessfully, 0: close successfully.
*/
static int
closeFile(int currentFileNo)
{
    if (currentFileNo != 0 && currentFileNo != 1) {
        return -1;
    }

    if (fclose(log_file_handle) != 0) {
        printf("\033[;31m"
               "Debug log file - %s close error!\n"
               "\033[0m",
               log_file_name[currentFileNo]);
    } else {
        log_file_handle = NULL;
        return 0;
    }
    log_file_handle = NULL;
    return -1;
}

/** Write debug information to file.
* It write the module name, file name, line no and extra information to the file.
* 
* @param[in] currentFileNo the number of file which is being written. 
* @param[in] modName point of the name of the module which DBG within.
* @param[in] modName point of the name of file which DBG within.
* @param[in] modName number of the line which DBG within.
* @param[in] s point of the extra information.
* @return returns -1, 0, 1; -1: write unsuccessfully, 0: write successfully, 1: reach the upper limit of file size.
*/
static int
writeFile(int currentFileNo,
          const char* modName,
          const char* fileName,
          unsigned lineNo,
          const char* s)
{
    if (currentFileNo != 0 && currentFileNo != 1) {
        return -1;
    }

    static int fileByteCount = 0;
    int rt;

    time_t current = time(NULL);
    struct tm* p;
    p = gmtime(&current);

    rt = fprintf(log_file_handle,
                 "[%.4d.%.2d.%.2d %.2d:%.2d:%.2d] %s %s: %u %s\n",
                 (1900 + p->tm_year),
                 (1 + p->tm_mon),
                 p->tm_mday,
                 p->tm_hour,
                 p->tm_min,
                 p->tm_sec,
                 modName,
                 fileName,
                 lineNo,
                 s);

    if (rt < 0) {
        fileByteCount = 0;
        printf("\033[;31m"
               "Debug log file - %s write error!\n"
               "\033[0m",
               log_file_name[currentFileNo]);
        return -1;

    } else {
        if (fflush(log_file_handle) != 0) {
            printf("\033[;31m"
                   "Debug log file - %s flush error!\n"
                   "\033[0m",
                   log_file_name[currentFileNo]);
            return -1;
        }
        fileByteCount += rt;
        if (fileByteCount >= MAX_FILE_SIZE) {
            fileByteCount = 0;
            return 1;
        }
        return 0;
    }
}

/** @} */
