/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include "mac_ue.h"
static MacUeCtxt_s sMacUeCtxt[MAX_UE_NUM_IN_ONE_CELL] = {};

void macUeStatsResetAll()
{
    int i = 0;
    for (i = 0; i < MAX_UE_NUM_IN_ONE_CELL; i++) {
        memset(&sMacUeCtxt[i].ueStatsInfo, 0, sizeof(sMacUeCtxt[i].ueStatsInfo));
    }
}

void macUeStatsResetByCrnti(uint16_t cRnti)
{
    ASSERT((cRnti > 4095) & (cRnti < (MAX_UE_NUM_IN_ONE_CELL + 4095)))
    memset(&sMacUeCtxt[cRnti & 4095].ueStatsInfo, 0, sizeof(sMacUeCtxt[cRnti & 4095].ueStatsInfo));
}

MacUeCtxt_s* macGetUeByCrnti(uint16_t cRnti)
{
    return &sMacUeCtxt[cRnti & 4095];
}
