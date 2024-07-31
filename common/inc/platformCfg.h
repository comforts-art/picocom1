/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

/**@brief Define the common structure and enum be used in all RRM module.\n The file should be used in RRM Algorithm, RRM Framework
*/
#ifndef __PLATFORMCFG_H_
#define __PLATFORMCFG_H_

#include "cmmMsgDesc.h"
#include "cmmTypes.h"

/**@brief Trace Data Type*/
typedef enum {
    TRACE_FALSE,
    TRACE_TERMINAL,
    TRACE_FILE,
    TRACE_UDP,
    TRACE_SYS_LOG,
    TRACETYPE_E_CNT
} TraceType_e;

/**@brief Trace data trace configuration,*/
typedef struct
    {
    TraceType_e type;
    /**Tgt Udp Port, use in TRACE_UDP*/
    uint16_t port;
    /**Tgt Ip Address or tgt file name*/
    char str[30];
} TraceCfg_s;

/**@brief Platform Configure*/
typedef struct
    {
    /**message between module tracing configuration. support TRACE_UDP or TRACE_FILE*/
    TraceCfg_s modMsgTrace;
    /**Message between PicoArray and ARM. Suppor TRACE_UDP*/
    TraceCfg_s phyTrace;
    /**Message of UU interface between UE and femto*/
    TraceCfg_s uuTrace;
    /**Debug configuration, Support TRACE_TERMINAL, TRACE_FILE, TRACE_SYS_LOG, TRACE_UDP*/
    TraceCfg_s debugCfg;
    /**For PHY Remote run mode. PC302 or PC3x3 Board IP, which a remote server running on*/
    char rmtSvrIp[30];
    /**Uu Measrument data monitor client host IP.*/
    TraceCfg_s monSvrIp;
    CmmBool_t dbgLevPresent;
    uint8_t dbgLev[32];
} PlatformCfg_s;

extern ContainerTypeDef_s TraceType_e_def;
#define ADDR_TRACETYPE_E_DEF (&TraceType_e_def)

extern ContainerTypeDef_s TraceCfg_s_def;
#define ADDR_TRACECFG_S_DEF (&TraceCfg_s_def)

extern ContainerTypeDef_s PlatformCfg_s_def;
#define ADDR_PLATFORMCFG_S_DEF (&PlatformCfg_s_def)

#endif