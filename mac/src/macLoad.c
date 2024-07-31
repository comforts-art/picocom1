/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include "macLoad.h"
#include "mac.h"

#include "cmm_zlog.h"

void testCae1()
{
    uint16_t sfn, slot;
    uint16_t i = 0;
    uint16_t txDataSlot = 0;
    uint16_t ulTtiSlot = 0;

    gL1ConfigRequest[0][0]->CarriConfig.dlFrequency = 2565690;
    gL1ConfigRequest[0][0]->CarriConfig.ulFrequency = 2565690;
    gL1ConfigRequest[0][0]->CarriConfig.dlBandwidth = 100;
    gL1ConfigRequest[0][0]->CarriConfig.ulBandwidth = 100;
    gL1ConfigRequest[0][0]->CarriConfig.dlGridSize[1] = 273;
    gL1ConfigRequest[0][0]->CarriConfig.ulGridSize[1] = 273;
    gL1ConfigRequest[0][0]->CarriConfig.numTxAnt = 1;
    gL1ConfigRequest[0][0]->CarriConfig.numRxAnt = 1;
    gL1ConfigRequest[0][0]->CarriConfig.FrequencyShift7p5KHz = 0;

    gL1ConfigRequest[0][0]->CellConfig.phyCellId = 1;
    gL1ConfigRequest[0][0]->CellConfig.FrameDuplexType = 1;

    gL1ConfigRequest[0][0]->SSBConfig.bchPayloadFlag = 1;
    gL1ConfigRequest[0][0]->SSBConfig.ssPbchBlockPowerScaling = 0;

    gL1ConfigRequest[0][0]->PRACHCfgInfo.numPrachConfigurations = 1;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].prachResConfigIndex = 0;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].prachSequenceLength = 1;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].prachSubCSpacing = 1;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].ulBwpPuschScs = 1;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].restrictedSetConfig = 0;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].numPrachFdOccasions = 1;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].prachConfigIndex = 159;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].SsbPerRach = 3;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].PrachFdOccaCfgInfos[0].prachRootSequenceIndex = 20;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].PrachFdOccaCfgInfos[0].prachFrequencyOffset = 0;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].PrachFdOccaCfgInfos[0].numUnusedRootSequences = 1;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].PrachFdOccaCfgInfos[0].numRootSequences = 6;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].PrachFdOccaCfgInfos[0].prachZeroCorrConf = 6;
    gL1ConfigRequest[0][0]->PRACHCfgInfo.PrachConfiguration[0].PrachFdOccaCfgInfos[0].unusedRootSequences[0] = 137;

    gL1ConfigRequest[0][0]->SSBtableInfo.Case = 2;
    gL1ConfigRequest[0][0]->SSBtableInfo.subcarrierSpacing = 1;
    gL1ConfigRequest[0][0]->SSBtableInfo.subCarrierSpacingCommon = 1;
    gL1ConfigRequest[0][0]->SSBtableInfo.lMax = 8;
    gL1ConfigRequest[0][0]->SSBtableInfo.rmsiPresence = 1;
    gL1ConfigRequest[0][0]->SSBtableInfo.SsbMask[0] = 4278190080;
    for (i = 0; i < 64; i++) {
        gL1ConfigRequest[0][0]->SSBtableInfo.BeamId[i] = i;
    }
    gL1ConfigRequest[0][0]->TDDtableInfo.TddPeriod = 6;

    gL1ConfigRequest[0][0]->MeasurementCfgInfo.RsrpMeasurement = 1;
    gL1ConfigRequest[0][0]->MeasurementCfgInfo.RssiMeasurement = 1;

    gL1ConfigRequest[0][0]->UciConfigurationInfo.numUci2Maps = 1;
    gL1ConfigRequest[0][0]->UciConfigurationInfo.mapIndex[0].numPart1Params = 1;
    for (i = 1; i < 4; i++)
        gL1ConfigRequest[0][0]->UciConfigurationInfo.mapIndex[0].sizesPart1Params[i] = 1;

    gL1ConfigRequest[0][0]->PrbSymol.numberOfPrbSymbRateMatchPatterns = 1;
    gL1ConfigRequest[0][0]->PrbSymol.numCoresetRmPatterns = 1;
    gL1ConfigRequest[0][0]->PrbSymol.CoresetRmPatterns[0].symbolsPattern = 3;
    gL1ConfigRequest[0][0]->PrbSymol.CoresetRmPatterns[0].freqDomainResources[0] = 240;
    gL1ConfigRequest[0][0]->PrbSymol.CoresetRmPatterns[0].coresetRmPatternID = 0;
    gL1ConfigRequest[0][0]->PrbSymol.PrbSymbRateMatchPatterns[0].prbSymbRateMatchPatternID = 2;
    gL1ConfigRequest[0][0]->PrbSymol.PrbSymbRateMatchPatterns[0].subcarrierSpacing = 1;
    gL1ConfigRequest[0][0]->PrbSymol.PrbSymbRateMatchPatterns[0].symbolsInRB = 3;

    gL1ConfigRequest[0][0]->PucchSSConfig.numUlBwpIds = 1;
    gL1ConfigRequest[0][0]->PdschConfig.pdschCbgScheme = 0;

    gL1ConfigRequest[0][0]->DBTInfo.numDigBeams = 1;
    gL1ConfigRequest[0][0]->DBTInfo.numBasebandPorts = 1;
    gL1ConfigRequest[0][0]->DBTInfo.digBeam[0].beamIdx = 2;
    gL1ConfigRequest[0][0]->DBTInfo.digBeam[0].TXRU[0].digBeamWeightIm = 2;
    gL1ConfigRequest[0][0]->DBTInfo.digBeam[0].TXRU[0].digBeamWeightRe = 2;

    gL1ConfigRequest[0][0]->PMInfo.numPMTables = 1;
    gL1ConfigRequest[0][0]->PMInfo.pmTable[0].PMidx = 0;
	gL1ConfigRequest[0][0]->PMInfo.pmTable[0].numLayers = 1;
    gL1ConfigRequest[0][0]->PMInfo.pmTable[0].numAntPorts = 4;
    gL1ConfigRequest[0][0]->PMInfo.pmTable[0].layers[0].precoderInfo[0].precoderWeightIm = 2;
    gL1ConfigRequest[0][0]->PMInfo.pmTable[0].layers[0].precoderInfo[0].precoderWeightRe = 2;

    for (sfn = 0; sfn < 3; sfn++) {
        for (slot = 0; slot < 19; slot++) {
            txDataSlot = slot % 10;
            ulTtiSlot = slot % 10;
            gL1DLTTIReq[0][0][sfn][slot]->SFN = sfn;
            gL1DLTTIReq[0][0][sfn][slot]->Slot = slot;
            gL1ULTTI[0][0][sfn][slot]->SFN = sfn;
            gL1ULTTI[0][0][sfn][slot]->Slot = slot;
            gL1TXDATAREQ[0][0][sfn][slot]->SFN = sfn;
            gL1TXDATAREQ[0][0][sfn][slot]->Slot = slot;
            if (slot / 10 == 0) {
                gL1DLTTIReq[0][0][sfn][slot]->nPDUs = 1;
                gL1DLTTIReq[0][0][sfn][slot]->nDlTypes = 5;
                gL1DLTTIReq[0][0][sfn][slot]->nGroup = 0;
                gL1DLTTIReq[0][0][sfn][slot]->nPDUsOfEachType[0] = 1;
                gL1DLTTIReq[0][0][sfn][slot]->nPDUsOfEachType[1] = 1;
                gL1DLTTIReq[0][0][sfn][slot]->nPDUsOfEachType[4] = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].PDUType = 3;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].PDUSize = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.SsbPdu.ssbConfigIndex = 0;
                if (slot == 0)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.SsbPdu.ssbBlockIndex = 0;
                else if (slot == 10)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.SsbPdu.ssbBlockIndex = 16;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.SsbPdu.bchPayloadFlag = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.SsbPdu.BchPayload.bchPayload = 65539;
                //gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.SsbPdu.BeamformingInfoOffset = 0;
            } else if ((slot % 10 == 7) || (slot % 10 == 8) || (slot % 10 == 9)) {
                gL1DLTTIReq[0][0][sfn][slot]->nPDUs = 0;
                gL1DLTTIReq[0][0][sfn][slot]->nDlTypes = 5;
                gL1DLTTIReq[0][0][sfn][slot]->nGroup = 0;
            } else if ((slot % 10 == 1) || (slot % 10 == 2) || (slot % 10 == 3) || (slot % 10 == 6)) {
                gL1DLTTIReq[0][0][sfn][slot]->nPDUs = 2;
                gL1DLTTIReq[0][0][sfn][slot]->nDlTypes = 5;
                gL1DLTTIReq[0][0][sfn][slot]->nGroup = 0;
                gL1DLTTIReq[0][0][sfn][slot]->nPDUsOfEachType[0] = 1;
                gL1DLTTIReq[0][0][sfn][slot]->nPDUsOfEachType[1] = 1;
                gL1DLTTIReq[0][0][sfn][slot]->nPDUsOfEachType[4] = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].PDUType = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].PDUSize = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.pdcchPduIndex = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.numDlDci = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.BWP.CoresetBWPSize = 273;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.BWP.CoresetBWPStart = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.BWP.SubcarrierSpacing = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.BWP.CyclicPrefix = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.ShiftIndex = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.StartSymbolIndex = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.DurationSymbols = 2;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.FreqDomainResource[1] = 255;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.FreqDomainResource[2] = 255;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.FreqDomainResource[3] = 255;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.CceRegMappingType = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.RegBundleSize = 2;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.InterleaverSize = 3;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.CoreSetType = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.precoderGranularity = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].RNTI = 12345;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].nIdPdcchData = 201;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].nRntiPdcchData = 12345;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].nIdPdcchDmrs = 60;
                if (slot == 1)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].CceIndex = 30;
                if (slot == 2)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].CceIndex = 35;
                if (slot == 3)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].CceIndex = 0;
                if (slot == 6)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].CceIndex = 24;
                if (slot == 11)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].CceIndex = 14;
                if (slot == 12)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].CceIndex = 4;
                if (slot == 13)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].CceIndex = 27;
                if (slot == 16)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].CceIndex = 46;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].AggregationLevel = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].PayloadSizeBits = 52;
                //gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].BeamformingInfoOffset = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].TxPowerInfo.pdcchDmrsPowerOffsetProfileSSS = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].TxPowerInfo.pdcchDataPowerOffsetProfileSSS = 0;
                if (slot == 1) {
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[0] = 194;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[1] = 32;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[2] = 7;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[3] = 176;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[4] = 24;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[5] = 54;
                }
                if (slot == 2) {
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[0] = 194;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[1] = 32;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[2] = 7;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[3] = 177;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[4] = 0;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[5] = 14;
                }
                if (slot == 3) {
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[0] = 194;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[1] = 32;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[2] = 7;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[3] = 176;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[4] = 144;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[5] = 46;
                }
                if (slot == 6) {
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[0] = 194;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[1] = 32;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[2] = 7;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[3] = 177;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[4] = 64;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[5] = 6;
                }
                if (slot == 11) {
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[0] = 194;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[1] = 32;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[2] = 7;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[3] = 184;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[4] = 24;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[5] = 54;
                }
                if (slot == 12) {
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[0] = 194;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[1] = 32;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[2] = 7;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[3] = 185;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[4] = 0;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[5] = 14;
                }
                if (slot == 13) {
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[0] = 194;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[1] = 32;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[2] = 7;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[3] = 184;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[4] = 144;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[5] = 46;
                }
                if (slot == 16) {
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[0] = 194;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[1] = 32;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[2] = 7;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[3] = 185;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[4] = 64;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[5] = 6;
                }

                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].PDUType = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].PDUSize = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.pduBitmap = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.RNTI = 12345;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.BWP.BWPSize = 273;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.BWP.BWPStart = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.BWP.SubcarrierSpacing = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.BWP.CyclicPrefix = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.NrOfCodewords = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].TBSize = 30215;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].tbSizeLbrmBytes = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].targetCodeRate = 9480;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].mcsIndex = 27;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].mcsTable = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].rvIndex = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].ldpcBaseGraph = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.nIdPdsch = 50;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.refPoint = 0;
                //gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.BeamformingInfoOffset = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.dlDmrsSymbPos = 2052;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.dlDmrsScramblingId = 300;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.dmrsPorts = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.maxLen = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.dmrsConfigType = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.nSCID = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.numDmrsCdmGrpsNoData = 2;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.rbStart = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.rbSize = 273;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.coresetStartPoint = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.initialDlBwpSize = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.resourceAlloc = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.VRBtoPRBMapping = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.pdschTransType = 0;
                for (i = 0; i < 36; i++)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.rbBitmap[i] = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.RateMatchingInformation.ssbConfigForRateMatching = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.RateMatchingInformation.numCoresetRMP = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.RateMatchingInformation.numCsiRsForRM = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.RateMatchingInformation.coresetRMPIndex[0] = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.RateMatchingInformation.csiRsForRM[0] = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocTd.StartSymbolIndex = 2;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocTd.NrOfSymbols = 12;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.TxPowerInfo.pdschDataPowerOffsetProfileSSS = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.TxPowerInfo.pdschDmrsPowerOffsetProfileSSS = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.PTRS.pdschPtrsPowerOffsetProfileSSS = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.PTRS.PTRSFreqDensity = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.PTRS.PTRSPortIndex = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.PTRS.PTRSReOffset = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.PTRS.PTRSTimeDensity = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CbgInfo.maxNumCbgPerTb = 0;
                for (i = 0; i < 8; i++)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CbgInfo.dlTbCrcCW[i] = 0;
            } else {
                gL1DLTTIReq[0][0][sfn][slot]->nPDUs = 2;
                gL1DLTTIReq[0][0][sfn][slot]->nDlTypes = 5;
                gL1DLTTIReq[0][0][sfn][slot]->nGroup = 0;
                gL1DLTTIReq[0][0][sfn][slot]->nPDUsOfEachType[0] = 1;
                gL1DLTTIReq[0][0][sfn][slot]->nPDUsOfEachType[1] = 1;
                gL1DLTTIReq[0][0][sfn][slot]->nPDUsOfEachType[4] = 2;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].PDUType = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].PDUSize = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.pdcchPduIndex = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.numDlDci = 2;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.BWP.CoresetBWPSize = 273;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.BWP.CoresetBWPStart = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.BWP.SubcarrierSpacing = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.BWP.CyclicPrefix = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.ShiftIndex = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.StartSymbolIndex = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.DurationSymbols = 2;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.FreqDomainResource[1] = 255;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.FreqDomainResource[2] = 255;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.FreqDomainResource[3] = 255;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.CceRegMappingType = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.RegBundleSize = 2;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.InterleaverSize = 3;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.CoreSetType = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.Coreset.precoderGranularity = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].RNTI = 12345;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].nIdPdcchData = 201;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].nRntiPdcchData = 12345;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].nIdPdcchDmrs = 60;
                if (slot == 4)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].CceIndex = 38;
                if (slot == 5)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].CceIndex = 7;
                if (slot == 14)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].CceIndex = 30;
                if (slot == 15)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].CceIndex = 31;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].AggregationLevel = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].PayloadSizeBits = 52;
                //gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].BeamformingInfoOffset = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].TxPowerInfo.pdcchDmrsPowerOffsetProfileSSS = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].TxPowerInfo.pdcchDataPowerOffsetProfileSSS = 0;
                if (slot == 4) {
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[0] = 194;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[1] = 32;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[2] = 7;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[3] = 177;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[4] = 128;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[5] = 46;
                }
                if (slot == 5) {
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[0] = 194;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[1] = 32;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[2] = 7;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[3] = 176;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[4] = 64;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[5] = 46;
                }
                if (slot == 14) {
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[0] = 194;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[1] = 32;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[2] = 7;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[3] = 185;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[4] = 128;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[5] = 46;
                }
                if (slot == 15) {
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[0] = 194;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[1] = 32;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[2] = 7;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[3] = 184;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[4] = 64;
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[0].Payload[5] = 46;
                }

                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].RNTI = 12345;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].nIdPdcchData = 201;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].nRntiPdcchData = 12345;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].nIdPdcchDmrs = 60;
                if (slot == 4)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].CceIndex = 44;
                if (slot == 5)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].CceIndex = 13;
                if (slot == 14)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].CceIndex = 36;
                if (slot == 15)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].CceIndex = 37;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].AggregationLevel = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].PayloadSizeBits = 47;
                //gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].BeamformingInfoOffset = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].TxPowerInfo.pdcchDmrsPowerOffsetProfileSSS = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].TxPowerInfo.pdcchDataPowerOffsetProfileSSS = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].Payload[0] = 67;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].Payload[1] = 184;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].Payload[2] = 67;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].Payload[3] = 128;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].Payload[4] = 80;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[0].DlPDUConfig.u.PdcchPdu.DlDCIs[1].Payload[5] = 66;

                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].PDUType = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].PDUSize = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.pduBitmap = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.RNTI = 12345;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.BWP.BWPSize = 273;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.BWP.BWPStart = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.BWP.SubcarrierSpacing = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.BWP.CyclicPrefix = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.NrOfCodewords = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].TBSize = 30215;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].tbSizeLbrmBytes = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].targetCodeRate = 9480;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].mcsIndex = 27;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].mcsTable = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].rvIndex = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CodewordInfo.CodewordInfo[0].ldpcBaseGraph = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.nIdPdsch = 50;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.refPoint = 0;
                //gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.BeamformingInfoOffset = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.dlDmrsSymbPos = 2052;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.dlDmrsScramblingId = 300;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.dmrsPorts = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.maxLen = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.dmrsConfigType = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.nSCID = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.DMRSInfo.numDmrsCdmGrpsNoData = 2;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.rbStart = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.rbSize = 273;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.coresetStartPoint = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.initialDlBwpSize = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.resourceAlloc = 1;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.VRBtoPRBMapping = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.pdschTransType = 0;
                for (i = 0; i < 36; i++)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocFd.rbBitmap[i] = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.RateMatchingInformation.ssbConfigForRateMatching = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.RateMatchingInformation.numCoresetRMP = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.RateMatchingInformation.numCsiRsForRM = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.RateMatchingInformation.coresetRMPIndex[0] = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.RateMatchingInformation.csiRsForRM[0] = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocTd.StartSymbolIndex = 2;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.ResourceAllocTd.NrOfSymbols = 12;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.TxPowerInfo.pdschDataPowerOffsetProfileSSS = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.TxPowerInfo.pdschDmrsPowerOffsetProfileSSS = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.PTRS.pdschPtrsPowerOffsetProfileSSS = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.PTRS.PTRSFreqDensity = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.PTRS.PTRSPortIndex = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.PTRS.PTRSReOffset = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.PTRS.PTRSTimeDensity = 0;
                gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CbgInfo.maxNumCbgPerTb = 0;
                for (i = 0; i < 8; i++)
                    gL1DLTTIReq[0][0][sfn][slot]->pdus[1].DlPDUConfig.u.PdschPdu.CbgInfo.dlTbCrcCW[i] = 0;
            }

            if ((txDataSlot == 1) || (txDataSlot == 2) || (txDataSlot == 3) || (txDataSlot == 4) || (txDataSlot == 5) || (txDataSlot == 6)) {
                gL1TXDATAREQ[0][0][sfn][slot]->numPDUs = 1;
                gL1TXDATAREQ[0][0][sfn][slot]->pdus[0].PDULength = 30215;
                gL1TXDATAREQ[0][0][sfn][slot]->pdus[0].PDUIndex = 0;
                gL1TXDATAREQ[0][0][sfn][slot]->pdus[0].CWIndex = 0;
                gL1TXDATAREQ[0][0][sfn][slot]->pdus[0].numTLV = 1;
                gL1TXDATAREQ[0][0][sfn][slot]->pdus[0].TLVs[0].Tag = 2;
                gL1TXDATAREQ[0][0][sfn][slot]->pdus[0].TLVs[0].Length = 30215;
                gL1TXDATAREQ[0][0][sfn][slot]->pdus[0].TLVs[0].Value = 0;
            } else {
                gL1TXDATAREQ[0][0][sfn][slot]->numPDUs = 0;
            }

            if ((ulTtiSlot == 8) || (ulTtiSlot == 9)) {
                gL1ULTTI[0][0][sfn][slot]->nPDUs = 3;
                gL1ULTTI[0][0][sfn][slot]->nUlTypes = 5;
                gL1ULTTI[0][0][sfn][slot]->nGroup = 1;
                gL1ULTTI[0][0][sfn][slot]->nPDUsOfEachType[1] = 1;
                gL1ULTTI[0][0][sfn][slot]->nPDUsOfEachType[2] = 1;
                gL1ULTTI[0][0][sfn][slot]->nPDUsOfEachType[3] = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].PDUType = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].PDUSize = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.Handle = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.pduBitmap = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.RNTI = 12345;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.tbSizeLbrmBytes = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.targetCodeRate = 6160;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.nIdPusch = 123;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.qamModOrder = 6;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.mcsIndex = 14;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.mcsTable = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.TransformPrecoding = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.nrOfLayers = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ldpcBaseGraph = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.BeamformingInfoOffset = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.BWP.BWPSize = 273;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.BWP.BWPStart = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.BWP.SubcarrierSpacing = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.BWP.CyclicPrefix = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.DMRS.ulDmrsSymbPos = 4;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.DMRS.puschDmrsScramblingId = 456;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.DMRS.puschDmrsIdentity = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.DMRS.dmrsPorts = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.DMRS.maxLen = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.DMRS.dmrsConfigType = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.DMRS.nSCID = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.DMRS.numDmrsCdmGrpsNoData = 2;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.DMRS.groupOrSequenceHopping = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocFd.rbStart = 12;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocFd.rbSize = 261;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocFd.deltaBwp0StartFromActiveBwp = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocFd.initialUlBwpSize = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocFd.puschSecondHopPRB = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocFd.txDirectCurrentLocation = 3300;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocFd.resourceAlloc = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocFd.VRBtoPRBMapping = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocFd.puschTransType = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocFd.IntraSlotFrequencyHopping = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocFd.uplinkFrequencyShift7p5khz = 1;
                for (i = 0; i < 36; i++)
                    gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocFd.rbBitmap[i] = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocTd.StartSymbolIndex = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.ResourceAllocTd.NrOfSymbols = 13;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.puschData.TBSize = 16912;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.puschData.numCb = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.puschData.rvIndex = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.puschData.harqProcessID = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.puschData.newData = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.puschData.cbCrcRequest = 0;
                for (i = 0; i < 128; i++)
                    gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.puschData.cbPresentAndPosition[i] = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.puschPtrs.numPtrsPorts = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.dftsOfdm.ulPtrsSampleDensity = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[0].UlPDUConfig.u.PuschPdu.dftsOfdm.ulPtrsTimeDensityTransformPrecoding = 1;

                gL1ULTTI[0][0][sfn][slot]->pdus[1].PDUType = 2;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].PDUSize = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.Handle = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.RNTI = 34567;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.FormatType = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.multiSlotTxIndicator = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.pi2Bpsk = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.harqBitLen = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.csiPart1BitLen = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.srBitLen = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.combineUciReport = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.BeamformingInfoOffset = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.BWP.BWPSize = 273;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.BWP.BWPStart = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.BWP.SubcarrierSpacing = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.BWP.CyclicPrefix = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.ResourceAllocFd.prbStart = 11;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.ResourceAllocFd.prbSize = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.ResourceAllocTd.StartSymbolIndex = 12;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.ResourceAllocTd.NrOfSymbols = 2;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.hoppingInfo.nIdPucchHopping = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.hoppingInfo.InitialCyclicShift = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.hoppingInfo.nIdPucchScrambling = 150;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.hoppingInfo.secondHopPRB = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.hoppingInfo.intraSlotFrequencyHopping = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.hoppingInfo.preDftOccLen = 2;
                gL1ULTTI[0][0][sfn][slot]->pdus[1].UlPDUConfig.u.PucchPdu.DMRS.nId0PucchDmrsScrambling = 777;

                gL1ULTTI[0][0][sfn][slot]->pdus[2].PDUType = 2;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].PDUSize = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.Handle = 2;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.RNTI = 34568;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.FormatType = 2;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.multiSlotTxIndicator = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.pi2Bpsk = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.harqBitLen = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.csiPart1BitLen = 100;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.srBitLen = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.combineUciReport = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.BeamformingInfoOffset = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.BWP.BWPSize = 273;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.BWP.BWPStart = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.BWP.SubcarrierSpacing = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.BWP.CyclicPrefix = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.ResourceAllocFd.prbStart = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.ResourceAllocFd.prbSize = 10;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.ResourceAllocTd.StartSymbolIndex = 12;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.ResourceAllocTd.NrOfSymbols = 2;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.hoppingInfo.nIdPucchHopping = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.hoppingInfo.InitialCyclicShift = 1;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.hoppingInfo.nIdPucchScrambling = 150;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.hoppingInfo.secondHopPRB = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.hoppingInfo.intraSlotFrequencyHopping = 0;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.hoppingInfo.preDftOccLen = 2;
                gL1ULTTI[0][0][sfn][slot]->pdus[2].UlPDUConfig.u.PucchPdu.DMRS.nId0PucchDmrsScrambling = 778;
            } else {
                gL1ULTTI[0][0][sfn][slot]->nPDUs = 0;
                gL1ULTTI[0][0][sfn][slot]->nUlTypes = 5;
                gL1ULTTI[0][0][sfn][slot]->nGroup = 0;
            }
        }
    }
}
