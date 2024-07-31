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
#include "platformCfg.h"

/**Trace Data Type*/
ContainerTypeDef_s TraceType_e_def = {
    "TraceType_e",
    T_ENUM, sizeof(TraceType_e), T_INT, NULL,
    6,
    { { "TRACE_FALSE", T_UINT, NULL, 0, VAR_DATA, 0 },
      { "TRACE_TERMINAL", T_UINT, NULL, 0, VAR_DATA, 0 },
      { "TRACE_FILE", T_UINT, NULL, 0, VAR_DATA, 0 },
      { "TRACE_UDP", T_UINT, NULL, 0, VAR_DATA, 0 },
      { "TRACE_SYS_LOG", T_UINT, NULL, 0, VAR_DATA, 0 },
      { "TraceType_e_cnt", T_UINT, NULL, 0, VAR_DATA, 0 } }
};

/**Trace data trace configuration,*/
#ifndef ADDR_TRACETYPE_E_DEF
#define ADDR_TRACETYPE_E_DEF NULL
#endif

ContainerTypeDef_s TraceCfg_s_def = {
    "TraceCfg_s", T_STRUCT, sizeof(TraceCfg_s), T_TYPE, NULL, 3,
    { { "type", T_TYPE, ADDR_TRACETYPE_E_DEF, offsetof(TraceCfg_s, type), VAR_DATA, 0 },
      { "port", T_U16, NULL, offsetof(TraceCfg_s, port), VAR_DATA, 0 },
      { "str", T_CHAR, NULL, offsetof(TraceCfg_s, str), VAR_ARRAY_F, 30 } }
};

/**Platform Configure*/
#ifndef ADDR_TRACECFG_S_DEF
#define ADDR_TRACECFG_S_DEF NULL
#endif

#ifndef ADDR_TRACECFG_S_DEF
#define ADDR_TRACECFG_S_DEF NULL
#endif

#ifndef ADDR_TRACECFG_S_DEF
#define ADDR_TRACECFG_S_DEF NULL
#endif

#ifndef ADDR_TRACECFG_S_DEF
#define ADDR_TRACECFG_S_DEF NULL
#endif

#ifndef ADDR_TRACECFG_S_DEF
#define ADDR_TRACECFG_S_DEF NULL
#endif

ContainerTypeDef_s PlatformCfg_s_def = {
    "PlatformCfg_s", T_STRUCT, sizeof(PlatformCfg_s), T_TYPE, NULL, 8,
    { { "modMsgTrace", T_TYPE, ADDR_TRACECFG_S_DEF, offsetof(PlatformCfg_s, modMsgTrace), VAR_DATA, 0 },
      { "phyTrace", T_TYPE, ADDR_TRACECFG_S_DEF, offsetof(PlatformCfg_s, phyTrace), VAR_DATA, 0 },
      { "uuTrace", T_TYPE, ADDR_TRACECFG_S_DEF, offsetof(PlatformCfg_s, uuTrace), VAR_DATA, 0 },
      { "debugCfg", T_TYPE, ADDR_TRACECFG_S_DEF, offsetof(PlatformCfg_s, debugCfg), VAR_DATA, 0 },
      { "rmtSvrIp", T_CHAR, NULL, offsetof(PlatformCfg_s, rmtSvrIp), VAR_ARRAY_F, 30 },
      { "monSvrIp", T_TYPE, ADDR_TRACECFG_S_DEF, offsetof(PlatformCfg_s, monSvrIp), VAR_DATA, 0 },
      { "dbgLevPresent", T_BOOL, NULL, offsetof(PlatformCfg_s, dbgLevPresent), VAR_DATA, 0 },
      { "dbgLev", T_U8, NULL, offsetof(PlatformCfg_s, dbgLev), VAR_ARRAY_F, 32 } }
};
