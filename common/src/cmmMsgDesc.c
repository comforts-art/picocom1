/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include <stdio.h>
#include <string.h>

#include "cmmMsgDesc.h"
#include <assert.h>

#define MAX_NAME_LENGTH 32
#define MAX_FORMAT_LENGTH 32
#define MAX_MEMBER_NUM 32

const size_t _varSzT[T_TYPE] = {
    sizeof(unsigned int),
    sizeof(int),
    sizeof(int64_t),
    sizeof(uint64_t),
    sizeof(int32_t),
    sizeof(uint32_t),
    sizeof(int16_t),
    sizeof(uint16_t),
    sizeof(int8_t),
    sizeof(uint8_t),
    sizeof(char),
    sizeof(CmmBool_t),
    sizeof(OctArray28_s),
    sizeof(OctString_s),
    sizeof(BitArray64_s),
    sizeof(BitString_s),
    sizeof(CmmPdu_s)
};

#include "snprintfBuf.h"

static void _snprintfInt(CmmBuf_s* buf, unsigned spare, const void* data)
{
    snprintfBuf(buf, spare, " %d", *(int*)data);
}

static void _snprintfUint(CmmBuf_s* buf, unsigned spare, const void* data)
{
    snprintfBuf(buf, spare, " %u", *(unsigned int*)data);
}

static void _snprintfU64(CmmBuf_s* buf, unsigned spare, const void* data)
{
    snprintfBuf(buf, spare, " %lu", *(uint64_t*)data);
}

static void _snprintfI64(CmmBuf_s* buf, unsigned spare, const void* data)
{
    snprintfBuf(buf, spare, " %l", *(int64_t*)data);
}

static void _snprintfU32(CmmBuf_s* buf, unsigned spare, const void* data)
{
    snprintfBuf(buf, spare, " %u", *(uint32_t*)data);
}

static void _snprintfI32(CmmBuf_s* buf, unsigned spare, const void* data)
{
    snprintfBuf(buf, spare, " %d", *(int32_t*)data);
}

static void _snprintfU16(CmmBuf_s* buf, unsigned spare, const void* data)
{
    snprintfBuf(buf, spare, " %u", *(uint16_t*)data);
}

static void _snprintfI16(CmmBuf_s* buf, unsigned spare, const void* data)
{
    snprintfBuf(buf, spare, " %d", *(int16_t*)data);
}

static void _snprintfU8(CmmBuf_s* buf, unsigned spare, const void* data)
{
    snprintfBuf(buf, spare, " %u", *(uint8_t*)data);
}

static void _snprintfI8(CmmBuf_s* buf, unsigned spare, const void* data)
{
    snprintfBuf(buf, spare, " %d", *(int8_t*)data);
}

static void _snprintfBool(CmmBuf_s* buf, unsigned spare, const void* data)
{
    const CmmBool_t* tgt = data;
    if (false == *tgt) {
        snprintfBuf(buf, spare, "false ");
    } else {
        snprintfBuf(buf, spare, "true ");
    }
}

static void _snprintfStr(CmmBuf_s* buf, unsigned spare, const char* name, const void* data)
{
    snprintfBuf(buf, spare, "<%s>%s</%s>\n", name, data, name);
}

#define SN_PRINTF_ARR_TH()                                                   \
    do {                                                                     \
        uint32_t itemCnt;                                                    \
        snprintfBuf(buf, spare, "<%s itemCnt = \"%u\">", name, tgt->octCnt); \
        for (itemCnt = 0; itemCnt < tgt->octCnt; itemCnt++) {                \
            snprintfBuf(buf, 0, "%x ", tgt->data[itemCnt]);                  \
        }                                                                    \
        snprintfBuf(buf, 0, "</%s>\n", name);                                \
    } while (0)

#define SN_PRINTF_BIT_ARR_TH()                                               \
    do {                                                                     \
        uint32_t itemCnt;                                                    \
        snprintfBuf(buf, spare, "<%s itemCnt = \"%u\">", name, tgt->bitCnt); \
        for (itemCnt = 0; itemCnt < ((tgt->bitCnt + 7) >> 3); itemCnt++) {   \
            snprintfBuf(buf, 0, "%d ", tgt->data[itemCnt]);                  \
        }                                                                    \
        snprintfBuf(buf, 0, "</%s>\n", name);                                \
    } while (0)

void _snprintfOctStr(CmmBuf_s* buf, unsigned spare, const char* name, const void* data)
{
    const OctString_s* tgt = data;
    SN_PRINTF_ARR_TH();
}

static void _snprintfOctArr28(CmmBuf_s* buf, unsigned spare, const char* name, const void* data)
{
    const OctArray28_s* tgt = data;
    SN_PRINTF_ARR_TH();
}

static void _snprintfPdu(CmmBuf_s* buf, unsigned spare, const char* name, const void* data)
{
    const CmmPdu_s* tgt = data;
    SN_PRINTF_ARR_TH();
}

static void _snprintfBitStr(CmmBuf_s* buf, unsigned spare, const char* name, const void* data)
{
    const BitString_s* tgt = data;
    SN_PRINTF_BIT_ARR_TH();
}

static void _snprintfBitArr64(CmmBuf_s* buf, unsigned spare, const char* name, const void* data)
{
    const BitArray64_s* tgt = data;
    SN_PRINTF_BIT_ARR_TH();
}

typedef void (*_SnPrintfCmmType)(CmmBuf_s* buf, unsigned spare, const void* data);
typedef void (*_SnPrintfDepType)(CmmBuf_s* buf, unsigned spare, const char* name, const void* data);

static _SnPrintfCmmType cmmPrintFun[T_TYPE] = {
    _snprintfUint,
    _snprintfInt,
    _snprintfI64,
    _snprintfU64,
    _snprintfI32,
    _snprintfU32,
    _snprintfI16,
    _snprintfU16,
    _snprintfI8,
    _snprintfU8,
    _snprintfI8,
    _snprintfBool
};

static _SnPrintfDepType depPrintf[T_TYPE - T_OCTARR28] = {
    _snprintfOctArr28,
    _snprintfOctStr,
    _snprintfBitArr64,
    _snprintfBitStr,
    _snprintfPdu
};

VarType_e getContainerRootType(const ContainerTypeDef_s* def)
{
    switch (def->type) {
    case T_REF:
        if (def->ref) {
            return getContainerRootType(def->ref);
        }
        return T_TYPE;

    case T_ALIAS:
        return def->aliasType;
    default:
        return T_TYPE;
    }
}

VarType_e getVarRootType(const VarDesc_s* def)
{
    if (T_TYPE > def->type) {
        return def->type;
    }
    ASSERT(def->def);
    return getContainerRootType(def->def);
}

void _snprintfVarItem(CmmBuf_s* out, unsigned spare,
                      const char* name, const VarDesc_s* def, const void* data)
{
    VarType_e rootType = getVarRootType(def);

    /*Cmm Def Data*/
    if (NULL == data) {
        snprintfBuf(out, spare, "NULL_ADDR\n", name);
        return;
    }

    if ((def->option > VAR_DATA_P) && (rootType < T_OCTARR28)) {
        cmmPrintFun[rootType](out, 0, data);
        return;
    }

    if (rootType < T_OCTARR28) {
        if (0 == strncmp(name, "rev", 3))
            return;
        snprintfBuf(out, spare, "<%s>", name);
        cmmPrintFun[rootType](out, 0, data);
        snprintfBuf(out, 0, "</%s>\n", name);
        return;
    }

    if (rootType < T_TYPE) {
        depPrintf[rootType - T_OCTARR28](out, spare, name, data);
        return;
    }
    snprintfData(out, spare, name, def->def, data);
}

void _snprintfVar(CmmBuf_s* out, unsigned spare, const VarDesc_s* def, const void* data)
{
    const uint32_t* objCnt;
    const void* objAddr;
    const void** pObjAddr;
    uint32_t objNum;
    uint32_t objSize;
    VarType_e rootType = getVarRootType(def);

    if (def->type < T_TYPE) {
        objSize = _varSzT[def->type];
    } else {
        objSize = def->def->size;
    }

    switch (def->option) {
    case VAR_DATA:
        _snprintfVarItem(out, spare, def->name, def, data);
        break;
    case VAR_DATA_P:
        objAddr = *(void**)data;
        _snprintfVarItem(out, spare, def->name, def, objAddr);
        break;

    case VAR_ARRAY_F:
        objAddr = data;
        objCnt = &def->aCnt;
        if (T_CHAR == rootType) {
            _snprintfStr(out, spare, def->name, objAddr);
            break;
        }

        if (rootType < T_OCTARR28) {
            snprintfBuf(out, spare, "<%s itemCnt = \"%d\">", def->name, def->aCnt);
            for (objNum = 0; objNum < def->aCnt; objNum++) {
                cmmPrintFun[rootType](out, 0, objAddr);
                objAddr += objSize;
            }

            snprintfBuf(out, 0, "</%s>\n", def->name);
            break;
        }

        snprintfBuf(out, spare, "<%s itemCnt = \"%d\">\n", def->name, def->aCnt);
        for (objNum = 0; objNum < def->aCnt; objNum++) {
            _snprintfVarItem(out, spare + 1, "item", def, objAddr);
            objAddr += objSize;
        }
        snprintfBuf(out, spare, "</%s>\n", def->name);
        break;

    case VAR_ARRAY_V:
    case VAR_ARRAY_VF:
        objCnt = data;
        objAddr = data + sizeof(void*);

        if (VAR_ARRAY_VF == def->option) {
            if (*objCnt > def->aCnt) {
                printf("ObjCnt %d, def->aCnt[%d]\n", *objCnt, def->aCnt);
                break;
            }
        }

        if (T_CHAR == rootType) {
            _snprintfStr(out, spare, def->name, objAddr);
            break;
        }

        if (rootType < T_OCTARR28) {
            snprintfBuf(out, spare, "<%s itemCnt = \"%d\">", def->name, *objCnt);
            for (objNum = 0; objNum < *objCnt; objNum++) {
                cmmPrintFun[rootType](out, 0, objAddr);
                objAddr += objSize;
            }

            snprintfBuf(out, 0, "</%s>\n", def->name);
            break;
        }

        snprintfBuf(out, spare, "<%s itemCnt = \"%d\">\n", def->name, *objCnt);
        for (objNum = 0; objNum < *objCnt; objNum++) {
            _snprintfVarItem(out, spare + 1, "item", def, objAddr);
            objAddr += objSize;
        }
        snprintfBuf(out, spare, "</%s>\n", def->name);
        break;

    case VAR_PARRAY_V:
    case VAR_PARRAY_VF:
        objCnt = data;
        pObjAddr = (const void**)(data + sizeof(void*));

        if (VAR_PARRAY_VF == def->option) {
            if (*objCnt > def->aCnt) {
                printf("ObjCnt %d, def->aCnt[%d]\n", *objCnt, def->aCnt);
                break;
            }
        }

        if (T_CHAR == rootType) {
            _snprintfStr(out, spare, def->name, *pObjAddr);
            break;
        }

        if (rootType < T_OCTARR28) {
            snprintfBuf(out, spare, "<%s itemCnt = \"%d\">", def->name, *objCnt);
            for (objNum = 0; objNum < *objCnt; objNum++) {
                cmmPrintFun[rootType](out, 0, *pObjAddr);
                pObjAddr += 1;
            }

            snprintfBuf(out, 0, "</%s>\n", def->name);
            break;
        }

        snprintfBuf(out, spare, "<%s itemCnt = \"%d\">\n", def->name, *objCnt);
        for (objNum = 0; objNum < *objCnt; objNum++) {
            _snprintfVarItem(out, spare + 1, "item", def, *pObjAddr);
            pObjAddr += 1;
        }
        snprintfBuf(out, spare, "</%s>\n", def->name);

        break;
    default:
        snprintfBuf(out, spare, "<%s>\n", def->name);
        snprintfBuf(out, spare + 1, "VarOption %d not be supported", def->option);
        snprintfBuf(out, spare, "</%s>\n", def->name);
        break;
    }
}

void snprintfData(CmmBuf_s* out, unsigned spare, const char* name, const ContainerTypeDef_s* def, const void* data)
{

    ASSERT(name);
    uint32_t t = *(uint32_t*)data;
    uint32_t varNum;
    switch (def->type) {
    case T_REF:
        snprintfData(out, spare, name, def->ref, data);
        return;
    case T_CHOICE:
        snprintfBuf(out, spare, "<%s type=\"%s\">\n", name, def->var[t].name);
        _snprintfVar(out, spare + 1, &def->var[t], data + sizeof(t));
        snprintfBuf(out, spare, "</%s>\n", name, def->var[t].name);
        break;

    case T_ENUM:
        if (t < def->varCnt) {
            snprintfBuf(out, spare, "<%s>%s</%s>\n", name, def->var[t].name, name);
        } else {
            snprintfBuf(out, spare, "<%s>value outrange(%d)</%s> ", name, t, name);
        }
        break;

    case T_ALIAS:
        if (def->aliasType < T_OCTARR28) {
            snprintfBuf(out, spare, "<%s>", name);
            cmmPrintFun[def->aliasType](out, 0, data);
            snprintfBuf(out, 0, "</%s>\n", name);
            return;
        }

        if (def->aliasType < T_TYPE) {
            depPrintf[def->aliasType - T_OCTARR28](out, spare, name, data);
            return;
        }
        break;

    case T_STRUCT:
        snprintfBuf(out, spare, "<%s>\n", name, def->var[t].name);
        for (varNum = 0; varNum < def->varCnt; varNum++) {
            _snprintfVar(out, spare + 1, &def->var[varNum], data + def->var[varNum].offset);
        }
        snprintfBuf(out, spare, "</%s>\n", name, def->var[t].name);
        break;
    default:
        break;
    }
}

#define MAX_PRINTF_DATA_STR_LEN 500000
char* genDataPrintfStr(const ContainerTypeDef_s* def, const void* data)
{
    static uint8_t printData[MAX_PRINTF_DATA_STR_LEN];
    memset(printData, 0, sizeof(printData));
    CmmBuf_s buf = { MAX_PRINTF_DATA_STR_LEN, 0, printData };
    snprintfData(&buf, 0, "Data", def, data);

    return (char*)printData;
}

void snprintfMessage(CmmBuf_s* out, const MsgSetDesc_s* desc, const cmmMsg_s* msg)
{
    /*Get Msg Container Type*/
    OctString_s body;
    body.octCnt = msg->octCnt;
    body.data = (uint8_t*)&msg->data[0];
    snprintfMessageSep(out, desc, &body, msg->msgIdx);
}

void snprintfMessageSep(CmmBuf_s* out, const MsgSetDesc_s* desc, const OctString_s* msg, uint32_t msgId)
{
    /*Get Msg Container Type*/
    ASSERT(desc);

    const MsgDesc_s* msgDef = NULL;

    uint32_t i;
    if (NULL != desc) {
        for (i = 0; i < desc->msgCnt; i++) {
            if (desc->msg[i].msgId == msgId) {
                msgDef = &desc->msg[i];
                break;
            }
        }
    }

    if (NULL == msgDef) {
        snprintfBuf(out, 0, "\nA invlalid Msg, ID: %d\n", msgId);
        _snprintfOctStr(out, 1, "msgBody", msg);
        return;
    }

    snprintfBuf(out, 0, "\nMsg-%s, ID(%d) :\n", msgDef->name, msgId);
    snprintfData(out, 1, "MsgBody", msgDef->def, msg->data);
}

/*lint -restore */
/** @} */
