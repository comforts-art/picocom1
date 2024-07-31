/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __TTI_CMM_H__
#define __TTI_CMM_H__
#include "l1_api.h"

static inline L1SlotIndication_t gsfnShift(L1SlotIndication_t gsfn, uint32_t shift, uint32_t scs)
{
    uint32_t mask;

    if (scs == 15)
        mask = 10;
    else if (scs == 30)
        mask = 20;
    else if (scs == 60)
        mask = 40;
    else if (scs == 120)
        mask = 80;
    ASSERT(shift < mask);
    L1SlotIndication_t rtv = gsfn;

    if ((shift + gsfn.Slot) < mask) {
        rtv.Slot += shift;
    } else {
        rtv.Slot = rtv.Slot + shift - mask;
        rtv.SFN = (rtv.SFN + 1) & 0x3ff;
    }
    return rtv;
}

#endif
