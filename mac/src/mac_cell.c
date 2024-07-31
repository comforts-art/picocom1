/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include "mac_cell.h"
#include "cmm_zlog.h"
#include "mac.h"
#include "mac_ue.h"
//#define MAC_MAX_CELL_NUM 2
static macCellCtx_s sMacCellCtx[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM];
uint32_t gcurrentScs[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM];

void macCellStatsReset()
{
    int i, j;
    for (j = 0; j < MAC_PC802_INDEX_MAX; j++) {
        for (i = 0; i < MAC_MAX_CELL_NUM; i++)
            memset(&sMacCellCtx[j][i].cellStatsInfo, 0, sizeof(sMacCellCtx[j][i].cellStatsInfo));
    }
}

void macCellInit()
{
    macCellStatsReset();
    macUeStatsResetAll();
}

macCellCtx_s* macGetCellById(uint16_t dev_index, uint16_t cellId)
{
    return &sMacCellCtx[dev_index][cellId & 1];
}

void macCellSetScs(uint16_t i, uint16_t dev_index, uint16_t cellId)
{
    printf("macCellSetScs, cellid = %d scs = %d", cellId, i);
    gcurrentScs[dev_index][cellId] = i;
    if (i == 15) {
        maxTimeOut = 2 * maxTimeOut;
        minTimeOut = 2 * minTimeOut;
        printf("scs_15k, MaxTime[%d], MinTime[%d]", maxTimeOut, minTimeOut);
    }

    return;
}

uint32_t macGetSlotNumPerSfn(uint16_t dev_index, uint16_t cellid)
{

    switch (gcurrentScs[dev_index][cellid]) {
    case 15:
        return 10;
    case 30:
        return 20;
    case 60:
        return 40;
    case 120:
        return 80;
    default:
        printf("invalid scs:%d cellid:%d \n", gcurrentScs[dev_index][cellid], cellid);
    }

    return 0;
}
void macDumpCellUlInfo(uint16_t dev_index, uint16_t cellid)
{
    macCellCtx_s* cellCtx = NULL;
    cellCtx = macGetCellById(dev_index, cellid);
    CMM_NOTICE(LOG_MAC, "dev_index[%d]cellid[%d]", dev_index, cellid);
    CMM_NOTICE(LOG_MAC, "TbCrcPass[%d], TbCrcFail[%d],PucchCrcPass[%d], PucchCrcFail[%d]",
        cellCtx->cellStatsInfo.puschStatsInfo.numPuschCrcPass, cellCtx->cellStatsInfo.puschStatsInfo.numPuschCrcFail, cellCtx->cellStatsInfo.pucchStatsInfo.numPucchCrcPass, cellCtx->cellStatsInfo.pucchStatsInfo.numPucchCrcFail);
    if (cellCtx->cellStatsInfo.puschStatsInfo.numPuschCrcPass != 0)
        CMM_NOTICE(LOG_MAC, "Ta[%d], Rssi[%d], Snr[%d]\n", (cellCtx->cellStatsInfo.puschStatsInfo.sumTaPUSCH / cellCtx->cellStatsInfo.puschStatsInfo.numPuschCrcPass), ((int32_t)(cellCtx->cellStatsInfo.puschStatsInfo.sumRssiPUSCH / cellCtx->cellStatsInfo.puschStatsInfo.numPuschCrcPass)), ((cellCtx->cellStatsInfo.puschStatsInfo.sumSnrPUSCH / cellCtx->cellStatsInfo.puschStatsInfo.numPuschCrcPass) / 2) - 64);
    if (cellCtx->cellStatsInfo.puschStatsInfo.numPuschCrcFail != 0)
        CMM_NOTICE(LOG_MAC, "Fail Ta[%d]\n", (cellCtx->cellStatsInfo.puschStatsInfo.sumFailTaPUSCH / cellCtx->cellStatsInfo.puschStatsInfo.numPuschCrcFail));
    if (cellCtx->cellStatsInfo.rachStatsInfo.totalNumRssi != 0)
        CMM_NOTICE(LOG_MAC, "MaxRssi-[%d] MinRssi-[%d] AvgRssi[%d]\n", cellCtx->cellStatsInfo.rachStatsInfo.rssiMaxPRACH, cellCtx->cellStatsInfo.rachStatsInfo.rssiMinPRACH, (cellCtx->cellStatsInfo.rachStatsInfo.rssiAvgPRACH / cellCtx->cellStatsInfo.rachStatsInfo.totalNumRssi));
    if (cellCtx->cellStatsInfo.rachStatsInfo.totalNumTa != 0)
        CMM_NOTICE(LOG_MAC, "PreambleId-[%d] MaxTA-[%d] MinTA-[%d] AvgTA[%d]\n", cellCtx->cellStatsInfo.rachStatsInfo.prachPreambleID, cellCtx->cellStatsInfo.rachStatsInfo.taMaxPRACH, cellCtx->cellStatsInfo.rachStatsInfo.taMinPRACH, (cellCtx->cellStatsInfo.rachStatsInfo.taAvgPRACH / cellCtx->cellStatsInfo.rachStatsInfo.totalNumTa));

    cellCtx->cellStatsInfo.puschStatsInfo.sumTaPUSCH = 0;
    cellCtx->cellStatsInfo.puschStatsInfo.sumRssiPUSCH = 0;
    cellCtx->cellStatsInfo.puschStatsInfo.sumSnrPUSCH = 0;
    cellCtx->cellStatsInfo.puschStatsInfo.sumFailTaPUSCH = 0;
    cellCtx->cellStatsInfo.puschStatsInfo.numPuschCrcPass = 0;
    cellCtx->cellStatsInfo.puschStatsInfo.numPuschCrcFail = 0;
    cellCtx->cellStatsInfo.pucchStatsInfo.numPucchCrcPass = 0;
    cellCtx->cellStatsInfo.pucchStatsInfo.numPucchCrcFail = 0;
    cellCtx->cellStatsInfo.rachStatsInfo.totalNumRssi = 0;
    cellCtx->cellStatsInfo.rachStatsInfo.rssiMaxPRACH = 0;
    cellCtx->cellStatsInfo.rachStatsInfo.rssiMinPRACH = 0;
    cellCtx->cellStatsInfo.rachStatsInfo.rssiAvgPRACH = 0;
    cellCtx->cellStatsInfo.rachStatsInfo.totalNumTa = 0;
    cellCtx->cellStatsInfo.rachStatsInfo.prachPreambleID = 0;
    cellCtx->cellStatsInfo.rachStatsInfo.taMaxPRACH = 0;
    cellCtx->cellStatsInfo.rachStatsInfo.taMinPRACH = 0;
    cellCtx->cellStatsInfo.rachStatsInfo.taAvgPRACH = 0;
	cellCtx->cellStatsInfo.pucchStatsInfo.numPucchUciDetected=0;
	cellCtx->cellStatsInfo.pucchStatsInfo.numPucchUciNotDetected=0;
}
