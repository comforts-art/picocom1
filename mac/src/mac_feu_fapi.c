/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#define _GNU_SOURCE
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

//#include "cmm_thread.h"
#include "cmmApiInit.h"
#include "cmm_zlog.h"
#include "mac.h"
#include "semaphore.h"
//#include "pcxx_ipc.h"
#include "mac_feu_fapi.h"
#include "mac_oam_interface.h"
#include "pcxx_oam.h"
#include "rte_pmd_pc802.h"

#define OAM_SUCCESS 0
#define OAM_FAIL -1

#define UDP_PORT 6885
#define OAM_MSG_MAX_LEN 2048

#define CLIENTIP "172.16.30.31"

typedef struct {
    pcxx_oam_sub_msg_t sub;
    FEUMsgHdr_t feu;
    FEUGeneralMsgHdr_t gen;
} oam_p19_data;

typedef struct {
    sem_t sem;
    char buf[2048];
} p19_decode_data;

typedef enum e_pfi_p19_type {
    P19_REQ = 0,
    P19_RSP,
} e_pfi_p19_type;

typedef enum {
    PICO_OAM_MSG = 0x00,
    PICO_P19_MSG = 0x01,
    PICO_DEBUG_MSG = 0x02,
} OamMessageType_e;

static struct sockaddr_in serveraddr[2], clientaddr[2];
static int32_t server_fd = -1;
static int32_t client_fd = -1;

int32_t oamSendAppRsp(OamMessageType_e msgType, uint8_t* pMsg, uint16_t msgSize)
{
    uint32_t send_length = 0;

    CMM_DEBUG(LOG_MAC, "oamSendAppRsp, msgSize = %d", msgSize);
#if 0
	if (client_fd < 0) {
		if((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		    CMM_ERROR(LOG_MAC,"socket() error");
		    exit(0);
		}

        bzero(&clientaddr[1],sizeof(clientaddr[1]));
		clientaddr[1].sin_family = AF_INET;
		clientaddr[1].sin_port = htons(UDP_PORT+1);
		//clientaddr[1].sin_addr.s_addr = inet_addr(CLIENTIP);
		inet_aton(platformCfg.phyTrace.str, &clientaddr[1].sin_addr);
    }
#endif
    send_length = sendto(server_fd, (char*)pMsg, msgSize, 0, (struct sockaddr*)&clientaddr[0], sizeof(clientaddr[0]));
    if (send_length != msgSize) {
        printf("UDP buffer blocked,send_length = %d\n", send_length);
        return OAM_FAIL;
    }

    return OAM_SUCCESS;
}

int32_t p19_config_rsp(void* arg, uint16_t dev_index, uint32_t msg_type, const pcxx_oam_sub_msg_t** sub_msg, uint32_t msg_num)
{
    FEUMsgHdr_t* feu = (FEUMsgHdr_t*)sub_msg[0]->msb_body;
    FEUGeneralMsgHdr_t* pHdr = (FEUGeneralMsgHdr_t*)&feu[1];

    printf("received FEU_FAPI_RESPONSE:port_id = %u, msg_num = %u, size= %d, Line %d, %s\n",
        dev_index, msg_num, sub_msg[0]->msg_size, __LINE__, __func__);
    if (sub_msg[0]->msg_size < sizeof(FEUMsgHdr_t) + sizeof(FEUGeneralMsgHdr_t))
        printf("p19 Message is too small size %d\n", sub_msg[0]->msg_size);

    oamSendAppRsp(PICO_P19_MSG, (uint8_t*)pHdr, pHdr->msgSize);
    return OAM_SUCCESS;
}

int32_t p19_config_req(uint8_t* pInput, uint16_t msgId, uint16_t msgSize,uint16_t cellid)
{
    uint8_t param_buf[2048] = { 0 };
    pcxx_oam_sub_msg_t* sub_msg = (pcxx_oam_sub_msg_t*)param_buf;
    const pcxx_oam_sub_msg_t* list = (const pcxx_oam_sub_msg_t*)sub_msg;
    FEUMsgHdr_t* feu = (FEUMsgHdr_t*)sub_msg->msb_body;

    sub_msg->msg_id = P19_REQ;
    sub_msg->msg_size = sizeof(FEUMsgHdr_t) + msgSize; /*FEUGeneralMsgHdr_t included in msgSize*/

    /** FEU Msg Head*/
    feu->msgNum = 1;
    feu->opaque = cellid;

    memcpy(&feu[1], pInput, msgSize);
    printf("send FEU_FAPI_MSG, msgsize = %d\n", msgSize);
    if (0 != pcxx_oam_send_msg(0, PCXX_P19_MSG, &list, 1))
        printf("FEU_FAPI_MSG send err!\n");

    return OAM_SUCCESS;
}

int32_t oam_config_rsp(void* arg, uint16_t dev_index, uint32_t msg_type, const pcxx_oam_sub_msg_t** sub_msg, uint32_t msg_num)
{
    printf("received OAM_PHY_RSP:port_id = %u, msg_num = %u, size = %d, Line %d, %s\n",
        dev_index, msg_num, sub_msg[0]->msg_size, __LINE__, __func__);
    oamSendAppRsp(PICO_OAM_MSG, (uint8_t*)sub_msg[0], sub_msg[0]->msg_size + sizeof(pcxx_oam_sub_msg_t));
    return OAM_SUCCESS;
}

static int oam_config_req(uint8_t* pTlv, uint16_t msgId, uint16_t msgSize, uint8_t cellid)
{
    uint8_t buf[2048] = { 0 };
    pcxx_oam_sub_msg_t* sub_msg = (pcxx_oam_sub_msg_t*)buf;
    const pcxx_oam_sub_msg_t* list = (const pcxx_oam_sub_msg_t*)sub_msg;

    sub_msg->msg_id = msgId;
    sub_msg->msg_size = msgSize-sizeof(FEUGeneralMsgHdr_t);
    if (msgSize)
        memcpy(sub_msg->msb_body, (pTlv+sizeof(FEUGeneralMsgHdr_t)), sub_msg->msg_size);

    if (0 != pcxx_oam_send_msg(0, PCXX_OAM_MSG, &list, 1))
        printf("OAM_PHY_REQ send err!\n");

    return 0;
}

int32_t oamConfigMsgDec(uint8_t* pRecvMsg, uint32_t msgSize)
{
    // FEUMsgHdr_t *msgHdr = (FEUMsgHdr_t *)pRecvMsg;
    FEUGeneralMsgHdr_t* genHdr = (FEUGeneralMsgHdr_t*)pRecvMsg;

    switch (genHdr->msgId) {
    case FEU_FAPI_PARAM_REQUEST:
    case FEU_FAPI_CONFIG_REQUEST:
    case FEU_FAPI_START_REQUEST:
    case FEU_FAPI_DFE_CONFIG_REQUEST:
    case FEU_FAPI_STOP_REQUEST:
        CMM_DEBUG(LOG_MAC, "msgId = %d, msgSize = %d, ", genHdr->msgId, genHdr->msgSize);
        p19_config_req(pRecvMsg, genHdr->msgId, genHdr->msgSize,genHdr->cellid);
        break;
    case PC802_TEMP_GET_REQ:
	case ECPRI_GET_REQ:
	case ECPRI_SET_REQ:
	case ECPRI_TEST_NTFY:
	case ECPRI_ERR_IND:
        CMM_DEBUG(LOG_MAC, "msgId = %d, msgSize = %d, ", genHdr->msgId, genHdr->msgSize);
        oam_config_req(pRecvMsg, genHdr->msgId, genHdr->msgSize, 0);
        break;
    default:
        CMM_ERROR(LOG_MAC, "illegal message msgId %d", genHdr->msgId);
        return OAM_FAIL;
    }

    return OAM_SUCCESS;
}

static void setnonblocking(int sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0) {

        printf("fcntl F_GETFL fail\n");
        return;
    }
    if (fcntl(sockfd, F_SETFL, flag | O_NONBLOCK) < 0) {
        printf("fcntl F_SETFL fail\n");
    }
}

void oamRecvAppMsg()
{
    uint32_t server_addr_len, client_addr_len;
    int32_t recv_length = 0;
    uint8_t recvBuf[OAM_MSG_MAX_LEN];

    if (server_fd < 0) {
        if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            CMM_ERROR(LOG_MAC, "socket() error");
            exit(0);
        }
        setnonblocking(server_fd);
        bzero(&serveraddr[0], sizeof(serveraddr[0]));
        serveraddr[0].sin_family = AF_INET;
        serveraddr[0].sin_addr.s_addr = htonl(INADDR_ANY);
        // inet_aton(platformCfg.phyTrace.str, &serveraddr.sin_addr);
        serveraddr[0].sin_port = htons(UDP_PORT);
        server_addr_len = sizeof(serveraddr);

        bzero(&clientaddr[0], sizeof(clientaddr[0]));
        clientaddr[0].sin_family = AF_INET;
        clientaddr[0].sin_port = htons(UDP_PORT);
        // clientaddr[0].sin_addr.s_addr = inet_addr(CLIENTIP);
        inet_aton(platformCfg.phyTrace.str, &clientaddr[0].sin_addr);

        if (bind(server_fd, (struct sockaddr*)&serveraddr[0], server_addr_len) < 0) {
            CMM_ERROR(LOG_MAC, "bind() error");
            close(server_fd);
            exit(0);
        }

        pcxx_oam_register(PCXX_OAM_MSG, oam_config_rsp, NULL);
        pcxx_oam_register(PCXX_P19_MSG, p19_config_rsp, NULL);
    }
    client_addr_len = sizeof(clientaddr[0]);
    recv_length = recvfrom(server_fd, recvBuf, sizeof(recvBuf), 0, (struct sockaddr*)&clientaddr[0], &client_addr_len);
    if (recv_length > 0) {
        printf("recv msg from app, len = %d\n", recv_length);
        oamConfigMsgDec(recvBuf, recv_length);
    }
}
