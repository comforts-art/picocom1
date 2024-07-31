/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef _CMM_THREADLIB_H_
#define _CMM_THREADLIB_H_
#include <pthread.h>

#include "cmmTypes.h"

#define THREAD_PRIORITY_LOWEST 0
#define THREAD_PRIORITY_BELOW_NORMAL 0
#define THREAD_PRIORITY_NORMAL 0
#define THREAD_PRIORITY_ABOVE_NORMAL 1 /* double-check linux thread priorities*/
#define THREADRETURNTYPE void *

/*------------------------Thread Management---------------------------------------------------*/

/*
   * The definition of the thread's id. 
   */
typedef pthread_t ThreadId_t;

/* The definition of the return value of the executing thread */
typedef void* ThreadRtnRet_t;

/*
   The type of user's parameter  
   */
typedef void* ThreadUsrArg_t;

/**
  * The definition of the thread struct.
  **/
typedef struct Thread {

    ThreadId_t Tid; /**< Thread id. */
    ThreadUsrArg_t UsrArg; /**< user defined parameter */
    char szName[64]; /**< the name of thread */
} Thread_s;

/* The definition of the thread's routine type*/
typedef void* (*ThreadRtn_t)(void*);

/**
  * Create a thread.
  * @param[in] szName The thread name.  
  * @param[in] Rtn The start adrress of the thread's executing routine. 
  * @param[in] UsrArg The parameter defined by user passed to the executing routine.
  * @param[in] Prior The priority of the new thread.
  * @param[in] nStackSize The stack size of the new thread. If this parameter is '0',
  *	it will use default value.
  * @return The pointer to the new thread struct that will be used by other 
  thread function. 
  * */
Thread_s* cmm_threadCreate(
    char* szName,
    ThreadRtn_t Rtn, ThreadUsrArg_t UsrArg,
    uint32_t Prior, uint32_t nStackSize);

/**
  * Exit from a executing thread routine.
  * @param[in] pThread The pointer to the thread struct. 
  * @param[in] RtnRet The exit value of the thread, it is a pointer.
  **/
void cmm_threadExit(Thread_s* pThread, ThreadRtnRet_t RtnRet);

/**
  * Delay specified milliseconds in thread.
  * @param[in] delay The time given in milliseconds.
  **/
void cmm_threadSleep(int delay);

/**
  * cancel a executing thread routine.
  * @param[in] pThread The pointer to the thread struct. 
  **/
void cmm_threadCancel(Thread_s* pThread);

#endif /*_Thread_H_*/
