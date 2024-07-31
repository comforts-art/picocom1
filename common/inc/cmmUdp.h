/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __CMM_UDP_H__
#define __CMM_UDP_H__
#include <netinet/in.h>
#include <sys/socket.h>

#include "cmmTypes.h"

typedef struct
    {
    int fd;
    uint16_t rmtPort;
    uint16_t lcPort;
    struct in_addr lcIp;
    struct in_addr rmtIp;
} UdpCfg_s;
int initUdpConnectedFile(UdpCfg_s* cfg);

int initListenUdpFile(UdpCfg_s* cfg);
#ifdef __GTP_ZBC_ENABLE__
int initZbcListenUdpFile(UdpCfg_s* cfg);
#endif

int listenUdp(UdpCfg_s* cfg);

int receiveCheckUdp(UdpCfg_s* cfg, void* data, uint32_t bufLen);

int setUdpNonblock(int fd);

#endif
