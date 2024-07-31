/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cmmApiInit.h"
#include "cmmTypes.h"
#include "cmm_thread.h"
#include "macTrace.h"
#include "platformCfg.h"

static struct sockaddr_in _rmtAddr[2] = {};
static int _ptFd = -1;
static int _ptMacFd = -1;
// static pthread_t _tid;

#define MAX_PHY_TRACE_ITEM 32
struct _PhyTraceBuf_s {
    uint16_t start;
    uint16_t end;
    OctString_s item[MAX_PHY_TRACE_ITEM];
} _PhyTraceBuf = { 0 };

#define PHY_TRACE_ITEM_CNT(start, end) ((end + MAX_PHY_TRACE_ITEM - start) & (MAX_PHY_TRACE_ITEM - 1))

void phyTrace(void* data, uint32_t octCnt);

static void* _phyTraceCheckProcess(void* pArg)
{
#if 0
    struct sched_param sp;
    sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if (sched_setscheduler(0, SCHED_FIFO, &sp) >= 0) {
        printf("sched_setscheduler Success (%d)", sp.sched_priority);
    } else {
        printf("sched_setscheduler False(%d)", sp.sched_priority);
    }

    /*The thread  should block the signal from system*/
    sigset_t signal_mask; /* signals to block*/
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGINT);
    sigaddset(&signal_mask, SIGPIPE);
    sigaddset(&signal_mask, SIGCHLD);
    int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
    //setCpuId(0);
    if (rc != 0) {
        printf("[%s : %s] Block signal error  \n", __FILE__, __func__);
    }
#endif
    if (NULL == pArg) {
        printf("No para for trace\n");
    }
    uint32_t i, start;
    uint16_t traceCnt, traceIdx;
    /*watch all fds to asynclib*/
    while (1) {
        usleep(100);
        start = _PhyTraceBuf.start;
        traceCnt = PHY_TRACE_ITEM_CNT(start, _PhyTraceBuf.end);
        if (0 == traceCnt) {
            usleep(100);
        } else {
            for (i = 0; i < traceCnt; i++) {
                traceIdx = (start + i) & (MAX_PHY_TRACE_ITEM - 1);
                phyTrace(_PhyTraceBuf.item[traceIdx].data, _PhyTraceBuf.item[traceIdx].octCnt);
            }
            _PhyTraceBuf.start = (start + traceCnt) & (MAX_PHY_TRACE_ITEM - 1);
        }
    }
    return NULL;
}

void addPhyTraceItem(const void* data, uint32_t len)
{
    if (platformCfg.phyTrace.type == TRACE_UDP) {
        uint16_t end = _PhyTraceBuf.end;
        _PhyTraceBuf.item[end].data = (uint8_t*)data;
        _PhyTraceBuf.item[end].octCnt = len;
        _PhyTraceBuf.end = (end + 1) & (MAX_PHY_TRACE_ITEM - 1);
    }
}

static CmmBool_t _startPhyTraceThread()
{
    if (platformCfg.phyTrace.type == TRACE_UDP) {
        Thread_s* rtThread = NULL;
        rtThread = cmm_threadCreate("phyTraceThread", _phyTraceCheckProcess, NULL, 55, 0x100000);
        if (NULL == rtThread) {
            printf("SYS error[%d] %s\n", errno, strerror(errno));
            return false;
        }
    }
    return true;
}

int openL1Trace()
{
    _startPhyTraceThread();
    return 0;
}

void phyTrace(void* data, uint32_t octCnt)
{
    int res, rev;
    if (platformCfg.phyTrace.type == TRACE_UDP) {
        if (_ptFd < 0) {
            _ptFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            _rmtAddr[0].sin_family = AF_INET;
            _rmtAddr[0].sin_port = htons(platformCfg.phyTrace.port);
            inet_aton(platformCfg.phyTrace.str, &_rmtAddr[0].sin_addr);
#if 0

            rev = connect(_ptFd, (struct sockaddr*)_rmtAddr, sizeof(_rmtAddr[0]));
            if (rev < 0) {
                printf("Phy Trance Connect Error, rev = %d \n", rev);
                close(_ptFd);
                platformCfg.phyTrace.type = TRACE_FALSE;
                return;
            }
#endif
            _ptMacFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            _rmtAddr[1] = _rmtAddr[0];
            _rmtAddr[1].sin_port = htons(platformCfg.phyTrace.port + 1);

            rev = connect(_ptMacFd, (struct sockaddr*)&_rmtAddr[1], sizeof(_rmtAddr[1]));
            if (rev < 0) {
                printf("Phy Trance Connect Error, rev = %d \n", rev);
                close(_ptFd);
                close(_ptMacFd);
                platformCfg.phyTrace.type = TRACE_FALSE;
                return;
            }

#if 0
            _rmtAddr[1] = _rmtAddr[0];
            _rmtAddr[1].sin_port = htons(platformCfg.phyTrace.port + 1);

            _rmtAddr[2] = _rmtAddr[0];
            _rmtAddr[2].sin_port = htons(platformCfg.phyTrace.port + 2);
#endif
        }
        int sendLen = (int)octCnt;
#if 0

        TXSDUREQ* txSdu = data;
        switch( txSdu->msgType)
        {
            case PHY_TXSDU_REQ:
                if(txSdu->nbDlChannelType == 4)
                {
                    _cvtAndSndTxSdu(data);
                    return;
                }

				else if(txSdu->nbDlChannelType == 3)
				{
                    _cvtAndSndPdcch(data);
					return;
				}


            break;
            case PHY_RXSDU_IND:
                _cvtAndSndRxSdu(data);
                return;

            default:
                break;
        }
#endif

        if ((res = sendto(_ptFd, data, sendLen, 0, (struct sockaddr*)&_rmtAddr[0], sizeof(_rmtAddr[0]))) != sendLen) {
            printf("UDP buffer blocked %d\n", res);
            return;
        }
        return;
        if (sendLen < 1470) {
            if ((res = write(_ptFd, data, sendLen)) != sendLen) {
                return;
            }
        } else {
        }
    }
}

void traceStrInfo(char* str)
{
    phyTrace(str, strlen(str));
}
