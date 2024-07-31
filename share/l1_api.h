/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

/**@brief Define F1AP struct be used in all 5G NR stacks.
The file is come from nr_f1.yaml.
v1.0 initial version
v1.1
     - change PrachFdOccasionConfigInfo_t.numUnusedRootSequences type u8 to u16;
     - change PrachFdOccasionConfigInfo_t.unusedRootSequences type u8 to u16;
     - change SrsPdu_t.frequencyShift type u8 to u16;
     - change SrsPdu_t.cyclicshift type u16 to u8;
     - change DlPduInfo_t.PDUType type u16 to u8;
     - Delete the use of TxPrecodingBeamformingPdu_t & RxBeamformingPdu_t.
v1.2
     - change ULDCI_t.UldciPdu count 255 to 64.
v1.3
     - change "CcrInfo_t" to "CrcInfo_t"
     - TxDataPduInfo_t delete "numTLV"&"TLVs",add "PduDataOffset"
v1.4
     - change "StioRequest_t" to "StopRequest_t"
     - change "ErrorIndocation_t" to "ErrorIndication_t"
     - change TXDATAREQ_t.pdus Count 4 to 70
     - change RxdataIndicatin_t.pdus Count 4 to 70
     - change CrcIndication_t.CRCs Count 4 to 70
     - change UciIndication_t.pdus Count 4 to 70 type
     - change TddTableConfigInfo_t.rev0 u16 to u8
     - change TddTableConfigInfo_t.rev1 u8 to u16
v1.5
     - change "StopResponse_t" to "StopIndication_t"
v1.6
     - adjust PDU size according to UE capability
     - delete SrsRbInfo_t and change SrsSymbolInfo_t.rbSNRs type "SrsRbInfo_t" to "u8"
v1.7
     - change DlDciInfo_t.Payload type u32 to u8
v1.8
     - change PdcchPduInfo_t.DLDCIs count MAX_NUM_DL_DCI_PDU_PER_PDCCH_PDU to 16
v1.9
     - change DLTTIReq_t.groups Count 1 to 16
     - change ULTTI_t.groups Count 1 to 16
v2.0
     - prefix each function with "L1"
v2.1
     - DataTLV length changed from u16 to u32
     -  L1TxDataPduInfo_t use TLV present data value
v2.2
     - Add L1FAPIMsgId_e and L1FAPIErrorCode_e
     - Add L1 General Msg Head and L1 Msg Head
v2.3
     - Modified the value of macro definition MAX_NUM_PREAMBLE_PER_PRACH_PDU_RPT
     - change L1TddTableConfigInfo_t.SlotConfig type u32 to u8
     - change MAX_SLOT_CFG_NUM 70 to 560
     - L1ConfigResponse_t four byte alignment
     - L1PrachConfigInfo_t add "prachConfigIndex" that is u8
v2.4
     - change L1GeneralMsgHdr_t.msgSize type u16 to u32
     - change L1PrachConfigInfo_t.prachConfigIndex type u8 to u16
     - add two macros MAX_TDD_PERIODICITY and MAX_NUM_OF_SYMBOL_PER_SLOT
     - MAX_SLOT_CFG_NUM is the multiplication of MAX_TDD_PERIODICITY and MAX_NUM_OF_SYMBOL_PER_SLOT
v2.5
     - L1DmrsInfo_t is divided into L1PdschDmrsInfo_t and L1PuschDmrsInfo_t
     - L1PuschDmrsInfo_t add puschIdentity type is u16
     - change L1PucchPduInfo_t.BitLenHarq type u8 to u16
v2.6
     - Fix 4 bytes align issue in L1ErrorIndication_t
     - Change FAPI interface structure array size and make it based on the Macros
v2.7
     - change L1CarriConfigInfo_t.rev1 type u8 to u16
     - change L1UciPduInfo_t type STRUCT to CHOICE
v2.8
     - change L1RxDataPduInfo_t.PDULength type u16 to u32
v3.0
     - L1SsbConfigInfo_t add ssPbchBlockPowerScaling & bchPayloadFlag
     - L1SsbConfigInfo_t delete ssPbchPower
     - L1PrachConfigInfo_t add prachResConfigIndex
     - change L1PrachConfigInfo_t.prachConfigIndex type u16 to u8
     - change L1PrachFdOccasionConfigInfo_t.prachFrequencyOffset type u16 to
     - L1SsbTableConfigInfo_t add ssbConfigIndex & subcarrierSpacing & subCarrierSpacingCommon & Case
     - L1SsbTableConfigInfo_t delete betaPss
     - L1MeasurementConfigInfo_t add RsrpMeasurement
     - L1DBTConfigInfo_t change numTXRUs to numBasebandPorts
     - add L1PMConfigInfo_t L1UciConfigurationInfo_t L1PrbSym_t L1PucchSSConfig_t L1PdschConfig_t
     - L1DLTTIReq_t add nDlTypes & nPDUsOfEachType
     - change L1DlPduInfo_t.PDUType type u8 to u16
     - change dataScramblingId to nIdPdsch in L1PdschPduInfo_t
     - change i16 ptrsPowerOffset to u8 PTRSReOffset in L1PtrsInfo_t
     - Change L1CbgFeildInfo_t. dlTbCrcCW to array number is 8
     - Change L1ULTTI_t.nPDUs to array number is 5
     - delete RachPresent nULSCH nULCCH in L1ULTTI_t
     - change L1UlPduInfo_t.PDUType type u8 to u16
     - change L1PuschPduInfo_t.dataScramblingId to nIdPusch
     - change dataScramblingId to nIdPucchScrambling in L1PucchPduInfo_t
     - change freqHopFlag to intraSlotFrequencyHopping in L1PucchHoppingInfo_t
     - delete BitLenCsiPart2 & add ulBwpId in L1PucchPduInfo_t
     - change L1TxDataPduInfo_t.numTLV type u16-to u32
     - add CWIndex in L1TxDataPduInfo_t
     - change MAX_NUM_RPT_SRS_SYMBOL from 2 to 4
     - change L1UciInfoPdu_t.uciPayload to variable array.
v3.1 - change L1PuschUci_t.Part2s to array max numble is 4
v3.2 - change beamforminginfo from offset to fields.
v3.3
v3.4
v3.5
v3.7
     - added Table 3-46 uci1 to uci2 information
     - added numPart2s, Added reference for PUSCH and PUCCH PDU
     - added SRS TRI,TPMI report
v3.8
     - add codebook subset and transform precoding for SRS PDU, ed codebook subset and transform precoding
v3.9
     - add parameters of srs usage report type, etc for SRS PDU from FAPI
     - Updated SRS indication to align with FAPI
     - add note for empty SRS cyclic shift in SRS PDU
*/
#ifndef __L1_API_H_
#define __L1_API_H_

#include <stdint.h>
#ifndef NUM_EROLOGIES
#define NUM_EROLOGIES 5
#endif

#ifndef NUM_UNUSED_ROOT_SEQUENCES
#define NUM_UNUSED_ROOT_SEQUENCES 3
#endif

#ifndef NUM_SSB_MASK
#define NUM_SSB_MASK 2
#endif

#ifndef NUM_SSB_BEAM_ID
#define NUM_SSB_BEAM_ID 64
#endif

#ifndef NUM_MAP
#define NUM_MAP 2
#endif

#ifndef NUM_FREQ_DOMAIN_RES
#define NUM_FREQ_DOMAIN_RES 6
#endif

#ifndef NUM_FREQ_DOMAIN_RB
#define NUM_FREQ_DOMAIN_RB 35
#endif

#ifndef NUM_PART1_PARAMS
#define NUM_PART1_PARAMS 4
#endif

#ifndef MAX_NUM_PART2_TYPES
#define MAX_NUM_PART2_TYPES 100
#endif

#ifndef MAX_NP
#define MAX_NP 272
#endif

#ifndef MAX_NU
#define MAX_NU 7
#endif

#ifndef MAX_NG
#define MAX_NG 511
#endif

#ifndef MAX_IQSIZE
#define MAX_IQSIZE 4
#endif

#ifndef MAX_CHANNEL_MATRIX_ARRAY
#define MAX_CHANNEL_MATRIX_ARRAY MAX_NP *MAX_NU *MAX_NG *MAX_IQSIZE
#endif

#ifndef NUM_DL_DCI_PAYLOAD
#define NUM_DL_DCI_PAYLOAD 20
#endif

#ifndef NUM_CORESET_FREQDOMAINRESOURCE
#define NUM_CORESET_FREQDOMAINRESOURCE 6
#endif

#ifndef NUM_PXSCH_RB_BITMAP
#define NUM_PXSCH_RB_BITMAP 36
#endif

#ifndef NUM_DL_TB_CRCCW
#define NUM_DL_TB_CRCCW 8
#endif

#ifndef NUM_PUSCH_CB_PRESENT_AND_POSITION
#define NUM_PUSCH_CB_PRESENT_AND_POSITION 128
#endif

#ifndef NUM_HARQ_PAYLOAD
#define NUM_HARQ_PAYLOAD 16
#endif

#ifndef NUM_CSI_PART1_PAYLOAD
#define NUM_CSI_PART1_PAYLOAD 32
#endif

#ifndef NUM_CB_CRC_STATUS
#define NUM_CB_CRC_STATUS 128
#endif

#ifndef NUM_FMT01_HARQ_VALUE
#define NUM_FMT01_HARQ_VALUE 2
#endif

#ifndef NUM_FMT234_SR_PAYLOAD
#define NUM_FMT234_SR_PAYLOAD 2
#endif

#ifndef NUM_PDU_EACH_TYPE
#define NUM_PDU_EACH_TYPE 5
#endif

#ifndef NUM_ULDCI_PDU_EACH_TYPE
#define NUM_ULDCI_PDU_EACH_TYPE 2
#endif

#ifndef NUM_PARAM_OFFSET
#define NUM_PARAM_OFFSET 4
#endif

#ifndef NUM_PARAM_SIZE
#define NUM_PARAM_SIZE 4
#endif

#ifndef MAX_TDD_PERIODICITY
#define MAX_TDD_PERIODICITY 80
#endif

#ifndef MAX_NUM_OF_SYMBOL_PER_SLOT
#define MAX_NUM_OF_SYMBOL_PER_SLOT 14
#endif

#ifndef MAX_SLOT_CFG_NUM
#define MAX_SLOT_CFG_NUM MAX_TDD_PERIODICITY* MAX_NUM_OF_SYMBOL_PER_SLOT
#endif

#ifndef MAX_NUM_CELLS
#define MAX_NUM_CELLS 2
#endif

#ifndef MAX_NUM_TX_ANTENNAS_1CELL
#define MAX_NUM_TX_ANTENNAS_1CELL 4
#endif

#ifndef MAX_NUM_RX_ANTENNAS_1CELL
#define MAX_NUM_RX_ANTENNAS_1CELL 4
#endif

#ifndef MAX_NUM_ERR_INDICATION_PER_SLOT
#define MAX_NUM_ERR_INDICATION_PER_SLOT 8
#endif

#ifndef MAX_NUM_ERR_INDICATION_TLVS
#define MAX_NUM_ERR_INDICATION_TLVS 3
#endif

#ifndef MAX_NUM_ERR_INFO_VALUE_LEN
#define MAX_NUM_ERR_INFO_VALUE_LEN 7
#endif

#ifndef MAX_NUM_TX_ANTENNAS_2CELL
#define MAX_NUM_TX_ANTENNAS_2CELL 8
#endif

#ifndef MAX_NUM_RX_ANTENNAS_2CELL
#define MAX_NUM_RX_ANTENNAS_2CELL 8
#endif

#ifndef MAX_NUM_TX_ANTENNAS
#define MAX_NUM_TX_ANTENNAS MAX_NUM_TX_ANTENNAS_1CELL
#endif

#ifndef MAX_NUM_RX_ANTENNAS
#define MAX_NUM_RX_ANTENNAS MAX_NUM_RX_ANTENNAS_1CELL
#endif

#ifndef MAX_NUM_SSB_PDU_PER_SLOT
#define MAX_NUM_SSB_PDU_PER_SLOT 2
#endif

#ifndef MAX_TOTAL_NUM_DL_DCI_PDU_PER_SLOT
#define MAX_TOTAL_NUM_DL_DCI_PDU_PER_SLOT 16
#endif

#ifndef MAX_TOTAL_NUM_UL_DCI_PDU_PER_SLOT
#define MAX_TOTAL_NUM_UL_DCI_PDU_PER_SLOT 16
#endif

#ifndef MAX_NUM_PDCCH_PDU_FOR_DL_PER_SLOT
#define MAX_NUM_PDCCH_PDU_FOR_DL_PER_SLOT 16
#endif

#ifndef MAX_NUM_PDCCH_PDU_FOR_UL_PER_SLOT
#define MAX_NUM_PDCCH_PDU_FOR_UL_PER_SLOT 16
#endif

#ifndef MAX_NUM_DL_DCI_PDU_PER_PDCCH_PDU
#define MAX_NUM_DL_DCI_PDU_PER_PDCCH_PDU (MAX_TOTAL_NUM_DL_DCI_PDU_PER_SLOT / MAX_NUM_PDCCH_PDU_FOR_DL_PER_SLOT)
#endif

#ifndef MAX_NUM_UL_DCI_PDU_PER_PDCCH_PDU
#define MAX_NUM_UL_DCI_PDU_PER_PDCCH_PDU (MAX_TOTAL_NUM_UL_DCI_PDU_PER_SLOT / MAX_NUM_PDCCH_PDU_FOR_UL_PER_SLOT)
#endif

#ifndef MAX_NUM_CSI_RS_PDU_PER_SLOT
#define MAX_NUM_CSI_RS_PDU_PER_SLOT 32
#endif

#ifndef MAX_NUM_PDSCH_PDU_PER_SLOT
#define MAX_NUM_PDSCH_PDU_PER_SLOT 16
#endif

#ifndef MAX_NUM_DL_PDU_PER_SLOT
#define MAX_NUM_DL_PDU_PER_SLOT (MAX_NUM_SSB_PDU_PER_SLOT + MAX_TOTAL_NUM_DL_DCI_PDU_PER_SLOT + MAX_NUM_CSI_RS_PDU_PER_SLOT + MAX_NUM_PDSCH_PDU_PER_SLOT)
#endif

#ifndef MAX_NUM_PUSCH_PDU_PER_SLOT
#define MAX_NUM_PUSCH_PDU_PER_SLOT 16
#endif

#ifndef MAX_NUM_ULSCH_PDU_IN_RX_DATA_INDICATION
#define MAX_NUM_ULSCH_PDU_IN_RX_DATA_INDICATION MAX_NUM_PUSCH_PDU_PER_SLOT
#endif

#ifndef MAX_NUM_ULSCH_CRC_IN_CRC_INDICATION
#define MAX_NUM_ULSCH_CRC_IN_CRC_INDICATION MAX_NUM_PUSCH_PDU_PER_SLOT
#endif

#ifndef MAX_NUM_PUSCH_UCI_PDU_IN_UCI_INDICATION
#define MAX_NUM_PUSCH_UCI_PDU_IN_UCI_INDICATION MAX_NUM_PUSCH_PDU_PER_SLOT
#endif

#ifndef MAX_NUM_PUCCH_PDU_FMT_0_1_PER_SLOT
#define MAX_NUM_PUCCH_PDU_FMT_0_1_PER_SLOT 112
#endif

#ifndef MAX_NUM_PUCCH_FMT_0_1_UCI_IN_UCI_INDICATION
#define MAX_NUM_PUCCH_FMT_0_1_UCI_IN_UCI_INDICATION MAX_NUM_PUCCH_PDU_FMT_0_1_PER_SLOT
#endif

#ifndef MAX_NUM_PUCCH_PDU_FMT_2_3_4_PER_SLOT
#define MAX_NUM_PUCCH_PDU_FMT_2_3_4_PER_SLOT 32
#endif

#ifndef MAX_NUM_PUCCH_FMT_2_3_4_UCI_IN_UCI_INDICATION
#define MAX_NUM_PUCCH_FMT_2_3_4_UCI_IN_UCI_INDICATION MAX_NUM_PUCCH_PDU_FMT_2_3_4_PER_SLOT
#endif

#ifndef MAX_UCI_NUM_IN_UCI_IND
#define MAX_UCI_NUM_IN_UCI_IND (MAX_NUM_PUSCH_UCI_PDU_IN_UCI_INDICATION + MAX_NUM_PUCCH_FMT_0_1_UCI_IN_UCI_INDICATION + MAX_NUM_PUCCH_FMT_2_3_4_UCI_IN_UCI_INDICATION)
#endif

#ifndef MAX_NUM_PUCCH_PDU_PER_SLOT
#define MAX_NUM_PUCCH_PDU_PER_SLOT (MAX_NUM_PUCCH_PDU_FMT_0_1_PER_SLOT + MAX_NUM_PUCCH_PDU_FMT_2_3_4_PER_SLOT)
#endif

#ifndef MAX_NUM_SRS_PDU_PER_SLOT
#define MAX_NUM_SRS_PDU_PER_SLOT 16
#endif

#ifndef MAX_NUM_SRS_PRB
#define MAX_NUM_SRS_PRB 272
#endif

#ifndef MAX_NUM_SRS_SYMBOL
#define MAX_NUM_SRS_SYMBOL 2
#endif

#ifndef MAX_NUM_RPT_SRS_SYMBOL
#define MAX_NUM_RPT_SRS_SYMBOL 4
#endif

#ifndef MAX_NUM_PRACH_OCCASIONS_IN_FREQ_DOMAIN
#define MAX_NUM_PRACH_OCCASIONS_IN_FREQ_DOMAIN 4
#endif

#ifndef MAX_NUM_PRACH_OCCASIONS_IN_TIME_DOMAIN
#define MAX_NUM_PRACH_OCCASIONS_IN_TIME_DOMAIN 7
#endif

#ifndef MAX_NUM_PRACH_PDU_PER_SLOT
#define MAX_NUM_PRACH_PDU_PER_SLOT MAX_NUM_PRACH_OCCASIONS_IN_FREQ_DOMAIN
#endif

#ifndef MAX_TOTAL_NUM_PRACH_PREAMPLES
#define MAX_TOTAL_NUM_PRACH_PREAMPLES 64
#endif

#ifndef MAX_NUM_PRACH_PDU_IN_PRACH_INDICATION
#define MAX_NUM_PRACH_PDU_IN_PRACH_INDICATION (MAX_NUM_PRACH_OCCASIONS_IN_FREQ_DOMAIN * MAX_NUM_PRACH_OCCASIONS_IN_TIME_DOMAIN)
#endif

#ifndef MAX_NUM_PREAMBLE_PER_PRACH_PDU_RPT
#define MAX_NUM_PREAMBLE_PER_PRACH_PDU_RPT MAX_TOTAL_NUM_PRACH_PREAMPLES
#endif

#ifndef MAX_NUM_PDU_IN_UL_TTI_REQ
#define MAX_NUM_PDU_IN_UL_TTI_REQ (MAX_NUM_PREAMBLE_PER_PRACH_PDU_RPT + MAX_NUM_SRS_PDU_PER_SLOT + MAX_NUM_PUCCH_PDU_PER_SLOT + MAX_NUM_PUSCH_PDU_PER_SLOT)
#endif

#ifndef MAX_NUM_TXRU
#define MAX_NUM_TXRU 8
#endif

#ifndef MAX_NUM_DIG_BEAMS
#define MAX_NUM_DIG_BEAMS 64
#endif

#ifndef MAX_NUM_LAYERS
#define MAX_NUM_LAYERS 4
#endif

#ifndef MAX_NUM_DL_GROUP_UE
#define MAX_NUM_DL_GROUP_UE 12
#endif

#ifndef MAX_NUM_UL_GROUP_UE
#define MAX_NUM_UL_GROUP_UE 6
#endif

#ifndef MAX_NUM_BUG_BFINTERFACES
#define MAX_NUM_BUG_BFINTERFACES 512
#endif

#ifndef MAX_NUM_PRGS
#define MAX_NUM_PRGS 275
#endif

#ifndef MAX_NUM_PRGS_SRS
#define MAX_NUM_PRGS_SRS 272
#endif

#ifndef MAX_NUM_PDSCH_NR_Of_CODEWORD
#define MAX_NUM_PDSCH_NR_Of_CODEWORD 2
#endif

#ifndef MAX_NUM_PDCCH_DlDCI
#define MAX_NUM_PDCCH_DlDCI 16
#endif

#ifndef MAX_NUM_DL_GROUP_PER_SLOT
#define MAX_NUM_DL_GROUP_PER_SLOT 16
#endif

#ifndef MAX_NUM_RX_BEAMFORMING
#define MAX_NUM_RX_BEAMFORMING 512
#endif

#ifndef MAX_NUM_PUSCH_PTRS_PORT
#define MAX_NUM_PUSCH_PTRS_PORT 2
#endif

#ifndef MAX_NUM_GROUP_IN_UL_TTI_REQ
#define MAX_NUM_GROUP_IN_UL_TTI_REQ 16
#endif

#ifndef MAX_NUM_TXDATA_TLV
#define MAX_NUM_TXDATA_TLV 4
#endif

#ifndef MAX_NUM_UCIMAPS
#define MAX_NUM_UCIMAPS 4
#endif

#ifndef MAX_NUM_PART2
#define MAX_NUM_PART2 100
#endif

typedef enum {
    FAPI_PARAM_REQUEST,
    FAPI_PARAM_RESPONSE,
    FAPI_CONFIG_REQUEST,
    FAPI_CONFIG_RESPONSE,
    FAPI_START_REQUEST,
    FAPI_STOP_REQUEST,
    FAPI_STOP_INDICATION,
    FAPI_ERROR_INDICATION,
    FAPI_DL_TTI_REQUEST = 0x80,
    FAPI_UL_TTI_REQUEST,
    FAPI_SLOT_INDICATION,
    FAPI_UL_DCI_REQUEST,
    FAPI_TX_DATA_REQUEST,
    FAPI_RX_DATA_INDICATION,
    FAPI_CRC_INDICATION,
    FAPI_UCI_INDICATION,
    FAPI_SRS_INDICATION,
    FAPI_PRACH_INDICATION,
    L1FAPIMSGID_E_CNT
} L1FAPIMsgId_e;

typedef enum {
    FAPI_MSG_OK,
    FAPI_MSG_INVALID_STATE,
    FAPI_MSG_INVALID_CONFIG,
    FAPI_SFN_OUT_OF_SYNC,
    FAPI_MSG_SLOT_ERR,
    FAPI_MSG_BCH_MISSING,
    FAPI_MSG_INVALID_SFN,
    FAPI_MSG_UL_DCI_ERR,
    FAPI_MSG_TX_ERR,
    L1FAPIERRORCODE_E_CNT
} L1FAPIErrorCode_e;

/**@brief L1 Msg Head*/
typedef struct
{
    uint8_t msgNum;
    uint8_t opaque;
    uint16_t rev;
} L1MsgHdr_t;

/**@brief L1 General Msg Head*/
typedef struct
{
    L1FAPIMsgId_e msgId;
    uint32_t msgSize;
} L1GeneralMsgHdr_t;

/**@brief PARAM request*/
typedef struct
{
} L1ParamRequest_t;

/**@brief PARAM response*/
typedef struct
{
    uint8_t ErrorCode;
} L1ParamResponse_t;

/**@brief PRACHFdOccasions.*/
typedef struct
{
    uint16_t prachRootSequenceIndex;
    int16_t prachFrequencyOffset;
    uint16_t numUnusedRootSequences;
    uint8_t numRootSequences;
    uint8_t prachZeroCorrConf;
    uint16_t unusedRootSequences[NUM_UNUSED_ROOT_SEQUENCES];
    uint16_t rev;
} L1PrachFdOccasionConfigInfo_t;

/**@brief TXRU.*/
typedef struct
{
    int16_t digBeamWeightRe;
    int16_t digBeamWeightIm;
} L1TxRu_t;

/**@brief DigBeams*/
typedef struct
{
    uint16_t beamIdx;
    uint16_t rev;
    /**variable-size by numTXRUs*/
    L1TxRu_t TXRU[]; /* Max size: MAX_NUM_TXRU*/
} L1DigBeam_t;

/**@brief ant Ports.*/
typedef struct
{
    int16_t precoderWeightRe;
    int16_t precoderWeightIm;
} L1PrecoderInfo_t;

/**@brief layers*/
typedef struct
{
    /**variable-size by numAntPorts*/
    L1PrecoderInfo_t precoderInfo[0]; /* Max size: MAX_NUM_TX_ANTENNAS*/
} L1PMLayer_t;

/**@brief Carri Configuration.*/
typedef struct
{
    uint32_t dlFrequency;
    uint32_t ulFrequency;
    uint16_t dlBandwidth;
    uint16_t ulBandwidth;
    uint16_t dlk0[NUM_EROLOGIES];
    uint16_t ulk0[NUM_EROLOGIES];
    uint16_t dlGridSize[NUM_EROLOGIES];
    uint16_t ulGridSize[NUM_EROLOGIES];
    uint16_t numTxAnt;
    uint16_t numRxAnt;
    uint8_t FrequencyShift7p5KHz;
    uint8_t rev0;
    uint16_t rev1;
} L1CarriConfigInfo_t;

/**@brief Cell Configuration.*/
typedef struct
{
    uint16_t phyCellId;
    uint8_t FrameDuplexType;
    uint8_t rev0;
} L1CellConfigInfo_t;

/**@brief SSB Configuration.*/
typedef struct
{
    int16_t ssPbchBlockPowerScaling;
    uint8_t bchPayloadFlag;
    uint8_t rev0;
} L1SsbConfigInfo_t;

/**@brief L1PrachConfigurations*/
typedef struct
{
    uint16_t prachResConfigIndex;
    uint8_t prachSequenceLength;
    uint8_t prachSubCSpacing;
    uint8_t ulBwpPuschScs;
    uint8_t restrictedSetConfig;
    uint8_t numPrachFdOccasions;
    uint8_t prachConfigIndex;
    uint8_t SsbPerRach;
    uint8_t rev0;
    uint16_t rev1;
    /**variable-size by numPrachFdOccasions*/
    L1PrachFdOccasionConfigInfo_t PrachFdOccaCfgInfos[]; /* Max size: MAX_NUM_PRACH_OCCASIONS_IN_FREQ_DOMAIN*/
} L1PrachConfiguration_t;

/**@brief PRACH Configuration.*/
typedef struct
{
    uint16_t numPrachConfigurations;
    uint16_t rev;
    /**variable-size by numPrachConfigurations*/
    L1PrachConfiguration_t PrachConfiguration[]; /* Max size: 2*/
} L1PrachConfigInfo_t;

/**@brief SSB Table*/
typedef struct
{
    uint16_t ssbConfigIndex;
    uint16_t ssbOffsetPointA;
    int16_t betaPssProfileSSS;
    uint8_t ssbPeriod;
    uint8_t ssbSubcarrierOffset;
    uint8_t Case;
    uint8_t subcarrierSpacing;
    uint8_t subCarrierSpacingCommon;
    uint8_t lMax;
    uint8_t rmsiPresence;
    uint8_t rev0;
    uint16_t rev1;
    uint32_t SsbMask[NUM_SSB_MASK];
    uint8_t BeamId[NUM_SSB_BEAM_ID];
} L1SsbTableConfigInfo_t;

/**@brief TDD Table*/
typedef struct
{
    uint8_t TddPeriod;
    uint8_t rev0;
    uint16_t rev1;
    uint8_t SlotConfig[MAX_SLOT_CFG_NUM];
} L1TddTableConfigInfo_t;

/**@brief Measurement Config*/
typedef struct
{
    uint8_t RssiMeasurement;
    uint8_t RsrpMeasurement;
    uint16_t rev;
} L1MeasurementConfigInfo_t;

/**@brief Storing Precoding and Beamforming Tables*/
typedef struct
{
    uint16_t numDigBeams;
    uint16_t numBasebandPorts;
    /**variable-size by numDigBeams*/
    L1DigBeam_t digBeam[]; /* Max size: MAX_NUM_DIG_BEAMS*/
} L1DBTConfigInfo_t;

/**@brief Precoding Matrix (PM) PDU*/
typedef struct
{
    uint16_t PMidx;
    uint16_t numLayers;
    uint16_t numAntPorts;
    uint16_t rev;
    /**variable-size by numLayers*/
    L1PMLayer_t layers[]; /* Max size: MAX_NUM_LAYERS*/
} L1PMConfigInfo_t;

/**@brief Precoding Matrix (PM) table*/
typedef struct
{
    uint16_t numPMTables;
    uint16_t rev0;
    /**variable-size by numPMTables*/
    L1PMConfigInfo_t pmTable[]; /* Max size: 256*/
} L1PMCfgTableInfo_t;

/**@brief the map Index*/
typedef struct
{
    uint8_t numPart1Params;
    uint8_t rev0;
    uint16_t map[NUM_MAP];
    /**variable-size by numPart1Params*/
    uint8_t sizesPart1Params[]; /* Max size: 4*/
} L1MapIndex_t;

/**@brief UCI configuration*/
typedef struct
{
    uint16_t numUci2Maps;
    uint16_t rev;
    /**variable-size by numUci2Maps*/
    L1MapIndex_t mapIndex[]; /* Max size: MAX_NUM_UCIMAPS*/
} L1UciConfigurationInfo_t;

/**@brief For numCoresetRmPatterns*/
typedef struct
{
    uint16_t symbolsPattern;
    uint8_t freqDomainResources[NUM_FREQ_DOMAIN_RES];
    uint8_t coresetRmPatternID;
    uint8_t rev0;
    uint16_t rev1;
} L1CoresetRmPattern_t;

/**@brief For each PrbSymbRateMatchPattern*/
typedef struct
{
    uint8_t prbSymbRateMatchPatternID;
    uint8_t freqDomainRB[NUM_FREQ_DOMAIN_RB];
    uint8_t subcarrierSpacing;
    uint8_t rev0;
    uint16_t rev1;
    uint32_t symbolsInRB;
} L1PrbSymbRateMatchPatterns_t;

/**@brief PRB-Symbol Rate Match Patterns*/
typedef struct
{
    uint8_t numberOfPrbSymbRateMatchPatterns;
    uint8_t numCoresetRmPatterns;
    uint16_t rev;
    /**variable-size by numCoresetRmPatterns*/
    L1CoresetRmPattern_t CoresetRmPatterns[0];
    /**variable-size by numberOfPrbSymbRateMatchPatterns*/
    L1PrbSymbRateMatchPatterns_t PrbSymbRateMatchPatterns[]; /* Max size: 255*/
} L1PrbSym_t;

/**@brief For ulBwpId*/
typedef struct
{
    uint16_t nIdPucchHopping;
    uint8_t pucchGroupHopping;
    uint8_t rev0;
} L1UlBwpIds_t;

/**@brief PUCCH Semi-static Config*/
typedef struct
{
    uint8_t numUlBwpIds;
    uint8_t rev0;
    uint16_t rev1;
    L1UlBwpIds_t UlBwpIds[]; /* Max size: 8*/
} L1PucchSSConfig_t;

/**@brief PDSCH Config*/
typedef struct
{
    uint8_t pdschCbgScheme;
    uint8_t rev0;
    uint16_t rev1;
} L1PdschConfig_t;

/**@brief L1ConfigRequest_t*/
typedef struct
{
    L1CarriConfigInfo_t CarriConfig;
    L1CellConfigInfo_t CellConfig;
    L1SsbConfigInfo_t SSBConfig;
    L1PrachConfigInfo_t PRACHCfgInfo;
    L1SsbTableConfigInfo_t SSBtableInfo;
    L1TddTableConfigInfo_t TDDtableInfo;
    L1MeasurementConfigInfo_t MeasurementCfgInfo;
    L1UciConfigurationInfo_t UciConfigurationInfo;
    L1PrbSym_t PrbSymol;
    L1PucchSSConfig_t PucchSSConfig;
    L1PdschConfig_t PdschConfig;
    L1DBTConfigInfo_t DBTInfo;
    L1PMCfgTableInfo_t PMInfo;
} L1ConfigRequest_t;

/**@brief UE*/
typedef struct
{
    uint8_t PduIdx;
    uint8_t rev0;
    uint16_t rev1;
} L1GroupUeInfo_t;

/**@brief group*/
typedef struct
{
    uint8_t nUe;
    uint8_t rev0;
    uint16_t rev1;
    /**variable-size by nUe*/
    L1GroupUeInfo_t Ues[]; /* Max size: MAX_NUM_DL_GROUP_UE*/
} L1DlGroupInfo_t;

/**@brief Pdcch BWP*/
typedef struct
{
    uint16_t CoresetBWPSize;
    uint16_t CoresetBWPStart;
    uint8_t SubcarrierSpacing;
    uint8_t CyclicPrefix;
    uint16_t rev;
} L1PdcchBwpInfo_t;

/**@brief DL TTI BWP*/
typedef struct
{
    uint16_t BWPSize;
    uint16_t BWPStart;
    uint8_t SubcarrierSpacing;
    uint8_t CyclicPrefix;
    uint16_t rev;
} L1BwpInfo_t;

/**@brief Tx Precoding and Beamforming PDU*/
typedef struct
{
    uint16_t numPRGs;
    uint16_t prgSize;
    uint8_t digBFInterfaces;
    uint8_t rev0;
    uint16_t rev1;
    /**variable-size by numPRGs*/
    uint16_t PMidx[]; /* Max size: MAX_NUM_PRGS*/
    /**variable-size by digBFInterfaces*/
    // uint16_t beamIdx[]; /* Max size: 256*/
} L1TxPrecodingBeamformingPdu_t;

/**@brief Tx Power info*/
typedef struct
{
    int16_t pdcchDmrsPowerOffsetProfileSSS;
    int16_t pdcchDataPowerOffsetProfileSSS;
} L1PdcchTxPowerInfo_t;

/**@brief DL DCI PDU*/
typedef struct
{
    uint16_t RNTI;
    uint16_t nIdPdcchData;
    uint16_t nRntiPdcchData;
    uint16_t nIdPdcchDmrs;
    uint8_t CceIndex;
    uint8_t AggregationLevel;
    uint16_t PayloadSizeBits;
    L1PdcchTxPowerInfo_t TxPowerInfo;
    uint8_t Payload[NUM_DL_DCI_PAYLOAD];
    L1TxPrecodingBeamformingPdu_t BeamformingInfo;
} L1DlDciInfo_t;

/**@brief DL TTI Coreset*/
typedef struct
{
    uint16_t ShiftIndex;
    uint8_t StartSymbolIndex;
    uint8_t DurationSymbols;
    uint8_t FreqDomainResource[NUM_CORESET_FREQDOMAINRESOURCE];
    uint8_t CceRegMappingType;
    uint8_t RegBundleSize;
    uint8_t InterleaverSize;
    uint8_t CoreSetType;
    uint8_t precoderGranularity;
    uint8_t rev;
} L1CoresetInfo_t;

/**@brief Pdsch Codeword*/
typedef struct
{
    uint32_t TBSize;
    uint32_t tbSizeLbrmBytes;
    uint16_t targetCodeRate;
    uint8_t qamModOrder;
    uint8_t mcsIndex;
    uint8_t mcsTable;
    uint8_t rvIndex;
    uint8_t ldpcBaseGraph;
    uint8_t rev;
} L1Codeword_t;

/**@brief PDSCH Codeword Info*/
typedef struct
{
    uint8_t NrOfCodewords;
    uint8_t rev0;
    uint16_t rev1;
    L1Codeword_t CodewordInfo[MAX_NUM_PDSCH_NR_Of_CODEWORD];
} L1CodewordInfo_t;

/**@brief PDSCH DMRS*/
typedef struct
{
    uint16_t dlDmrsSymbPos;
    uint16_t dlDmrsScramblingId;
    uint16_t dmrsPorts;
    uint8_t maxLen;
    uint8_t dmrsConfigType;
    uint8_t nSCID;
    uint8_t numDmrsCdmGrpsNoData;
    uint16_t rev;
} L1PdschDmrsInfo_t;

/**@brief Pdsch Allocation in frequency domain.*/
typedef struct
{
    uint16_t rbStart;
    uint16_t rbSize;
    uint16_t coresetStartPoint;
    uint16_t initialDlBwpSize;
    uint8_t resourceAlloc;
    uint8_t VRBtoPRBMapping;
    uint8_t pdschTransType;
    uint8_t rev;
    uint8_t rbBitmap[NUM_PXSCH_RB_BITMAP];
} L1PdschAllocationFdInfo_t;

/**@brief Resource Allocation in time domain*/
typedef struct
{
    uint8_t StartSymbolIndex;
    uint8_t NrOfSymbols;
    uint16_t rev;
} L1PxschAllocationTdInfo_t;

/**@brief PDSCH PTRS*/
typedef struct
{
    int16_t pdschPtrsPowerOffsetProfileSSS;
    uint8_t PTRSPortIndex;
    uint8_t PTRSTimeDensity;
    uint8_t PTRSFreqDensity;
    uint8_t PTRSReOffset;
    uint16_t rev;
} L1PtrsInfo_t;

/**@brief PDSCH Tx Power*/
typedef struct
{
    int16_t pdschDmrsPowerOffsetProfileSSS;
    int16_t pdschDataPowerOffsetProfileSSS;
} L1TxPowerInfo_t;

/**@brief CSI Tx Power*/
typedef struct
{
    int16_t csiRsPowerOffsetProfileSSS;
    uint16_t rev;
} L1CsiTxPowerInfo_t;

/**@brief PDCSH CBG*/
typedef struct
{
    uint8_t maxNumCbgPerTb;
    uint8_t rev0;
    uint16_t rev1;
    uint8_t dlTbCrcCW[NUM_DL_TB_CRCCW];
} L1CbgFeildInfo_t;

/**@brief PDCCH PDU*/
typedef struct
{
    uint16_t pdcchPduIndex;
    uint16_t numDlDci;
    L1PdcchBwpInfo_t BWP;
    L1CoresetInfo_t Coreset;
    /**variable-size by numDlDci*/
    L1DlDciInfo_t DlDCIs[]; /* Max size: MAX_NUM_PDCCH_DlDCI*/
} L1PdcchPduInfo_t;

/**@brief For numPrbSymRmPatternsByValue*/
typedef struct
{
    uint16_t symbolsInRB;
    uint8_t freqDomainRB[NUM_FREQ_DOMAIN_RB];
} L1PrbSymRmPatternsByValue_t;

/**@brief Rate Matching references*/
typedef struct
{
    uint16_t ssbConfigForRateMatching;
    uint8_t numCoresetRMP;
    uint8_t numCsiRsForRM;
    /**variable-size by numCoresetRMP*/
    uint8_t coresetRMPIndex[0];
    /**variable-size by numCsiRsForRM*/
    uint16_t csiRsForRM[]; /* Max size: 255*/
} L1RateMatchingInformation_t;

/**@brief PDSCH PDU*/
typedef struct
{
    uint16_t pduBitmap;
    uint16_t RNTI;
    uint16_t pduIndex;
    uint16_t rev;
    L1BwpInfo_t BWP;
    L1CodewordInfo_t CodewordInfo;
    uint16_t nIdPdsch;
    uint8_t nrOfLayers;
    uint8_t refPoint;
    L1PdschDmrsInfo_t DMRSInfo;
    L1PdschAllocationFdInfo_t ResourceAllocFd;
    L1RateMatchingInformation_t RateMatchingInformation;
    L1PxschAllocationTdInfo_t ResourceAllocTd;
    L1TxPowerInfo_t TxPowerInfo;
    /**This is included if indicated presence by the pduBitmap*/
    L1PtrsInfo_t PTRS;
    /**This is included if indicated presence by the pduBitmap*/
    L1CbgFeildInfo_t CbgInfo;
    L1TxPrecodingBeamformingPdu_t BeamformingInfo;
} L1PdschPduInfo_t;

/**@brief CSI-RS PDU BWP*/
typedef struct
{
    uint8_t SubcarrierSpacing;
    uint8_t CyclicPrefix;
    uint16_t rev;
} L1CsiBwpInfo_t;

/**@brief CSI-RS PDU*/
typedef struct
{
    uint16_t csiRsPduIndex;
    uint16_t StartRB;
    uint16_t NrOfRBs;
    uint16_t FreqDomain;
    uint16_t ScrambId;
    uint8_t CSIType;
    uint8_t Row;
    uint8_t SymbL0;
    uint8_t SymbL1;
    uint8_t CDMType;
    uint8_t FreqDensity;
    L1BwpInfo_t BWP;
    L1CsiTxPowerInfo_t TxPowerInfo;
    L1TxPrecodingBeamformingPdu_t BeamformingInfo;
} L1CsiRsPduInfo_t;

/**@brief bchPayload*/
typedef struct
{
    uint32_t bchPayload;
} L1BchPayload_t;

/**@brief SSB PDU*/
typedef struct
{
    uint8_t ssbConfigIndex;
    uint8_t ssbBlockIndex;
    uint8_t bchPayloadFlag;
    uint8_t rev;
    L1BchPayload_t BchPayload;
    L1TxPrecodingBeamformingPdu_t BeamformingInfo;
} L1SsbPduInfo_t;

typedef struct
{
    union {
        /**This is included if indicated presence by the PDUType*/
        L1PdcchPduInfo_t PdcchPdu;
        /**This is included if indicated presence by the PDUType*/
        L1PdschPduInfo_t PdschPdu;
        /**This is included if indicated presence by the PDUType*/
        L1CsiRsPduInfo_t CsiRsPdu;
        /**This is included if indicated presence by the PDUType*/
        L1SsbPduInfo_t SsbPdu;
    } u;
} L1DlPduConfiguration_t;

/**@brief DL TTI PDU*/
typedef struct
{
    uint16_t PDUType;
    uint16_t PDUSize;
    L1DlPduConfiguration_t DlPDUConfig;
} L1DlPduInfo_t;

/**@brief This message can be sent by the L2/L3 when the PHY is in the RUNNING state.*/
typedef struct
{
    uint16_t SFN;
    uint16_t Slot;
    uint16_t nPDUs;
    uint8_t nDlTypes;
    uint8_t nGroup;
    uint16_t rev;
    uint16_t nPDUsOfEachType[NUM_PDU_EACH_TYPE];
    /**variable-size by nPDUs*/
    L1DlPduInfo_t pdus[]; /*Max size: MAX_NUM_TXRU*/
    /**variable-size by nGroup*/
    // L1DlGroupInfo_t groups[MAX_NUM_DL_GROUP_PER_SLOT];
} L1DLTTIReq_t;

/**@brief UL DCI PDU*/
typedef struct
{
    uint16_t PDUType;
    uint16_t PDUSize;
    L1PdcchPduInfo_t PdcchPduConfig;
} L1UlDciPduCfg_t;

/**@brief UL DCI*/
typedef struct
{
    uint16_t SFN;
    uint16_t Slot;
    uint16_t numPDUs;
    uint8_t nDlTypes;
    uint8_t rev;
    uint16_t nPDUsOfEachType[NUM_ULDCI_PDU_EACH_TYPE];
    /**variable-size by numPDUs*/
    L1UlDciPduCfg_t UldciPdu[]; /* Max size: MAX_NUM_PDCCH_PDU_FOR_UL_PER_SLOT*/
} L1ULDCI_t;

/**@brief group*/
typedef struct
{
    uint8_t nUe;
    uint8_t rev0;
    uint16_t rev1;
    /**variable-size by nUe*/
    L1GroupUeInfo_t Ues[]; /* Max size: MAX_NUM_UL_GROUP_UE*/
} L1UlGroupInfo_t;

/**@brief PUSCH DMRS*/
typedef struct
{
    uint16_t ulDmrsSymbPos;
    uint16_t puschDmrsScramblingId;
    uint16_t puschDmrsIdentity;
    uint16_t dmrsPorts;
    uint8_t maxLen;
    uint8_t dmrsConfigType;
    uint8_t nSCID;
    uint8_t numDmrsCdmGrpsNoData;
    uint8_t groupOrSequenceHopping;
    uint8_t rev0;
    uint16_t rev1;
} L1PuschDmrsInfo_t;

/**@brief Pusch Allocation in frequency domain*/
typedef struct
{
    uint16_t rbStart;
    uint16_t rbSize;
    uint16_t deltaBwp0StartFromActiveBwp;
    uint16_t initialUlBwpSize;
    uint16_t puschSecondHopPRB;
    uint16_t txDirectCurrentLocation;
    uint8_t resourceAlloc;
    uint8_t VRBtoPRBMapping;
    uint8_t puschTransType;
    uint8_t IntraSlotFrequencyHopping;
    uint8_t uplinkFrequencyShift7p5khz;
    uint8_t rev0;
    uint16_t rev1;
    uint8_t rbBitmap[NUM_PXSCH_RB_BITMAP];
} L1PuschAllocationFdInfo_t;

/**@brief Optional puschData information*/
typedef struct
{
    uint32_t TBSize;
    uint16_t numCb;
    uint8_t rvIndex;
    uint8_t harqProcessID;
    uint8_t newData;
    uint8_t cbCrcRequest;
    uint16_t rev;
    uint8_t cbPresentAndPosition[NUM_PUSCH_CB_PRESENT_AND_POSITION];
} L1PuschData_t;

/**@brief UCI part2 types*/
typedef struct
{
    uint16_t priority;
    uint16_t part2SizeMapIndex;
    uint8_t numPart1Params;
    uint8_t numPart2s;
    uint16_t reserved;
    uint16_t paramOffsets[NUM_PART1_PARAMS];
    uint8_t paramSizes[NUM_PART1_PARAMS];
} L1UCIPart2Types_t;

/**@brief UCI part1 to part2 information*/
typedef struct
{
    uint16_t numPart2Types;
    uint16_t reserved;
    /**variable-size by numPart2Types*/
    L1UCIPart2Types_t UCIpart2[];
} L1Uci1ToUci2Info_t;

/**@brief Optional puschUci information*/
typedef struct
{
    uint16_t harqAckBitLength;
    uint16_t csiPart1BitLength;
    uint16_t flagCsiPart2;
    uint8_t AlphaScaling;
    uint8_t betaOffsetHarqAck;
    uint8_t betaOffsetCsi1;
    uint8_t betaOffsetCsi2;
    uint16_t reserved;
    L1Uci1ToUci2Info_t uci1ToUci2Info;
} L1PuschUci_t;

/**@brief PUSCH PtrsPorts*/
typedef struct
{
    uint16_t PTRSPortIndex;
    uint8_t PTRSDmrsPort;
    uint8_t PTRSReOffset;
} L1PtrsPortInfo_t;

/**@brief Optional puschPtrs information*/
typedef struct
{
    uint8_t PTRSTimeDensity;
    uint8_t PTRSFreqDensity;
    uint8_t ulPTRSPower;
    uint8_t numPtrsPorts;
    /**variable-size by numPtrsPorts*/
    L1PtrsPortInfo_t PtrsPorts[]; /* Max size: MAX_NUM_PUSCH_PTRS_PORT*/
} L1PuschPtrs_t;

/**@brief Optional dftsOfdm information*/
typedef struct
{
    uint16_t lowPaprSequenceNumber;
    uint8_t lowPaprGroupNumber;
    uint8_t ulPtrsSampleDensity;
    uint8_t ulPtrsTimeDensityTransformPrecoding;
    uint8_t rev0;
    uint16_t rev1;
} L1DftsOfdm_t;

/**@brief Pucch Allocation in frequency domain*/
typedef struct
{
    uint16_t prbStart;
    uint16_t prbSize;
} L1PucchAllocationFdInfo_t;

/**@brief Hopping information*/
typedef struct
{
    uint16_t nIdPucchHopping;
    uint16_t InitialCyclicShift;
    uint16_t nIdPucchScrambling;
    uint16_t secondHopPRB;
    uint8_t intraSlotFrequencyHopping;
    uint8_t pucchGroupHopping;
    uint8_t ulBwpId;
    uint8_t timeDomainOccIdx;
    uint8_t preDftOccIdx;
    uint8_t preDftOccLen;
    uint16_t rev;
} L1PucchHoppingInfo_t;

/**@brief PUCCH PDU DMRS*/
typedef struct
{
    uint16_t nId0PucchDmrsScrambling;
    uint8_t m0PucchDmrsCyclicShift;
    uint8_t addDmrsFlag;
    L1Uci1ToUci2Info_t uci1ToUci2Info;
} L1PucchDMRSInfo_t;

/**@brief PRACH PDU.*/
typedef struct
{
    uint32_t Handle;
    uint16_t physCellID;
    uint16_t prachResConfigIndex;
    uint16_t numCs;
    uint8_t NumPrachOcas;
    uint8_t prachFormat;
    uint8_t indexFdRa;
    uint8_t numFdRa;
    uint8_t prachStartSymbol;
    uint8_t startPreambleIndex;
    uint8_t numPreambleIndices;
    uint8_t rev0;
    uint16_t rev1;
    uint32_t BeamformingInfoOffset;
} L1PrachPdu_t;

/**@brief PUSCH PDU*/
typedef struct
{
    uint32_t Handle;
    uint16_t pduBitmap;
    uint16_t RNTI;
    uint32_t tbSizeLbrmBytes;
    uint16_t targetCodeRate;
    uint16_t nIdPusch;
    uint8_t qamModOrder;
    uint8_t mcsIndex;
    uint8_t mcsTable;
    uint8_t TransformPrecoding;
    uint8_t nrOfLayers;
    uint8_t ldpcBaseGraph;
    uint16_t rev;
    uint32_t BeamformingInfoOffset;
    L1BwpInfo_t BWP;
    L1PuschDmrsInfo_t DMRS;
    L1PuschAllocationFdInfo_t ResourceAllocFd;
    L1PxschAllocationTdInfo_t ResourceAllocTd;
    /**This is included if indicated presence by the pduBitmap*/
    L1PuschData_t puschData;
    /**This is included if indicated presence by the pduBitmap*/
    L1PuschUci_t puschUci;
    /**This is included if indicated presence by the pduBitmap*/
    L1PuschPtrs_t puschPtrs;
    /**This is included if indicated presence by the pduBitmap*/
    L1DftsOfdm_t dftsOfdm;
} L1PuschPduInfo_t;

/**@brief PUCCH PDU*/
typedef struct
{
    uint32_t Handle;
    uint16_t RNTI;
    uint8_t FormatType;
    uint8_t multiSlotTxIndicator;
    uint8_t pi2Bpsk;
    uint8_t rev0;
    uint16_t harqBitLen;
    uint16_t csiPart1BitLen;
    uint8_t srBitLen;
    uint8_t combineUciReport;
    uint32_t BeamformingInfoOffset;
    L1BwpInfo_t BWP;
    L1PucchAllocationFdInfo_t ResourceAllocFd;
    L1PxschAllocationTdInfo_t ResourceAllocTd;
    L1PucchHoppingInfo_t hoppingInfo;
    L1PucchDMRSInfo_t DMRS;
} L1PucchPduInfo_t;

/**@brief srs resource allocation*/
typedef struct
{
    uint16_t sequenceId;
    uint16_t frequencyShift;
    uint16_t Tsrs;
    uint16_t Toffset;
    uint8_t numAntPorts;
    uint8_t numSymbols;
    uint8_t numRepetitions;
    uint8_t timeStartPosition;
    uint8_t configIndex;
    uint8_t bandwidthIndex;
    uint8_t combSize;
    uint8_t combOffset;
    uint8_t cyclicShift;
    uint8_t frequencyPosition;
    uint8_t frequencyHopping;
    uint8_t groupOrSequenceHopping;
    uint8_t resourceType;
    uint8_t TransformPrecoding;
    uint8_t codebookSubset;
    uint8_t SRSUsage;
    uint32_t ReportType;
    uint8_t SingularValueRepresentation;
    uint8_t IqRepresentation;
    uint16_t PrgSize;
    uint16_t SymbolPuncturingBitmap;
    uint16_t reserved;
    uint32_t BeamformingInfoOffset;
} L1SrsResource_t;

/**@brief SRS PDU*/
typedef struct
{
    uint32_t Handle;
    uint16_t RNTI;
    uint16_t reserved;
    L1SrsResource_t resource;
    L1BwpInfo_t BWP;
} L1SrsPdu_t;

typedef struct
{
    union {
        /**This is included if indicated presence by the PDUType*/
        L1PrachPdu_t PrachPdu;
        /**This is included if indicated presence by the PDUType*/
        L1PuschPduInfo_t PuschPdu;
        /**This is included if indicated presence by the PDUType*/
        L1PucchPduInfo_t PucchPdu;
        /**This is included if indicated presence by the PDUType*/
        L1SrsPdu_t SrsPdu;
    } u;
} L1UlPduConfiguration_t;

/**@brief UL TTI PDU*/
typedef struct
{
    uint16_t PDUType;
    uint16_t PDUSize;
    L1UlPduConfiguration_t UlPDUConfig;
} L1UlPduInfo_t;

/**@brief The PDUs included in this structure have no ordering requirements*/
typedef struct
{
    uint16_t SFN;
    uint16_t Slot;
    uint16_t nPDUs;
    uint8_t nUlTypes;
    uint8_t nGroup;
    uint16_t nPDUsOfEachType[NUM_PDU_EACH_TYPE];
    uint16_t rev;
    /**variable-size by nPDUs*/
    L1UlPduInfo_t pdus[]; /* Max size: MAX_NUM_PDU_IN_UL_TTI_REQ*/
    /**variable-size by nGroup*/
    // L1UlGroupInfo_t groups[MAX_NUM_GROUP_IN_UL_TTI_REQ];
} L1ULTTI_t;

/**@brief TLV structure*/
typedef struct
{
    uint16_t Tag;
    uint16_t rev;
    uint32_t Length;
    uint32_t Value;
} L1DataTLV_t;

/**@brief TX DATA PDU*/
typedef struct
{
    uint32_t PDULength;
    uint16_t PDUIndex;
    uint8_t CWIndex;
    uint8_t rev;
    uint32_t numTLV;
    /**variable-size by numTLV*/
    L1DataTLV_t TLVs[]; /* Max size: MAX_NUM_TXDATA_TLV*/
} L1TxDataPduInfo_t;

/**@brief TX DATA*/
typedef struct
{
    uint16_t SFN;
    uint16_t Slot;
    uint16_t numPDUs;
    uint16_t rev;
    /**variable-size by numPDUs*/
    L1TxDataPduInfo_t pdus[]; /* Max size: MAX_NUM_PDSCH_PDU_PER_SLOT*/
} L1TXDATAREQ_t;

/**@brief Config response*/
typedef struct
{
    uint8_t ErrorCode;
    uint8_t NumberInvalid;
    uint8_t NumberTLVsIDLE;
    uint8_t NumberTLVsRUNNING;
    uint8_t NumberMissTLVs;
    uint8_t rev0;
    uint16_t rev1;
} L1ConfigResponse_t;

/**@brief Start request*/
typedef struct
{
} L1StartRequest_t;

/**@brief Start response*/
typedef struct
{
    uint32_t ErrorCode;
} L1StartResponse_t;

/**@brief Stop request*/
typedef struct
{
} L1StopRequest_t;

/**@brief Stop indication*/
typedef struct
{
    uint32_t ErrorCode;
} L1StopIndication_t;

/**@brief Error indication info*/
typedef struct
{
    uint16_t tag;
    uint16_t length;  //  shall be always multiple of 4
    uint32_t value[]; //  MAX_NUM_ERR_INFO_VALUE_LEN
} L1ErrorInfoTLV_t;

typedef struct
{
    uint16_t SFN;
    uint16_t Slot;
    uint16_t ExpectedSFN;
    uint16_t ExpectedSlot;
    uint8_t MessageId;
    uint8_t ErrorCode;
    uint8_t NumTLVs;
    uint8_t rev;
    /**variable-size by NumTLVs */
    L1ErrorInfoTLV_t TLV[]; /* Max size: MAX_NUM_ERR_INDICATION_TLVS*/
} L1ErrorIndication_t;

/**@brief Slot indication*/
typedef struct
{
    uint16_t SFN;
    uint16_t Slot;
} L1SlotIndication_t;

/**@brief Rxdata Indication Pdu*/
typedef struct
{
    uint32_t Handle;
    uint16_t RNTI;
    uint8_t HarqID;
    uint8_t rev;
    uint32_t numTLV;
    /**variable-size by numTLV*/
    L1DataTLV_t TLVs[]; /* Max size: MAX_NUM_TXDATA_TLV*/
} L1RxDataPduInfo_t;

/**@brief Rx data indication*/
typedef struct
{
    uint16_t SFN;
    uint16_t Slot;
    uint16_t numPDUs;
    uint16_t rev;
    /**variable-size by numPDUs*/
    L1RxDataPduInfo_t pdus[]; /* Max size: MAX_NUM_ULSCH_PDU_IN_RX_DATA_INDICATION*/
} L1RxdataIndication_t;

/**@brief CRC Info*/
typedef struct
{
    uint32_t Handle;
    uint16_t RNTI;
    uint8_t HarqID;
    uint8_t TbCrcStatus;
    uint32_t TA;
    uint16_t RSSI;
    uint8_t RSRP;
    uint8_t SINR;
    uint16_t numCb;
    uint16_t rev;
    uint8_t CbCrcStatus[NUM_CB_CRC_STATUS];
} L1CrcInfo_t;

/**@brief CRC indication*/
typedef struct
{
    uint16_t SFN;
    uint16_t Slot;
    uint16_t numCRCs;
    uint16_t rev;
    /**variable-size by numCRCs*/
    L1CrcInfo_t CRCs[]; /* Max size: MAX_NUM_ULSCH_CRC_IN_CRC_INDICATION*/
} L1CrcIndication_t;

/**@brief Hard pdu for fmt234*/
typedef struct
{
    uint16_t HarqBitLen;
    uint8_t HarqCrc;
    uint8_t rev;
    uint8_t HarqPayload[NUM_HARQ_PAYLOAD];
} L1HarqPduForFmt234_t;

/**@brief CSI part1 pdu*/
typedef struct
{
    uint16_t CsiPart1BitLen;
    uint8_t CsiPart1Crc;
    uint8_t rev;
    uint8_t CsiPart1Payload[NUM_CSI_PART1_PAYLOAD];
} L1CSIPart1Pdu_t;

/**@brief TLV for CSI Part 2 PDU*/
typedef struct
{
    uint16_t tag;
    uint16_t length;
    uint32_t value;
} L1CsiPart2TLV_t;

/**@brief UCI INFO pdu*/
typedef struct
{
    uint16_t UciBitLen;
    uint8_t CrcStatus;
    uint8_t rev;
    /**variable-size by UciBitLen*/
    uint8_t uciPayload[]; /* Max size: 64*/
} L1UciInfoPdu_t;

/**@brief CSI part2 pdu*/
typedef struct
{
    uint16_t CsiPart2Len;
    uint8_t CsiPart2Crc;
    uint8_t numTLV;
    /**variable-size by numTLV*/
    L1CsiPart2TLV_t TLVs[]; /* Max size: 255*/
} L1CSIPart2Pdu_t;

/**@brief SR PDU FMT01*/
typedef struct
{
    uint8_t SRindication;
    uint8_t SRconfidenceLevel;
    uint16_t rev;
} L1SrPduFMT01_t;

/**@brief Hard pdu for fmt01*/
typedef struct
{
    uint8_t NumHarq;
    uint8_t HarqconfidenceLevel;
    uint8_t HarqValue[NUM_FMT01_HARQ_VALUE];
} L1HarqPduForFm01_t;

/**@brief Sr pdu Fmt234*/
typedef struct
{
    uint8_t SrBitLen;
    uint8_t SrPayload;
    uint16_t rev;
} L1SrPduFMT234_t;

/**@brief UCI inpusch pdu*/
typedef struct
{
    uint32_t Handle;
    uint16_t RNTI;
    uint16_t TA;
    uint16_t RSSI;
    uint8_t SINR;
    uint8_t RSRP;
    uint8_t pduBitmap;
    uint8_t rev0;
    uint16_t rev1;
    /**This is included if indicated presence by the pduBitmap*/
    L1HarqPduForFmt234_t HARQInfo;
    /**This is included if indicated presence by the pduBitmap*/
    L1CSIPart1Pdu_t CSIPart1Info;
    /**This is included if indicated presence by the pduBitmap*/
    L1CSIPart2Pdu_t CSIPart2Info;
} L1UciInPuschPdu_t;

/**@brief UCI Inpucch Pdu Fmt01*/
typedef struct
{
    uint32_t Handle;
    uint16_t RNTI;
    uint16_t TA;
    uint16_t RSSI;
    uint8_t SINR;
    uint8_t RSRP;
    uint8_t pduBitmap;
    uint8_t PucchFormat;
    uint16_t rev;
    /**This is included if indicated presence by the pduBitmap*/
    L1SrPduFMT01_t SrInfo;
    /**This is included if indicated presence by the pduBitmap*/
    L1HarqPduForFm01_t HARQInfo;
} L1UciInPucchPduFmt01_t;

/**@brief UCI Inpucch Pdu Fmt234*/
typedef struct
{
    uint32_t Handle;
    uint16_t RNTI;
    uint16_t TA;
    uint16_t RSSI;
    uint8_t SINR;
    uint8_t RSRP;
    uint8_t PucchFormat;
    uint8_t pduBitmap;
    uint16_t rev;
    /**This is included if indicated presence by the pduBitmap*/
    L1SrPduFMT234_t SrInfo;
    /**This is included if indicated presence by the pduBitmap*/
    L1HarqPduForFmt234_t HARQInfo;
    /**This is included if indicated presence by the pduBitmap*/
    L1CSIPart1Pdu_t CSIPart1Info;
    /**This is included if indicated presence by the pduBitmap*/
    L1CSIPart2Pdu_t CSIPart2Info;
    /**This is included if indicated presence by the pduBitmap*/
    L1UciInfoPdu_t UCIInfo;
} L1UciInPucchPduFmt234_t;

typedef struct
{
    union {
        /**This is included if indicated presence by the PDUType*/
        L1UciInPuschPdu_t uciInPuschPdu;
        /**This is included if indicated presence by the PDUType*/
        L1UciInPucchPduFmt01_t uciInPucchPduFmt01;
        /**This is included if indicated presence by the PDUType*/
        L1UciInPucchPduFmt234_t uciInPucchPduFmt234;
    } u;
} L1UciPduInfo_t;

/**@brief UCI Info*/
typedef struct
{
    uint16_t PDUType;
    uint16_t PDUSize;
    L1UciPduInfo_t Pdu;
} L1UciInfo_t;

/**@brief UCI indication*/
typedef struct
{
    uint16_t SFN;
    uint16_t Slot;
    uint16_t numUcis;
    uint16_t rev;
    /**variable-size by numUcis*/
    L1UciInfo_t pdus[]; /* Max size: MAX_UCI_NUM_IN_UCI_IND*/
} L1UciIndication_t;

/**@brief For each resource block*/
typedef struct
{
    uint8_t rbSINR;
    uint8_t rbRSRP;
    uint16_t rev;
} L1RbsBlock_t;

/**@brief SRS report TLV*/
typedef struct
{
    uint16_t tag;
    uint16_t reserved;
    uint32_t length;
    uint32_t value;
} L1SrsReportInfo_t;

/**@brief Srs Pdu*/
typedef struct
{
    uint32_t Handle;
    uint16_t RNTI;
    uint16_t TA;
    int16_t TAOffest;
    uint8_t SRSUsage;
    uint8_t ReportType;
    L1SrsReportInfo_t reportTLV;
} L1SrsPduInfo_t;

/**@brief SRS indication*/
typedef struct
{
    uint16_t SFN;
    uint16_t Slot;
    uint16_t controlLength;
    uint8_t numPDUs;
    uint8_t rev0;
    /**variable-size by numPDUs*/
    L1SrsPduInfo_t pdus[]; /* Max size: MAX_NUM_SRS_PDU_PER_SLOT*/
} L1SrsIndication_t;

/**@brief Beamforming report symbol*/
typedef struct
{
    uint16_t numPRGs;
    uint16_t reserved1;
    /**variable-size by numPDUs*/
    uint8_t prgSINR[]; /* Max size: MAX_NUM_PRGS_SRS*/
    /**variable-size by numPDUs*/
    // uint8_t prgRSRP[];/* Max size: MAX_NUM_PRGS_SRS*/
    /**variable-size by numPDUs%2*/
    // uint16_t reserved2[];/* Max size: 2*/
} L1BeamformingReportinfo_t;

/**@brief Beamforming report*/
typedef struct
{
    uint16_t prgSize;
    uint8_t numSymbols;
    uint8_t wideBandSINR;
    uint8_t numReportedSymbols;
    uint8_t reserved1;
    uint16_t reserved2;
    /**variable-size by numReportedSymbols*/
    L1BeamformingReportinfo_t symbols[]; /* Max size: MAX_NUM_RPT_SRS_SYMBOL*/
} L1BeamformingReport_t;

/**@brief Normalized Channel I/Q Matrix*/
typedef struct
{
    uint8_t normalizedIqRepresentation;
    uint8_t reserved1;
    uint16_t numGnbAntennaElements;
    uint16_t numUeSrsPorts;
    uint16_t prgSize;
    uint16_t numPRGs;
    uint16_t reserved2;
    /**variable-size by numPRGs*numUeSrsPorts*numGnbAntennaElements*iqSize*/
    uint16_t ArrayRepresentingChannelMatrixH[]; /* Max size: MAX_CHANNEL_MATRIX_ARRAY*/
} L1NormalizedChannelMatrix_t;

/**@brief Preamble*/
typedef struct
{
    uint32_t PreamblePwr;
    uint16_t TA;
    uint8_t preambleSinr;
    uint8_t preambleIndex;
} L1PreambleInfo_t;

/**@brief SU-MIMO recommended codebook*/
typedef struct
{
    uint16_t prgSize;
    uint8_t wideBandSNR;
    uint8_t wideBandRSRP;
    uint8_t TRI;
    uint8_t TPMI;
    uint16_t numPRGs;
    /**variable-size by numPRGs*/
    uint8_t prgSNR[]; /* Max size: MAX_NUM_PRGS_SRS*/
    /**variable-size by numPRGs*/
    // uint8_t prgRSRP[];/* Max size: MAX_NUM_PRGS_SRS*/
    /**variable-size by PnumPRGs%2*/
    // uint16_t reserved[];/* Max size: 2*/
} L1SUMIMORecommended_t;

/**@brief Rach Pdu*/
typedef struct
{
    uint16_t handle;
    uint16_t avgRssi;
    uint8_t avgSinr;
    uint8_t SymbolIndex;
    uint8_t SlotIndex;
    uint8_t raIndex;
    uint8_t numPreambles;
    uint8_t rev0;
    uint16_t rev1;
    /**variable-size by numPreamble*/
    L1PreambleInfo_t preamble[]; /* Max size: MAX_NUM_PREAMBLE_PER_PRACH_PDU_RPT*/
} L1RachPduInfo_t;

/**@brief Rach Indication*/
typedef struct
{
    uint16_t SFN;
    uint16_t Slot;
    uint8_t numPDUs;
    uint8_t rev0;
    uint16_t rev1;
    /**variable-size by numPDUs*/
    L1RachPduInfo_t pdus[]; /* Max size: MAX_NUM_PRACH_PDU_IN_PRACH_INDICATION*/
} L1RachIndication_t;

#endif
