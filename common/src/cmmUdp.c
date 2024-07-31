/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cmmDebug.h"
#include "cmmTypes.h"
#include "cmmUdp.h"
#define _MODULE MOD_CMM
#define UDP_ERR PR_ERROR
#define UDP_WARN PR_WARNING
#define UDP_INFO PR_INFO
#define UDP_DBG PR_DEBUG

int setUdpNonblock(int fd)
{
    int flags;
    flags = fcntl(fd, F_GETFL, 0);
    if (-1 == flags) {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int initUdpConnectedFile(UdpCfg_s* cfg)
{
    ASSERT(cfg);
    int rev;
    struct sockaddr_in lcAddr, rmtAddr;
    cfg->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (cfg->fd < 0) {
        UDP_ERR("init Socket error, fd = %d\n", cfg->fd);
        return cfg->fd;
    }

    UDP_INFO("Open Connection Fd = %d\n", cfg->fd);

    lcAddr.sin_family = AF_INET;
    lcAddr.sin_port = htons(cfg->lcPort);
    lcAddr.sin_addr = cfg->lcIp;

    rmtAddr.sin_family = AF_INET;
    rmtAddr.sin_port = htons(cfg->rmtPort);
    rmtAddr.sin_addr = cfg->rmtIp;

    rev = bind(cfg->fd, (struct sockaddr*)&lcAddr, sizeof(lcAddr));
    if (rev < 0) {
        UDP_ERR("Binding Error, rev = %d \n", rev);
        close(cfg->fd);
        cfg->fd = -1;
        return rev;
    }
    UDP_INFO("Binding Rev %d \n", rev);

    rev = connect(cfg->fd, (struct sockaddr*)&rmtAddr, sizeof(lcAddr));
    if (rev < 0) {
        UDP_ERR("Connect Error, rev = %d \n", rev);
        close(cfg->fd);
        cfg->fd = -1;
        return rev;
    }
    return rev;
}

int initListenBroadCastUdp(UdpCfg_s* cfg)
{
    int rev;
    ASSERT(cfg);

    struct sockaddr_in lcAddr;

    cfg->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (cfg->fd <= 0) {
        UDP_ERR("Open Listen socket error, fd  = %d \n", cfg->fd);
        return cfg->fd;
    }
    UDP_INFO("Open Listen socket OK, fd  = %d \n", cfg->fd);

    lcAddr.sin_family = AF_INET;
    lcAddr.sin_port = htons(cfg->lcPort);
    lcAddr.sin_addr = cfg->lcIp;

    int bcast = 1;
    if (setsockopt(cfg->fd, SOL_SOCKET, SO_BROADCAST, &bcast, sizeof(bcast)) != 0) {
        UDP_ERR("Open Listen socket set option %d error\n", SO_BROADCAST);
        close(cfg->fd);
        cfg->fd = -1;
        return 1;
    }

    rev = bind(cfg->fd, (struct sockaddr*)&lcAddr, sizeof(lcAddr));
    if (rev < 0) {
        UDP_ERR("Binding Error, rev = %d \n", rev);
        return rev;
    }

    UDP_INFO("Binding Rev %d \n", rev);
    return rev;
}

int initListenUdpFile(UdpCfg_s* cfg)
{
    int rev;
    ASSERT(cfg);

    struct sockaddr_in lcAddr;

    cfg->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (cfg->fd <= 0) {
        UDP_ERR("Open Listen socket error, fd  = %d \n", cfg->fd);
        return cfg->fd;
    }
    UDP_INFO("Open Listen socket OK, fd  = %d \n", cfg->fd);

    lcAddr.sin_family = AF_INET;
    lcAddr.sin_port = htons(cfg->lcPort);
    lcAddr.sin_addr = cfg->lcIp;
    rev = bind(cfg->fd, (struct sockaddr*)&lcAddr, sizeof(lcAddr));
    if (rev < 0) {
        UDP_ERR("Binding Error, rev = %d \n", rev);
        return rev;
    }

    UDP_INFO("Binding Rev %d \n", rev);
    return rev;
}

int initZbcListenUdpFile(UdpCfg_s* cfg)
{
    int rev;
    ASSERT(cfg);

    struct sockaddr_in lcAddr;
    cfg->fd = socket(AF_INET, (SOCK_DGRAM | (1 << 30)), 0);
    if (cfg->fd <= 0) {
        UDP_ERR("Open Listen socket error, fd  = %d \n", cfg->fd);
        return cfg->fd;
    }
    setUdpNonblock(cfg->fd);
    UDP_INFO("Open Listen socket OK, fd  = %d \n", cfg->fd);

    lcAddr.sin_family = AF_INET;
    lcAddr.sin_port = htons(cfg->lcPort);
    lcAddr.sin_addr = cfg->lcIp;
    rev = bind(cfg->fd, (struct sockaddr*)&lcAddr, sizeof(lcAddr));
    if (rev < 0) {
        UDP_ERR("Binding Error, rev = %d \n", rev);
        return rev;
    }

    UDP_INFO("Binding Rev %d \n", rev);
    return rev;
}

int receiveCheckUdp(UdpCfg_s* cfg, void* data, uint32_t bufLen)
{
    struct sockaddr_in rmtAddr[2]; /*Safe for 8 byte rsv len in struct sockaddr*/
    socklen_t addrLen = sizeof(rmtAddr[0]);
    ssize_t revLen;

    revLen = recvfrom(cfg->fd, data, bufLen, 0, (struct sockaddr*)rmtAddr, &addrLen);
    UDP_INFO("Recv network data , len %zd\n", revLen);
    if (revLen <= 0) {
        return -1;
    }

    if (cfg->rmtIp.s_addr == INADDR_ANY) {
        cfg->rmtIp = rmtAddr->sin_addr;
        cfg->rmtPort = ntohs(rmtAddr->sin_port);
        connect(cfg->fd, (struct sockaddr*)rmtAddr, sizeof(rmtAddr[0]));
        return revLen;
    }

    if (cfg->rmtIp.s_addr != rmtAddr->sin_addr.s_addr) /*IP mismatch*/
    {
        return -1;
    }

    return revLen;
}
