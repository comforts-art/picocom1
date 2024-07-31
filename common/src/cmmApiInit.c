/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "cmmApiInit.h"
#include "cmmDebug.h"
#include "cmmMsgDesc.h"
#include "cmmUdp.h"
#include "uuMonMsg.h"

#define _MODULE MOD_CMM

PlatformCfg_s platformCfg;

void cmmApiInit(char* ipAddress)
{
    memset(&platformCfg, 0, sizeof(platformCfg));
    platformCfg.modMsgTrace.type = TRACE_FALSE;
    platformCfg.modMsgTrace.port = 0;

    platformCfg.phyTrace.type = TRACE_UDP;
    platformCfg.phyTrace.port = 6882;
    strcpy(platformCfg.phyTrace.str, ipAddress);

    platformCfg.uuTrace.type = TRACE_FALSE;
    platformCfg.uuTrace.port = 0;

    platformCfg.debugCfg.type = TRACE_FALSE;
    platformCfg.debugCfg.port = 0;

    platformCfg.monSvrIp.type = TRACE_FALSE;
    platformCfg.monSvrIp.port = 0;

    platformCfg.dbgLevPresent = false;
    char cfgStr[2000] = {};
    CmmBuf_s out = { 0 };
    out.octCnt = 2000;
    out.data = (uint8_t*)cfgStr;
    snprintfData(&out, 1, "platformCfg", ADDR_PLATFORMCFG_S_DEF, &platformCfg);
    PR_INFO("%s", cfgStr);
    if (platformCfg.dbgLevPresent) {
        setupgDbgLevSet(platformCfg.dbgLev);
    }
}

static UdpCfg_s uuMonUdp = { .fd = -1 };

void sendUuMonMsg(const UuMonMsg_s* msg)
{
    if (platformCfg.monSvrIp.type != TRACE_UDP) {
        return;
    }

    if (uuMonUdp.fd < 0) {

        inet_aton(platformCfg.monSvrIp.str, &uuMonUdp.rmtIp);
        uuMonUdp.rmtPort = platformCfg.monSvrIp.port;
        uuMonUdp.lcPort = platformCfg.monSvrIp.port + 1;
        initUdpConnectedFile(&uuMonUdp);
    }

    if (0 > write(uuMonUdp.fd, msg, sizeof(*msg))) {
        PR_ERROR("SendUUMon Failed\n");
    }
}
