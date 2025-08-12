//***************************** possixHandler *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//*****************************************************************************
//
// File     : possixHandler.h
// Summary  : Includes basic possix functions
// Note     : None
// Author   : Pragalf T Jose
// Date     : 06/08/2025
//
//*****************************************************************************
#ifndef _POSSIX_HANDLER_H
#define _POSSIX_HANDLER_H

//******************************* Include Files *******************************
#include <pthread.h>
#include <semaphore.h>
#include <mqueue.h>
#include <fcntl.h>              // For O_CREAT, O_EXCL
#include <sys/stat.h>           // For mode constants
#include "Common.h"

//******************************* Global Types ********************************

//***************************** Global Constants ******************************

//***************************** Global Variables ******************************

//**************************** Forward Declarations ***************************

bool possixHandlerThreadCreate(pthread_t *pulThread, 
                              void *(*routine)(void*), 
                              void *pvarguments);
bool possixHandlerThreadJoin(pthread_t pulThread);
bool possixHandlerThreadCancel(pthread_t pulThread);
bool possixHandlerMutexCreate(pthread_mutex_t *punMutex);
bool possixHandlerMutexDelete(pthread_mutex_t *punMutex);
bool possixHandlerMutexLock(pthread_mutex_t *punMutex);
bool possixHandlerMutexUnlock(pthread_mutex_t *punMutex);
bool possixHandlerSemaphoreOpen(sem_t **punSemaphore, 
                                int8 *pcSemName, 
                                int32 lOFlags, 
                                int32 lModeFlags, 
                                int32 lInitialValue);
bool possixHandlerSemaphoreClose(sem_t *punSemaphore, int8 *pcSemName);
bool possixHandlerSemaphoreWait(sem_t *punSemaphore);
bool possixHandlerSemaphorePost(sem_t *punSemaphore);
bool possixHandlerMQueueOpen(mqd_t *plMQueue, 
                             int8 *pcMQueueName, 
                             int32 lOFlags, 
                             int32 lModeFlags, 
                             int32 lMaxMessages, 
                             int32 lMaxMessageLength);
bool possixHandlerMQueueClose(mqd_t *plMQueue, int8 *pcMQueueName);
bool possixHandlerMQueueSend(mqd_t *plMQueue, 
                            int8 *pcMessageString, 
                            int32 lMessageLength, 
                            uint32 ulMessagePriority);
bool possixHandlerMQueueReceive(mqd_t *plMQueue, 
                                int8 *pcMessageString, 
                                int32 lMessageLength, 
                                uint32 *pulMessagePriority);
bool possixHandlerConditionalVarInit(pthread_cond_t *puConditionalVar, 
                                     const pthread_condattr_t *attr);
bool possixHandlerConditionalVarDelete();
bool possixHandlerConditionalVarWait();
bool possixHandlerConditionalVarSignal();
bool possixHandlerConditioanlVarBroadcast();

#endif // _POSSIX_HANDLER_H
// EOF