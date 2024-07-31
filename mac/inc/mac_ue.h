/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef _MAC_UE_H__
#define _MAC_UE_H__
#include "cmmTypes.h"

typedef enum {
    PUCCH_RPT_TYPE_FMT0,
    PUCCH_RPT_TYPE_FMT1,
    PUCCH_RPT_TYPE_FMT2,
    PUCCH_RPT_TYPE_FMT3,
    PUCCH_RPT_TYPE_FMT4,
    PUCCH_RPT_TYPE_NUM
} pucchRptType_e;

#define MAX_UE_NUM_IN_ONE_CELL 256

typedef struct {
    uint32_t cntPUCCH;
    uint16_t maxRSSIPUCCH;
    uint16_t minRSSIPUCCH;
    uint16_t avgRSSIPUCCH;
    uint16_t maxTaPUCCH;
    uint16_t minTaPUCCH;
    uint16_t avgTaPUCCH;
    uint16_t maxSnrPUCCH;
    uint16_t minSnrPUCCH;
    uint16_t avgSnrPUCCH;
} macStatsPUCCHRptUe_s;

typedef struct {
    uint32_t cntPUSCH;
    uint32_t cntUciInPUSCH;
    uint16_t maxRSSIPUSCH;
    uint16_t minRSSIPUSCH;
    uint16_t avgRSSIPUSCH;
    uint16_t maxTaPUSCH;
    uint16_t minTaPUSCH;
    uint16_t avgTaPUSCH;
    uint16_t maxSnrPUSCH;
    uint16_t minSnrPUSCH;
    uint16_t avgSnrPUSCH;
} macStatsPUSCHRptUe_s;

typedef struct {
    uint32_t cntSRDet;
    uint32_t cntSRMis;
} macStatsSRRptUe_s;

typedef struct {
    uint32_t cntHarqconfidenDet;
    uint32_t cntHarqconfidenMis;
} macStatsHarqconfidenRptUe_s;

typedef struct {
    uint8_t cellId;
    uint16_t ueId;
    uint16_t srnti;
    uint16_t crnti;
    struct {
        macStatsPUCCHRptUe_s rptPucch[PUCCH_RPT_TYPE_NUM];
        macStatsPUSCHRptUe_s rptPusch;
        macStatsSRRptUe_s rptSr;
        macStatsHarqconfidenRptUe_s rptHarqconfiden;
        uint32_t numHarqCrcPass;
        uint32_t numHarqCrcFail;
        uint32_t numACK;
        uint32_t numNACK;
        uint32_t crcPass;
        uint32_t crcFail;
        uint32_t numCsiPart1CrcPass;
        uint32_t numCsiPart1CrcFail;
        uint32_t numCsiPart2CrcPass;
        uint32_t numCsiPart2CrcFail;
    } ueStatsInfo;
} MacUeCtxt_s;

void macUeStatsResetAll();
void macUeStatsResetByCrnti(uint16_t cRnti);
MacUeCtxt_s* macGetUeByCrnti(uint16_t cRnti);

#endif
