/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "cmm_zlog.h"

logCatInfo_s sLogCatInfo[LOG_MAX + 1] = {
    { LOG_CMM, "cmm", NULL },
    { LOG_APP, "app", NULL },
    { LOG_MAC, "mac", NULL },
    { LOG_MAX, "", NULL },
};

int cmmLogInit(char* confFile)
{
    int rc;

    rc = zlog_init(confFile);
    if (rc) {
        printf("init failed\n");
        return -1;
    }
    int i;
    for (i = 0; i < LOG_MAX; i++) {
        sLogCatInfo[i].zc = zlog_get_category(sLogCatInfo[i].catName);
        if (!sLogCatInfo[i].zc) {
            printf("get %s fail\n", sLogCatInfo[i].catName);
            zlog_fini();
            return -2;
        }
    }
    return 0;
}

int cmmLogPutMdc(char* key, char* value)
{
    return zlog_put_mdc(key, value);
}

void cmmLogTerminate(void)
{
    zlog_fini();
}

int cmmTraceData(int id, int len, const char* data)
{
    int i, j, n = 0, num;
    char buf[4096], *s;

    CMM_TRACE(id, "********TRACE DATA BEGIN********");
    if (len < 0) {
        CMM_ERROR(id, "Trace> cmmTraceData(): len %d error!\n", len);
        return -1;
    }

    if (len == 0)
        return 0;

    if (data == NULL) {
        CMM_ERROR(id, "Trace> cmmTraceData(): null data pointer!\n");
        return -1;
    }

    if (len > 128)
        len = 128;

    for (s = buf, i = 0; i < len; i++) {
        if ((n = i % 16) == 0)
            for (j = 0; j < 10; j++, *s++ = ' ')
                ;
        else if (n == 8)
            *s++ = ' ', *s++ = '-', *s++ = ' ';
        else
            *s++ = ' ';

        num = data[i] >> 4;
        *s++ = num <= 9 ? num + '0' : num - 10 + 'a';

        num = data[i] & 0xf;
        *s++ = num <= 9 ? num + '0' : num - 10 + 'a';

        if (n == 15)
            *s++ = '\n';
    }

    if (n != 15)
        *s++ = '\n';
    *s = '\0';

    CMM_TRACEDATA(id, buf);
    CMM_TRACE(id, " ********TRACE DATA END********");
    return 0;
}
