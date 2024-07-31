/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef _MODULE_INF_H
#define _MODULE_INF_H

typedef enum {
    CTRL_PHY_INIT = 1,
    CTRL_PHY_CFG,
    CTRL_CELL_SETUP,
    CTRL_PHY_START,
    CTRL_PHY_STOP,
    CTRL_DL_TTI_REQ,
    CTRL_UL_TTI_REQ,
    CTRL_UL_DCI_REQ,
    CTRL_TX_DATA_REQ,
    CTRL_RX_DATA_REQ,
    CTRL_CRC_REQ,
    CTRL_UCI_REQ,
    CTRL_SRS_REQ,
    CTRL_RACH_REQ,
    CTRL_TEST_CASE,
    CTRL_TEST_INIT,
    APP_START_TEST = 100,
    APP_SIM_TTI,
    APP_STOP_TEST,
    APP_TC_START,
    APP_TC_CONFIG_CASE,
    APP_TC_DL_CASE,
    APP_TC_UL_CASE,
    APP_TC_TX_CASE,
    APP_TX_CASE,
    APP_TC_STOP,
    CTRL_PHY_INIT_RSP = 200,
    CTRL_PHY_CFG_RSP,
    CTRL_PHY_START_RSP,
    CTRL_PHY_STOP_RSP,
    CTRL_TEST_INIT_RSP
} CtrlMsg_ev;
typedef uint16_t CtrlMsg_e;

typedef struct {
    CtrlMsg_e msgId;
    uint16_t msgLen;
    uint32_t paras[4];
} CtrlMsg_s;

#endif
