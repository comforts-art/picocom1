/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __CMMMSGDESC_H
#define __CMMMSGDESC_H

#include "cmmTypes.h"

#define MAX_DESC_NAME_LEN 64

typedef enum {
    T_UINT,
    T_INT,
    T_I64,
    T_U64,
    T_I32,
    T_U32,
    T_I16,
    T_U16,
    T_I8,
    T_U8,
    T_CHAR,
    T_BOOL,
    T_OCTARR28,
    T_OCTSTR,
    T_BITARR64,
    T_BITSTR,
    T_PDU,
    T_TYPE
} VarType_e;

typedef enum {
    T_CHOICE, /**<A Choice definition*/
    T_ENUM, /**<A ENUM Defineition*/
    T_STRUCT, /**< A Structure definition*/
    T_REF, /**<A Rename of a struct, choice or enum*/
    T_ALIAS /**< A Rename of a common type. */
} ContainerType_e;

typedef enum {
    VAR_DATA, /*One Variable*/
    VAR_DATA_P, /*Pointer to One Variable, like VarTypes_s * pVar*/
    VAR_ARRAY_F, /*Fixed Lengh Array,  VarType_s aVar[100]*/
    VAR_ARRAY_VF, /*Variable Len array with fix allocate, like uint32_t numVar, VarType_s aVar[100]*/
    VAR_ARRAY_V, /*Variable Len array, should be defined as  uint32_t numVar, VarType_s aVar[]*/
    VAR_ARRAY_P, /*A Array that used a pointer, defined as uint32_t numVar, VarType_s * pVar*/
    VAR_PARRAY_F, /*Fixed Length Array of Point to Data,  VarType_s * apVar[100]*/
    VAR_PARRAY_VF, /*Variable Len array with fix allocate, like uint32_t numVar, VarType_s *apVar[100]*/
    VAR_PARRAY_V, /*Variable Len array, should be defined as  uint32_t numVar, VarType_s *apVare[]*/
    VAR_PARRAY_P, /*A Pointer Array, define as uint32_t numVar, VarType_s ** ppVar*/
    VAR_DATA_PP /*Pointer to a Pointer, used to output a pointer, define just like VarType_s ** ppVar, This value will not be packout*/
} VarOption_e;

struct ContainerTypeDef;

/**struct for variable description*/

typedef struct VarDesc {
    /** Name of the variable*/
    char name[MAX_DESC_NAME_LEN + 1];

    /**type of the variable*/
    VarType_e type;

    /**if variable is enum, choice, structure or alias, this is pointer to the ref define of the type*/
    struct ContainerTypeDef* def;

    /**offset of the varialbe to the hdr of structure*/
    size_t offset;

    /** option of variable*/
    VarOption_e option;

    /**If the variable if a fixed array, this will be the num of element in array*/
    uint32_t aCnt;
} VarDesc_s;

/**
* Struct or Union Definiation
*/
typedef struct ContainerTypeDef {
    /**container name*/
    char name[MAX_DESC_NAME_LEN + 1];

    /**container type*/
    ContainerType_e type;

    /**Size of the struct without packet subIes*/
    uint32_t size;

    /**If type is alias, it the vartype of alias*/
    VarType_e aliasType;

    /** If dataType is T_REF, this is point to the ref name*/
    struct ContainerTypeDef* ref;

    /** Indicate How many variable defined in */
    uint32_t varCnt;

    /** Varialbe definitions*/
    VarDesc_s var[];
} ContainerTypeDef_s;

/**
* struct for a message description
*/
typedef struct
    {
    /** Message No in a module*/
    uint32_t msgId;

    /** Message Name*/
    char name[MAX_DESC_NAME_LEN + 1];

    /**A Containter types should be used*/
    const ContainerTypeDef_s* def;
} MsgDesc_s;

/**
* struct for a message set description of module
*/

typedef struct
    {
    /** module mame*/
    char name[MAX_DESC_NAME_LEN + 1];

    /** messsag num*/
    unsigned msgCnt;

    /** message description list*/
    MsgDesc_s msg[];
} MsgSetDesc_s;

/**
* struct for a the message stream forment between different module/system
*/
typedef struct
    {
    uint16_t msgIdx; /*Message index that descripted in msgSetDesc_s*/
    uint16_t octCnt; /*Number of octs of the message body. Which is from data[0] to last byte of the message*/
    uint8_t data[]; /*message body, usually, the format was described by MessageDesc_s*/
} cmmMsg_s;

#if 0

/** Pack a message def to a buf, this often used to pack the description to send to trace terminal
*
* \param def pointer message desciption.
* \param buf buffer to save pack result.
*/
void packMessageSetDef(CmmBuf_s * buf, const MsgSetDesc_s * desc);

/** unpack a message def , this often used to unpack the description in trace terminal
*
* \param def pointer message desciption.
* \param srcBuf string description of message.
*/
void unpackMessageSetDef(MsgSetDesc_s * def, const CmmBuf_s * srcBuf);

/** pack a message, used to pack a message to a bytestream and sent to a trace terminal
*
* \param def message description.
* \param msg message.
* \param buf buffer to save pack result
* \result return ture if pack ok. or else return false.
*/
int packMessage(CmmBuf_s * buf, const cmmMsg_s * msg, const MsgSetDesc_s * desc);

/** unpack a message
*
* \param def message description.
* \param srcBuf source msg buffer.
* \param pool buffer to save unpack result
* \result return unpack result.
*/
cmmMsg_s * unpackMessage( const CmmBuf_s * srcBuf, const MsgSetDesc_s * desc);
#endif

/** format a container data to a string buffer
*
* \param def container description.
* \param data container data.
* \param fmtSpare format spare.
* \param out buffer to save format result
*/
void snprintfData(CmmBuf_s* out, unsigned spare, const char* name,
                  const ContainerTypeDef_s* def, const void* data);

/**format a message to a string buffer
*
* \param def message description.
* \param msg message data.
* \param out buffer to save format result
*/
void snprintfMessage(CmmBuf_s* out, const MsgSetDesc_s* desc, const cmmMsg_s* msg);

void snprintfMessageSep(CmmBuf_s* out, const MsgSetDesc_s* desc, const OctString_s* msg, uint32_t msgId);

VarType_e getContainerRootType(const ContainerTypeDef_s* def);

VarType_e getVarRootType(const VarDesc_s* def);

void _logMsg(const MsgSetDesc_s* set, OctString_s* body, uint32_t msgIdx);

char* genDataPrintfStr(const ContainerTypeDef_s* def, const void* data);

#endif
