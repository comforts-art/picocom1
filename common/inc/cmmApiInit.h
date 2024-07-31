/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __CMM_API_INIT_H__
#define __CMM_API_INIT_H__
#include "platformCfg.h"

extern PlatformCfg_s platformCfg;

/**
 \brife Common Module Init

 Read configuration file(platformCfg.xml) to platformCfg.
 This function should be called before all other module init.
*/

void cmmApiInit(char* ipAddress);

#endif
