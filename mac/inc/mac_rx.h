/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __MAC_RX_H__
#define __MAC_RX_H__
#include "cmmTypes.h"
#include "l1_api.h"
#include "mac.h"

uint32_t fapiMsgDec(uint32_t* l1MessageInput, uint16_t dev_index, uint16_t cellid);

#endif
