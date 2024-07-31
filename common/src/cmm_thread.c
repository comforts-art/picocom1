/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include <linux/sched.h>

#include "cmmDebug.h"
#include "cmmTypes.h"
#include "cmm_thread.h"

#define _MODULE MOD_CMM
#define THREAD_ERR PR_ERROR
#define THREAD_WARN PR_WARNING
#define THREAD_INFO PR_INFO
#define THREAD_DBG PR_DEBUG

/*!
  * @brief Get thread user defined parameter.
  * @param[in] pThread The pointer to thread.
  * @return The pointer to user defined parameter.
  */
inline static ThreadUsrArg_t ThreadGetUsrArg(Thread_s* pThread)
{
    return pThread->UsrArg;
}

/*------------------------Thread Management---------------------------------------------------*/
Thread_s* cmm_threadCreate(
    char* szName,
    ThreadRtn_t Rtn, ThreadUsrArg_t UsrArg,
    uint32_t Prior, uint32_t nStackSize)
{
    Thread_s* pThread;
    pthread_attr_t attr;

    int err = 0;

    pThread = malloc(sizeof(Thread_s));
    if (pThread == NULL) {
        goto ERR;
    }
    pThread->UsrArg = UsrArg;

    /* set attributes for new thread - first initialize attr 
	   * structure with default values */
    strcpy(pThread->szName, szName);
    err = pthread_attr_init(&attr);
    if (0 != err) {
        THREAD_ERR("pthread_attr_init returned: %d\n", err);
        goto ERR;
    }
#ifndef _RT_SUPPORT
    THREAD_DBG("Ignore Prior%d in none-rt env\n", Prior);
    err = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (0 != err) {
        THREAD_ERR("pthread_attr_setinheritsched returned: %d\n", err);
        goto ERR;
    }

    err = pthread_attr_setstacksize(&attr, nStackSize);
    /*err = pthread_attr_setstack(&attr, nStackSize);*/
    if (0 != err) {
        THREAD_ERR("pthread_attr_setstack returned: %d\n", err);
        goto ERR;
    }

    /* Create the thread in detached state - this will mean that thread cleans itself up once object that created it goes out of scope */
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (0 != err) {
        THREAD_ERR("pthread_attr_setdetachstate returned: %d", err);
        goto ERR;
    }
#else
    struct sched_param params;
    err = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (0 != err) {
        THREAD_ERR("pthread_attr_setinheritsched returned: %d\n", err);
        goto ERR;
    }
    err = pthread_attr_setschedpolicy(&attr, SCHED_RR);
    if (0 != err) {
        THREAD_ERR("pthread_attr_setschedpolicy returned: %d\n", err);
        goto ERR;
    }
    err = pthread_attr_setstacksize(&attr, nStackSize);
    if (0 != err) {
        THREAD_ERR("pthread_attr_setstack returned: %d\n", err);
        goto ERR;
    }

    /* This member reflects the priority that was assigned to the thread or process */
    params.sched_priority = (uint32_t)Prior;

    /* Set the thread scheduling parameters attribute in the thread attribute object attr to param */
    err = pthread_attr_setschedparam(&attr, &params);
    if (0 != err) {
        THREAD_ERR("pthread_attr_setschedparam returned: %d\n", err);
        goto ERR;
    }

    /* Create the thread in detached state - this will mean that thread cleans itself up once object that created it goes out of scope */
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (0 != err) {
        THREAD_ERR("pthread_attr_setdetachstate returned: %d\n", err);
        goto ERR;
    }
#endif
    err = pthread_create(&(pThread->Tid), &attr, Rtn, pThread);
    if (err != 0) {
        THREAD_ERR("pthread_create returned: %d\n", err);
        goto ERR;
    }

    THREAD_DBG("new tid %x for %s\n", (uint32_t)pThread->Tid, pThread->szName);

    return pThread;

ERR:
    if (err != 0) {
        pthread_attr_destroy(&attr);
    }
    if (pThread != NULL) {
        free(pThread);
    }
    return NULL;
}

void cmm_threadExit(Thread_s* pThread, ThreadRtnRet_t RtnRet)
{
    assert(pThread);
    if (pThread->szName != 0) {
        THREAD_DBG("thread(%s) %x exit\n", pThread->szName, (uint32_t)pThread->Tid);
    } else {
        THREAD_DBG("thread(%x) exit\n", (uint32_t)pThread->Tid);
    }

    free(pThread);
    pThread = NULL;
    pthread_exit(RtnRet);
}

void cmm_threadCancel(Thread_s* pThread)
{
    assert(pThread);

    pthread_t Tid = pThread->Tid;
    if (pThread->szName != 0) {
        THREAD_DBG("cancel thread(%s) %x\n", pThread->szName, (uint32_t)pThread->Tid);
    } else {
        THREAD_DBG("cancel thread(%x)\n", (uint32_t)pThread->Tid);
    }
    free(pThread);
    pThread = NULL;
    pthread_cancel(Tid);
}

void cmm_threadSleep(int mSDelay) /*time given in milliseconds*/
{
    struct timeval delay;

    delay.tv_sec = mSDelay / 1000;
    delay.tv_usec = mSDelay - (1000 * delay.tv_sec);

    if (select(0, NULL, NULL, NULL, &delay) != 0) {
        THREAD_DBG("taskDelay failed to set time of %d\n", mSDelay);
    }
}
