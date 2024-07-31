/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __FEU_FAPI_H__
#define __FEU_FAPI_H__

#include <stdint.h>

#define MAX_FEU_FAPI_MSG_ONE_SLOT (1)
#define MAX_FEU_FAPI_CARRIER_NUM (2)

#define MAX_TX_NB_CHAINS (4)
#define MAX_RX_NB_CHAINS (MAX_TX_NB_CHAINS)
#define TX_NB_CHAINS_OFFSET (0)
#define RX_NB_CHAINS_OFFSET (MAX_TX_NB_CHAINS)
#define MIN_CHAIN_CARRIER_IDX (1)
#define MAX_CHAIN_CARRIER_IDX (32)

#define MAX_NUM_OF_SYMBOL_PER_SLOT 14
#define MAX_TXRX_PATTERN_SYMB_NUM (80 * 14)

#define PACK_STRUCT __attribute__((packed)) // Packed compiler instruction.

typedef enum {
    FEU_STATE_IDLE = 0,
    FEU_STATE_CONFIGURED = 1,
    FEU_STATE_RUNNING = 2
} NrFEUStateType_e;

typedef enum {
    FEU_CHAIN_DISABLE = 0,
    FEU_CHAIN_ENABLE
} NrFEUChainState_e;

typedef enum {
    FEU_FAPI_PARAM_REQUEST = 0x0400,
    FEU_FAPI_PARAM_RESPONSE,
    FEU_FAPI_CONFIG_REQUEST,
    FEU_FAPI_CONFIG_RESPONSE,
    FEU_FAPI_INDICATION,
    FEU_FAPI_SCHEDULE_REQUEST,
    FEU_FAPI_SCHEDULE_RESPONSE,
    FEU_FAPI_START_REQUEST,
    FEU_FAPI_START_RESPONSE,
    FEU_FAPI_STOP_REQUEST,
    FEU_FAPI_STOP_INDICATION,
    FEU_FAPI_ERROR_INDICATION,

    FEU_FAPI_DFE_PARAM_REQUEST = 0x0500,
    FEU_FAPI_DFE_PARAM_RESPONSE,
    FEU_FAPI_DFE_CONFIG_REQUEST,
    FEU_FAPI_DFE_CONFIG_RESPONSE,
    FEU_FAPI_DFE_CONFIG_INDICATION,
    FEU_FAPI_DFE_START_REQUEST = 0x0507,
    FEU_FAPI_DFE_START_RESPONSE,
    FEU_FAPI_DFE_STOP_REQUEST,
    FEU_FAPI_DFE_STOP_INDICATION,
    FEU_FAPI_DFE_ERROR_INDICATION,
    FEUFAPIMSGID_E_CNT
} FEUFAPIMsgId_e;

typedef enum {
    FEU_FAPI_MSG_OK,
    FEU_FAPI_INVALID_STATE,
    FEU_FAPI_INVALID_CONFIG,
    FEU_FAPI_MSG_SLOT_ERROR,
    FEU_FAPI_MSG_INVALID_FORMAT,
    FEU_FAPI_OUT_OF_SYNC,
    FEU_FAPI_MSG_EXTENDED_VENDOR = 0xFF,
    FEU_FAPI_ERRIND_ERRORCODE_E_CNT
} FEUFAPIErrorCode_e;

typedef enum {
    FEU_FAPI_TXRX_PATTERN_TLV = 0x5330,
    FEU_FAPI_DPD_TLV = 0x53C0,
    FEU_FAPI_CFR_TLV = 0x53D0,
    FEU_FAPI_TX_RFCHAIN_CFG_TLV = 0x6100,
    FEU_FAPI_RX_RFCHAIN_CFG_TLV = 0x6110,
} FEUFAPITLVCODE_e;

/**@brief L1 Msg Head*/
typedef struct
{
    uint8_t msgNum;
    uint8_t opaque;
    uint16_t rev;
} FEUMsgHdr_t;

/**@brief L1 General Msg Head*/
typedef struct
{
    uint16_t msgId;
    uint16_t cellid;
    uint32_t msgSize;
} FEUGeneralMsgHdr_t;

/**@brief TLV structure*/
typedef struct
{
    uint16_t Tag;
    uint16_t Length;
    uint8_t Value[];
} FEUDataTLV_t;

/**@brief PARAM request*/
typedef struct
{
} FEUParamRequest_t;

/**@brief PARAM response*/
typedef struct
{
    uint8_t ErrorCode;
    uint8_t NumberOfTLV;
    uint16_t rev;
    FEUDataTLV_t TLVs[];
} FEUParamResponse_t;

/**@brief Carri Configuration.*/
typedef struct
{
    uint8_t rfChaninId;
    uint8_t rfChaninCtrl;
    uint8_t bandType;
    uint16_t bandNum;
    uint8_t carrierIdx;
    uint8_t bandwidth;
    uint32_t carrierFreq;
    int16_t nominalRmsOutPower;
    int16_t nominalRmsInputPower;
    int16_t gain;
    uint8_t subcarrierSpacing;
    uint8_t cyclicPrefix;
} PACK_STRUCT FEUTXRFCHAINCFG;

typedef struct
{
    uint8_t rfChaninId;
    uint8_t rfChaninCtrl;
    uint8_t bandType;
    uint16_t bandNum;
    uint8_t carrierIdx;
    uint8_t bandwidth;
    uint32_t carrierFreq;
    int16_t gain;
    uint8_t subcarrierSpacing;
    uint8_t cyclicPrefix;
} PACK_STRUCT FEURXRFCHAINCFG;

typedef struct feu_fapi {
    uint16_t sfn;
    uint8_t slot;
    uint8_t subcarrierSpacing;
    uint8_t cyclicPrefixType;
    uint16_t patternLengthInSymbols;
    uint8_t symbolTxRxMask[];
} PACK_STRUCT FEUTXRXPATTERNCFG;

/**@brief L1ConfigRequest_t*/
typedef struct
{
    uint8_t NumberTLVs;
    uint8_t rev1;
    uint16_t rev2;
    FEUDataTLV_t TLVs[];
} PACK_STRUCT FEUConfigRequesTLV_t;
/**@brief Config response*/
typedef struct
{
    uint8_t ErrorCode;
    uint8_t NumberInvalidTLVs;
    uint8_t NumberOnlyIDLETLVs;
    uint8_t NumberOnlyRUNNINGTLVs;
    uint8_t NumberMissTLVs;
    uint8_t rev[3];
    FEUDataTLV_t TLVs[];
} PACK_STRUCT FEUConfigResponse_t;

/**@brief Start request*/
typedef struct
{
} FEUStartRequest_t;

/**@brief Start response*/
typedef struct
{
    uint8_t ErrorCode;
    uint8_t rev[3];
} FEUStartResponse_t;

/**@brief Stop request*/
typedef struct
{
} FEUStopRequest_t;

/**@brief Stop indication*/
typedef struct
{
} FEUStopIndication_t;

typedef struct
{
    uint16_t ExtendedStatusLen;
    uint8_t ExtendedStatusContents[];
} PACK_STRUCT FEUErrorIndicationExtenInfo_t;

typedef struct
{
    uint16_t Sfn;
    uint16_t Slot;
    uint16_t MsgID;
    uint8_t ErrorCode;
    uint16_t ExpectedSFN;
    uint16_t ExpectedSLOT;
    uint8_t rsv1;
    uint16_t rsv2;
    FEUErrorIndicationExtenInfo_t ExtendedStatus;
} PACK_STRUCT FEUErrorIndication_t;

typedef struct
{
    uint16_t ChainIdx;
    uint16_t Sfn;
    uint8_t Slot;
    uint8_t SubcarrierSpacing;
    uint8_t CyclicPrefixType;
    uint8_t DPDstate;
    uint16_t DPDtimeOffset;
} PACK_STRUCT FEUDPDCFG;

typedef struct
{
    uint16_t ChainIdx;
    uint16_t Sfn;
    uint8_t Slot;
    uint8_t SubcarrierSpacing;
    uint8_t CyclicPrefixType;
    uint8_t CFRstate;
    uint16_t CFRthreshold;
} PACK_STRUCT FEUCFRCFG;

typedef struct
{
    uint8_t NumberTLVs;
    uint8_t rev1;
    uint16_t rev2;
    FEUDataTLV_t TLVs[];
} PACK_STRUCT FEUDFEConfigRequesTLV_t;

typedef struct
{
    uint8_t ErrorCode;
    uint8_t NumberInvalidTLVs;
    uint8_t NumberOnlyIDLETLVs;
    uint8_t NumberOnlyRUNNINGTLVs;
    uint8_t NumberMissTLVs;
    uint8_t rev[3];
    FEUDataTLV_t TLVs[];
} PACK_STRUCT FEUDFEConfigResponse_t;

typedef struct
{
} FEUDFEParamRequest_t;

/**@brief PARAM response*/
typedef struct
{
    uint8_t ErrorCode;
    uint8_t NumberOfTLV;
    uint16_t rev;
    FEUDataTLV_t TLVs[];
} FEUDFEParamResponse_t;

typedef struct
{
} FEUDFEStartRequest_t;

/**@brief Start response*/
typedef struct
{
    uint8_t ErrorCode;
    uint8_t rev[3];
} FEUDFEStartResponse_t;

typedef struct
{
} FEUDFEStopRequest_t;

/**@brief Stop indication*/
typedef struct
{
} FEUDFEStopIndication_t;

typedef FEUErrorIndication_t FEUDFEErrorIndication_t;

#endif
