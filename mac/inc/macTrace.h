/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef _MAC_MSG_TRACE_H
#define _MAC_MSG_TRACE_H

void traceStrInfo(char* str);
int openL1Trace();
void addPhyTraceItem(const void* data, uint32_t len);

#endif
