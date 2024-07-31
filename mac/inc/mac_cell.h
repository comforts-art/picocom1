/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef _MAC_CELL_H__
#define _MAC_CELL_H__
#include "cmmTypes.h"

#define MAX_NUM_PRACH_PREAMPLES 64

typedef struct {
    uint32_t printPeriod;
    CmmBool_t enablePuschPrint;
    CmmBool_t enablePrachPrint;
    CmmBool_t enablePucchPrint;
    CmmBool_t enableSrPrint;
} macStatsConfig_s;

typedef struct {
    uint32_t prachPreambleID;
    uint32_t totalNumTa;
    uint16_t taMaxPRACH;
    uint16_t taMinPRACH;
    uint32_t taAvgPRACH;
    uint32_t totalNumRssi;
    uint16_t rssiMaxPRACH;
    uint16_t rssiMinPRACH;
    uint16_t rssiAvgPRACH;
    uint16_t rev1MaxPRACH[MAX_NUM_PRACH_PREAMPLES];
    uint16_t rev1MinPRACH[MAX_NUM_PRACH_PREAMPLES];
    uint16_t rev1AvgPRACH[MAX_NUM_PRACH_PREAMPLES];
    uint16_t preambleCntPRACH[MAX_NUM_PRACH_PREAMPLES];
} macStatsRach_s;

typedef struct {
    uint32_t totalNumPucchCrc;
    uint32_t numPucchCrcPass;
    uint32_t numPucchCrcFail;
	uint32_t numPucchUciDetected;
	uint32_t numPucchUciNotDetected;
    uint16_t rssiAvgPUCCH;
    uint16_t rssiMaxPUCCH;
    uint16_t rssiMinPUCCH;
    uint16_t sumRssiPUCCH;
    uint16_t snrMaxPUCCH;
    uint16_t snrMinPUCCH;
    uint16_t snrAvgPUCCH;
    uint16_t sumSnrPUCCH;
    uint16_t taMaxPUCCH;
    uint16_t taMinPUCCH;
    uint16_t taAvgPUCCH;
    uint16_t sumTaPUCCH;
    uint32_t cntSRDetPUCCH;
    uint32_t cntSRMisPUCCH;
    uint32_t sumSrPUCCH;
    uint32_t cntHarqconfidenceDetPUCCH;
    uint32_t cntHarqconfidenceMisPUCCH;
    uint32_t sumHarqconfidencePUCCH;
} macStatsPucch_s;

typedef struct {

    uint32_t totalNumPuschCrc;
    int32_t numPuschCrcPass;
    int32_t numPuschCrcFail;

    uint32_t totalNumPUSCH;
    uint16_t snrMax;
    uint16_t snrMin;
    uint16_t snrAvg;
    uint32_t sumSnrPUSCH;
    uint16_t rssiMax;
    uint16_t rssiMin;
    uint16_t rssiAvg;
    int32_t sumRssiPUSCH;
    uint16_t taMax;
    uint16_t taMin;
    uint16_t taAvg;
    int32_t sumTaPUSCH;
    int32_t sumFailTaPUSCH;
} macStatsPusch_s;

typedef struct {
    struct {
        macStatsRach_s rachStatsInfo;
        macStatsPucch_s pucchStatsInfo;
        macStatsPusch_s puschStatsInfo;
    } cellStatsInfo;
} macCellCtx_s;

macCellCtx_s* macGetCellById(uint16_t dev_index, uint16_t cellId);

void macCellInit();

void macCellStatsReset();

void macDumpCellUlInfo(uint16_t dev_index, uint16_t cellid);

void macCellSetScs(uint16_t i, uint16_t dev_index, uint16_t cellId);

uint32_t macGetSlotNumPerSfn(uint16_t dev_index, uint16_t cellid);

#endif
