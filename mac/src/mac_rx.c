/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include "mac_rx.h"
#include "cmm_zlog.h"
#include "l1_api.h"
#include "mac.h"
#include "macTx.h"
#include "mac_cell.h"
#include "mac_ue.h"
#include "pcxx_ipc.h"

#ifdef ATLL_PCIE
#include "rte_pmd_pc802.h"
#include <rte_cycles.h>
#endif

void macCSIPart1InfoUciInPuschPduFmt234PduHdlr(uint16_t cRnti, uint32_t handle, uint16_t numUci, L1CSIPart1Pdu_t* cSIPart1Pdu, uint16_t* TotalPuschCsiPart1Crc, uint8_t dev_index, uint8_t cellId)
{
    ASSERT(cSIPart1Pdu);
    ASSERT(handle < 65536);
    MacUeCtxt_s* ueCtx = NULL;
    ueCtx = macGetUeByCrnti(cRnti);
    ASSERT(ueCtx);
    if (0 == cSIPart1Pdu->CsiPart1Crc) {
        ueCtx->ueStatsInfo.numCsiPart1CrcPass++;
        CMM_DEBUG(LOG_MAC, "NUM[%d]: PuschCsiPart1Crc pass, have %d PuschCsiPart1Crc passed!", numUci, ueCtx->ueStatsInfo.numCsiPart1CrcPass);
    } else if (1 == cSIPart1Pdu->CsiPart1Crc) {
        ueCtx->ueStatsInfo.numCsiPart1CrcFail++;
        CMM_DEBUG(LOG_MAC, "NUM[%d]: PuschCsiPart1Crc fail, have %d PuschCsiPart1Crc failed!", numUci, ueCtx->ueStatsInfo.numCsiPart1CrcFail);
    }
    *TotalPuschCsiPart1Crc += ueCtx->ueStatsInfo.numCsiPart1CrcPass;
    return;
}

void macCSIPart1InfoUciInPucchPduFmt234PduHdlr(uint16_t cRnti, uint32_t handle, uint16_t numUci, L1CSIPart1Pdu_t* cSIPart1Pdu, uint16_t* TotalPucchCsiPart1Crc, uint8_t dev_index, uint8_t cellId)
{
    ASSERT(cSIPart1Pdu);
    ASSERT(handle < 65536);
    macCellCtx_s* cellCtx = NULL;
    cellCtx = macGetCellById(dev_index, cellId);
    MacUeCtxt_s* ueCtx = NULL;
    ueCtx = macGetUeByCrnti(cRnti);
    ASSERT(ueCtx);
    if (0 == cSIPart1Pdu->CsiPart1Crc) {
        ueCtx->ueStatsInfo.numCsiPart1CrcPass++;
        cellCtx->cellStatsInfo.pucchStatsInfo.numPucchCrcPass++;
        CMM_DEBUG(LOG_MAC, "NUM[%d]: PucchCsiPart1Crc pass, have %d PucchCsiPart1Crc passed!", numUci, ueCtx->ueStatsInfo.numCsiPart1CrcPass);
    } else if (1 == cSIPart1Pdu->CsiPart1Crc) {
        ueCtx->ueStatsInfo.numCsiPart1CrcFail++;
        CMM_DEBUG(LOG_MAC, "NUM[%d]: PucchCsiPart1Crc fail, have %d PucchCsiPart1Crc failed!", numUci, ueCtx->ueStatsInfo.numCsiPart1CrcFail);
    }
    *TotalPucchCsiPart1Crc += ueCtx->ueStatsInfo.numCsiPart1CrcPass;
    return;

} // CSIPart1InfoUciInPucchPduFmt234PduHdlr()

void macUciInfoInPucchPduFmt234PduHdlr(uint16_t cRnti, uint32_t handle, uint16_t numUci, L1UciInfoPdu_t* uciInfoPdu, uint16_t* TotalPucchCsiPart1Crc, uint8_t dev_index, uint8_t cellId)
{
    ASSERT(uciInfoPdu);
    ASSERT(handle < 65536);
    macCellCtx_s* cellCtx = NULL;
    cellCtx = macGetCellById(dev_index, cellId);
    MacUeCtxt_s* ueCtx = NULL;
    ueCtx = macGetUeByCrnti(cRnti);
    ASSERT(ueCtx);
    if (0 == uciInfoPdu->CrcStatus) {
        cellCtx->cellStatsInfo.pucchStatsInfo.numPucchCrcPass++;
        CMM_DEBUG(LOG_MAC, "NUM[%d]: pucch crc pass, have %d numPucchCrcPass passed!", numUci, cellCtx->cellStatsInfo.pucchStatsInfo.numPucchCrcPass);
    } else if (1 == uciInfoPdu->CrcStatus) {
        cellCtx->cellStatsInfo.pucchStatsInfo.numPucchCrcFail++;
        CMM_DEBUG(LOG_MAC, "NUM[%d]: pucch crc fail, have %d numPucchCrcFail failed!", numUci, cellCtx->cellStatsInfo.pucchStatsInfo.numPucchCrcFail);
    }else if (2 == uciInfoPdu->CrcStatus) {
        cellCtx->cellStatsInfo.pucchStatsInfo.numPucchUciDetected++;
        CMM_DEBUG(LOG_MAC, "NUM[%d]: pucch uci detected, have %d numPucchCrcFail failed!", numUci, cellCtx->cellStatsInfo.pucchStatsInfo.numPucchUciDetected);
    }else if (3 == uciInfoPdu->CrcStatus) {
        cellCtx->cellStatsInfo.pucchStatsInfo.numPucchUciNotDetected++;
        CMM_DEBUG(LOG_MAC, "NUM[%d]: pucch uci not detected, have %d numPucchCrcFail failed!", numUci, cellCtx->cellStatsInfo.pucchStatsInfo.numPucchUciNotDetected);
    }

    return;
}

void rxDataIndicationProcDecHdlr(const char* pMsg, uint8_t dev_index, uint8_t cellId)
{
    uint32_t offset = 0;
    uint16_t j = 0;
    uint16_t i = 0;
    void* buf;
    L1RxdataIndication_t* rxDataIndication = (L1RxdataIndication_t*)pMsg;

    offset += offsetof(L1RxdataIndication_t, pdus);
    L1RxDataPduInfo_t* rxdataPdu;

    for (j = 0; j < rxDataIndication->numPDUs; j++) {
        rxdataPdu = (L1RxDataPduInfo_t*)((char*)pMsg + offset);
        offset += offsetof(L1RxDataPduInfo_t, TLVs);
        for (i = 0; i < rxdataPdu->numTLV; i++) {
            L1DataTLV_t* DataTLV = (L1DataTLV_t*)((char*)pMsg + offset);
            if (DataTLV->Tag == 2) {
#ifndef MAC_MULTI_PC802
                buf = (void*)pcxxDataRecv(DataTLV->Value, DataTLV->Length);
#else
                buf = (void*)pcxxDataRecv(DataTLV->Value, DataTLV->Length, dev_index, cellId);
#endif
            }
            offset += sizeof(L1DataTLV_t);
        }
    }
    return;
} // rxDataIndicationProcDecHdlr()

void slotIndicationProcDecHdlr(const char* pMsg, uint8_t dev_index, uint8_t cellId)
{
    L1SlotIndication_t* slotIndication = (L1SlotIndication_t*)pMsg;

    macTxStartIndHandle(slotIndication, dev_index, cellId);
    return;
} // slotIndicationProcDecHdlr()

void configResponseProcDecHdlr(const char* pMsg, uint8_t dev_index, uint8_t cellId)
{
    L1ConfigResponse_t* configResponse = (L1ConfigResponse_t*)pMsg;

    macRevCfgResonse(configResponse, dev_index, cellId);
    return;
} // configResponseProcDecHdlr()

void errorIndicationProcDecHdlr(const char* pMsg, uint8_t dev_index, uint8_t cellId)
{
    L1ErrorIndication_t* errorIndication = (L1ErrorIndication_t*)pMsg;

    macRevErrorIndication(errorIndication, dev_index, cellId);
    return;
} // errorIndicationProcDecHdlr()

void macCrcInHdlr(L1CrcInfo_t* crcInfo, uint16_t numCrc, uint16_t* CrcTotal, uint8_t dev_index, uint8_t cellId)
{
    ASSERT(crcInfo);
    macCellCtx_s* cellCtx = NULL;
    cellCtx = macGetCellById(dev_index, cellId);
    static int i = 0;

    if (0 == crcInfo->TbCrcStatus) {
        cellCtx->cellStatsInfo.puschStatsInfo.numPuschCrcPass++;
        cellCtx->cellStatsInfo.puschStatsInfo.sumTaPUSCH += crcInfo->TA;
        cellCtx->cellStatsInfo.puschStatsInfo.sumRssiPUSCH += (((int32_t)(crcInfo->RSSI - 1280)) / 10);
        cellCtx->cellStatsInfo.puschStatsInfo.sumSnrPUSCH += crcInfo->SINR;
    } else if (1 == crcInfo->TbCrcStatus) {
        cellCtx->cellStatsInfo.puschStatsInfo.sumFailTaPUSCH += crcInfo->TA;
        cellCtx->cellStatsInfo.puschStatsInfo.numPuschCrcFail++;
    }

    return;
}

void crcIndicationProcDecHdlr(const char* pMsg, uint8_t dev_index, uint8_t cellId)
{
    uint16_t j;
    uint16_t crcTotal = 0;
    uint32_t offset = 0;
    L1CrcIndication_t* crcIndication = (L1CrcIndication_t*)pMsg;
    offset += offsetof(L1CrcIndication_t, CRCs);

    L1CrcInfo_t* crcInfo;
    for (j = 0; j < crcIndication->numCRCs; j++) {
        crcInfo = (L1CrcInfo_t*)((char*)pMsg + offset);
        macCrcInHdlr(crcInfo, j, &crcTotal, dev_index, cellId);
        offset += sizeof(L1CrcInfo_t);
    }
    CMM_DEBUG(LOG_MAC, "Total:TbCrc pass, have %d TbCrc passed!", crcTotal);
    return;
} // crcIndicationProcDecHdlr()

void srsIndicationProcDecHdlr(const char* pMsg, uint8_t dev_index, uint8_t cellId)
{
    uint16_t j, i;
    uint32_t offset = 0;
    L1SrsIndication_t* srsIndication = (L1SrsIndication_t*)pMsg;
    offset += offsetof(L1SrsIndication_t, pdus);

    for (j = 0; j < srsIndication->numPDUs; j++) {
        L1SrsPduInfo_t* SrsPduInfo = (L1SrsPduInfo_t*)((char*)pMsg + offset);
        offset += offsetof(L1SrsPduInfo_t, reportTLV);
#if 0
        for (i = 0; i < SrsPduInfo->numReportedSymbols; i++) {
            L1SrsSymbolInfo_t* SrsSymbolInfo = (L1SrsSymbolInfo_t*)((char*)pMsg + offset);
            offset += sizeof(L1SrsSymbolInfo_t);
        }
#endif
    }

    return;
} // srsIndicationProcDecHdlr()

void macRachPduInHdlr(L1RachPduInfo_t* rachPduInfo, uint8_t dev_index, uint8_t cellId)
{
    ASSERT(rachPduInfo);
    macCellCtx_s* cellCtx = NULL;
    cellCtx = macGetCellById(dev_index, cellId);
    static int i = 0;
    uint16_t Rssi = rachPduInfo->avgRssi;

    if (cellCtx->cellStatsInfo.rachStatsInfo.totalNumRssi == 0) {
        cellCtx->cellStatsInfo.rachStatsInfo.totalNumRssi++;
        cellCtx->cellStatsInfo.rachStatsInfo.rssiMaxPRACH = Rssi;
        cellCtx->cellStatsInfo.rachStatsInfo.rssiMinPRACH = Rssi;
        cellCtx->cellStatsInfo.rachStatsInfo.rssiAvgPRACH += Rssi;
    } else {
        cellCtx->cellStatsInfo.rachStatsInfo.totalNumRssi++;
        if (Rssi > cellCtx->cellStatsInfo.rachStatsInfo.taMaxPRACH)
            cellCtx->cellStatsInfo.rachStatsInfo.rssiMinPRACH = Rssi;
        if (Rssi < cellCtx->cellStatsInfo.rachStatsInfo.rssiMaxPRACH)
            cellCtx->cellStatsInfo.rachStatsInfo.rssiMaxPRACH = Rssi;
        cellCtx->cellStatsInfo.rachStatsInfo.rssiAvgPRACH += Rssi;
    }

    return;
}

void macPreambleInHdlr(L1PreambleInfo_t* preambleInfo, uint8_t dev_index, uint8_t cellId)
{
    ASSERT(preambleInfo);
    macCellCtx_s* cellCtx = NULL;
    cellCtx = macGetCellById(dev_index, cellId);
    static int i = 0;
    uint16_t TA = preambleInfo->TA;

    cellCtx->cellStatsInfo.rachStatsInfo.prachPreambleID = preambleInfo->preambleIndex;
    if (cellCtx->cellStatsInfo.rachStatsInfo.totalNumTa == 0) {
        cellCtx->cellStatsInfo.rachStatsInfo.totalNumTa++;
        cellCtx->cellStatsInfo.rachStatsInfo.taMaxPRACH = TA;
        cellCtx->cellStatsInfo.rachStatsInfo.taMinPRACH = TA;
        cellCtx->cellStatsInfo.rachStatsInfo.taAvgPRACH += TA;
    } else {
        cellCtx->cellStatsInfo.rachStatsInfo.totalNumTa++;
        if (TA > cellCtx->cellStatsInfo.rachStatsInfo.taMaxPRACH)
            cellCtx->cellStatsInfo.rachStatsInfo.taMinPRACH = TA;
        if (TA < cellCtx->cellStatsInfo.rachStatsInfo.taMaxPRACH)
            cellCtx->cellStatsInfo.rachStatsInfo.taMaxPRACH = TA;
        cellCtx->cellStatsInfo.rachStatsInfo.taAvgPRACH += TA;
    }

    return;
}

void rachIndicationProcDecHdlr(const char* pMsg, uint8_t dev_index, uint8_t cellId)
{
    uint16_t j, i;
    uint32_t offset = 0;
    L1RachIndication_t* rachIndication = (L1RachIndication_t*)pMsg;
    offset += offsetof(L1RachIndication_t, pdus);

    for (j = 0; j < rachIndication->numPDUs; j++) {
        L1RachPduInfo_t* RachPduInfo = (L1RachPduInfo_t*)((char*)pMsg + offset);
        offset += offsetof(L1RachPduInfo_t, preamble);
        macRachPduInHdlr(RachPduInfo, dev_index, cellId);
        for (i = 0; i < RachPduInfo->numPreambles; i++) {
            L1PreambleInfo_t* rps = (L1PreambleInfo_t*)((char*)pMsg + offset);
            macPreambleInHdlr(rps, dev_index, cellId);
            offset += sizeof(L1PreambleInfo_t);
        }
    }

    return;
} // rachIndicationProcDecHdlr()

void uciIndicationProcDecHdlr(const char* pMsg, uint8_t dev_index, uint8_t cellId)
{
    uint16_t j;
    uint32_t offset = 0;
    uint32_t initOffset = 0;
    uint16_t RNTI = 0;
    uint32_t Handle = 0;
    uint16_t TotalPucchCsiPart1Crc = 0;
    uint16_t TotalPuschCsiPart1Crc = 0;
    L1UciIndication_t* uciIndication = (L1UciIndication_t*)pMsg;
    initOffset += offsetof(L1UciIndication_t, pdus);
    offset = initOffset;

    /*Call any required sub structure handlers.*/
    for (j = 0; j < uciIndication->numUcis; j++) {
        L1UciInfo_t* uciPdu = (L1UciInfo_t*)((char*)pMsg + offset);
        offset += offsetof(L1UciInfo_t, Pdu);

        switch (uciPdu->PDUType) {
        case 0: {
            L1UciInPuschPdu_t* uciPduInfo = (L1UciInPuschPdu_t*)((char*)pMsg + offset);
            offset += offsetof(L1UciInPuschPdu_t, HARQInfo);
            if (0 != (uciPduInfo->pduBitmap & 2)) {
                L1HarqPduForFmt234_t* HarqPduForFmt234 = (L1HarqPduForFmt234_t*)((char*)pMsg + offset);
                ASSERT(HarqPduForFmt234);
                offset += sizeof(L1HarqPduForFmt234_t);
            }
            if (0 != (uciPduInfo->pduBitmap & 4)) {
                L1CSIPart1Pdu_t* CSIPart1Pdu = (L1CSIPart1Pdu_t*)((char*)pMsg + offset);
                macCSIPart1InfoUciInPuschPduFmt234PduHdlr(RNTI, Handle, j, CSIPart1Pdu, &TotalPuschCsiPart1Crc, dev_index, cellId);
                offset += sizeof(L1CSIPart1Pdu_t);
            }
            if (0 != (uciPduInfo->pduBitmap & 8)) {
                L1CSIPart2Pdu_t* cSIPart2Pdu = (L1CSIPart2Pdu_t*)((char*)pMsg + offset);
                ASSERT(cSIPart2Pdu);
                offset += sizeof(L1CSIPart2Pdu_t);
            }
            break;
        }
        case 1: {
            L1UciInPucchPduFmt01_t* uciInPucchPduFmt01 = (L1UciInPucchPduFmt01_t*)((char*)pMsg + offset);
            offset += offsetof(L1UciInPucchPduFmt01_t, SrInfo);

            if (0 != (uciInPucchPduFmt01->pduBitmap & 1)) {
                L1SrPduFMT01_t* srPduFMT01 = (L1SrPduFMT01_t*)((char*)pMsg + offset);
                ASSERT(srPduFMT01);
                offset += sizeof(L1SrPduFMT01_t);
            }

            if (0 != (uciInPucchPduFmt01->pduBitmap & 2)) {
                L1HarqPduForFm01_t* harqPduForFm01 = (L1HarqPduForFm01_t*)((char*)pMsg + offset);
                ASSERT(harqPduForFm01);
                offset += sizeof(L1HarqPduForFm01_t);
            }
            break;
        }
        case 2: {
            L1UciInPucchPduFmt234_t* uciInPucchPduFmt234 = (L1UciInPucchPduFmt234_t*)((char*)pMsg + offset);
            Handle = uciInPucchPduFmt234->Handle;
            RNTI = uciInPucchPduFmt234->RNTI;
            offset += offsetof(L1UciInPucchPduFmt234_t, SrInfo);

            if (0 != (uciInPucchPduFmt234->pduBitmap & 1)) {
                L1SrPduFMT234_t* srPduFMT234 = (L1SrPduFMT234_t*)((char*)pMsg + offset);
                ASSERT(srPduFMT234);
                offset += sizeof(L1SrPduFMT234_t);
            }
            if (0 != (uciInPucchPduFmt234->pduBitmap & 2)) {
                L1HarqPduForFmt234_t* harqPduForFmt234 = (L1HarqPduForFmt234_t*)((char*)pMsg + offset);
                ASSERT(harqPduForFmt234);
                offset += sizeof(L1HarqPduForFmt234_t);
            }
            if (0 != (uciInPucchPduFmt234->pduBitmap & 4)) {
                L1CSIPart1Pdu_t* cSIPart1Pdu = (L1CSIPart1Pdu_t*)((char*)pMsg + offset);
                macCSIPart1InfoUciInPucchPduFmt234PduHdlr(RNTI, Handle, j, cSIPart1Pdu, &TotalPucchCsiPart1Crc, dev_index, cellId);
                offset += sizeof(L1CSIPart1Pdu_t);
            }
            if (0 != (uciInPucchPduFmt234->pduBitmap & 8)) {
                L1CSIPart2Pdu_t* cSIPart2Pdu = (L1CSIPart2Pdu_t*)((char*)pMsg + offset);
                ASSERT(cSIPart2Pdu);
                offset += sizeof(L1CSIPart2Pdu_t);
            }
            if (0 != (uciInPucchPduFmt234->pduBitmap & 16)) {
                CMM_DEBUG(LOG_MAC, "macUciInfoInPucchPduFmt234PduHdlr");
                L1UciInfoPdu_t* uciInfoPdu = (L1UciInfoPdu_t*)((char*)pMsg + offset);
                macUciInfoInPucchPduFmt234PduHdlr(RNTI, Handle, j, uciInfoPdu, &TotalPucchCsiPart1Crc, dev_index, cellId);
                //offset += sizeof(L1UciInfoPdu_t);
                offset += offsetof(L1UciInfoPdu_t, uciPayload);
				offset += ceil(uciInfoPdu->UciBitLen / 8);
            }
            break;
        }
        }
        //ASSERT((offset - initOffset) <= uciPdu->PDUSize);
		if ((offset - initOffset) > uciPdu->PDUSize) {
			CMM_ERROR(LOG_MAC,"offset[%d]initOffset[%d]PDUSize[%d]PDUType[%d]",offset, initOffset, uciPdu->PDUSize,uciPdu->PDUType);
			ASSERT(0);
		}
        initOffset += uciPdu->PDUSize;
        offset = initOffset;
    }
    CMM_DEBUG(LOG_MAC, "Total:PucchCsiPart1Crc pass, have %d PucchCsiPart1Crc passed!", TotalPucchCsiPart1Crc);
    CMM_DEBUG(LOG_MAC, "Total:PuschCsiPart1Crc pass, have %d PuschCsiPart1Crc passed!", TotalPuschCsiPart1Crc);

    return;
} // uciIndicationProcDecHdlr()

static __inline__ unsigned long long rdtsc(void)
{
#ifdef __ARM_ARCH
    return rte_rdtsc();
#else
    unsigned long long int x;
    __asm__ volatile(".byte 0x0f, 0x31"
                     : "=A"(x));
    return x;
#endif
}

uint32_t fapiMsgDec(uint32_t* l1MessageInput, uint16_t dev_index, uint16_t cellId)
{
    L1GeneralMsgHdr_t* msgHdr = (L1GeneralMsgHdr_t*)l1MessageInput;

    static uint64_t timestamp[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 };
    uint64_t timestampEnd[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 };
    uint64_t rdtsc_diff = 0;
    char* pMsg = (char*)l1MessageInput + sizeof(L1GeneralMsgHdr_t);
    static uint32_t sSlotNum[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 };
    // uint8_t cellid = 0;

    switch (msgHdr->msgId) {
    case FAPI_RX_DATA_INDICATION:
        rxDataIndicationProcDecHdlr(pMsg, dev_index, cellId);
        break;
    case FAPI_SLOT_INDICATION:
        CMM_DEBUG(LOG_MAC, "slot_Indication: dev_index = %d cellid = %d msgType = %d, msgSize = %d, ", dev_index, cellId, msgHdr->msgId, msgHdr->msgSize);
        if (timestamp[dev_index][cellId] == 0) {
            timestamp[dev_index][cellId] = rdtsc();
        }
        timestampEnd[dev_index][cellId] = rdtsc();
        rdtsc_diff = abs(timestampEnd[dev_index][cellId] - timestamp[dev_index][cellId]);
        if (rdtsc_diff > maxTimeOut || rdtsc_diff < minTimeOut) {
            L1SlotIndication_t* slotIndication = (L1SlotIndication_t*)pMsg;
            CMM_ERROR(LOG_MAC, "dev:channel[%d:%d] timestamp = %lu slot times out, SFN = %d SLOT = %d\n", dev_index, cellId, rdtsc_diff, slotIndication->SFN, slotIndication->Slot);
        }
        timestamp[dev_index][cellId] = timestampEnd[dev_index][cellId];
        slotIndicationProcDecHdlr(pMsg, dev_index, cellId);
        sSlotNum[dev_index][cellId]++;
        if (sSlotNum[dev_index][cellId] >= 20000) {
            CMM_NOTICE(LOG_MAC, "dev_index:%d cellId: %d, Have %lu slots passed\n", dev_index, cellId, sSlotNum[dev_index][cellId]);
            macDumpCellUlInfo(dev_index, cellId);
            sSlotNum[dev_index][cellId] = 0;
        }
        setPhyState(dev_index, cellId, PHY_ACTIVE);
        break;
    case FAPI_CONFIG_RESPONSE:
        configResponseProcDecHdlr(pMsg, dev_index, cellId);
        break;
    case FAPI_STOP_INDICATION:
        macRevStopIndication(dev_index, cellId);
        break;
    case FAPI_ERROR_INDICATION:
        errorIndicationProcDecHdlr(pMsg, dev_index, cellId);
        break;
    case FAPI_CRC_INDICATION:
        CMM_DEBUG(LOG_MAC, "dev_index:%d cellId: %d,crc_Indication: msgType = %d, msgSize = %d, ", dev_index, cellId, msgHdr->msgId, msgHdr->msgSize);
        crcIndicationProcDecHdlr(pMsg, dev_index, cellId);
        break;
    case FAPI_UCI_INDICATION:
        CMM_DEBUG(LOG_MAC, "dev_index:%d cellId: %d,uci_Indication: msgType = %d, msgSize = %d, ", dev_index, cellId, msgHdr->msgId, msgHdr->msgSize);
        uciIndicationProcDecHdlr(pMsg, dev_index, cellId);
        break;
    case FAPI_SRS_INDICATION:
        CMM_DEBUG(LOG_MAC, "dev_index:%d cellId: %d,srs_Indication: msgType = %d, msgSize = %d, ", dev_index, cellId, msgHdr->msgId, msgHdr->msgSize);
        srsIndicationProcDecHdlr(pMsg, dev_index, cellId);
        break;
    case FAPI_PRACH_INDICATION:
        CMM_DEBUG(LOG_MAC, "dev_index:%d cellId: %d,prach_Indication: msgType = %d, msgSize = %d, ", dev_index, cellId, msgHdr->msgId, msgHdr->msgSize);
        rachIndicationProcDecHdlr(pMsg, dev_index, cellId);
        break;
    default:
        return -1;
    }
    return msgHdr->msgSize + 8;
}
