/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

/**@brief Define testmac configuration struct be used in testmac project.
The file is come from testmacCfg.yaml.
*/
#ifndef __TESTMAC_CFG_H_
#define __TESTMAC_CFG_H_

#include "cmmMsgDesc.h"
#include "cmmTypes.h"

typedef enum {
    RUNMODE_FAPI_GENERATOR,
    RUNMODE_RCT_MODE,
    TESTMACRUNMODE_E_CNT
} TestMacRunMode_e;

typedef enum {
    AUTO,
    MANUAL,
    TESTMACOPERMODE_E_CNT
} TestMacOperMode_e;

typedef enum {
    CLIMODE_NONE_MODE,
    CLIMODE_REMOTE_MODE,
    CLIMODE_LOCAL_MODE,
    CLIMODE_REMOTE_LOCAL_MODE,
    TESTMACCLIMODE_E_CNT
} TestMacCliMode_e;

typedef enum {
    PDSCHMODE_AUTO_MODE,
    PDSCHMODE_LOAD_MODE,
} TestMacPdschMode_e;

/**@brief test mac configure*/
typedef struct
    {
    TestMacRunMode_e runMode;
    TestMacOperMode_e operMode;
    TestMacCliMode_e cliMode;
    TestMacPdschMode_e pdschMode;
    CmmBool_t phyStub;
    /**0 unlimited  unit is second*/
    uint16_t runtimeTotal;
    /**always >1  unit is second*/
    uint32_t runtimePerTc;
    int8_t atllloglevel;
    CmmBool_t sfnMode;
    char remote_trace_ip[128];
	uint32_t cellMask;
} TestmacCfg_s;

#endif