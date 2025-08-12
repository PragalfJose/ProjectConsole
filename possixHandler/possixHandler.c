//*************************** possixHandler ***********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//*****************************************************************************
//
// File     : possixHandler.c
// Summary  : POSSIX handling functions
// Note     : Necessary functions to control POSSIX operations
// Author   : Pragalf T Jose
// Date     : 06/08/2025
//
//*****************************************************************************

//******************************* Include Files *******************************
#include "possixHandler.h"

//******************************* Local Types *********************************

//***************************** Local Constants *******************************

//***************************** Local Variables *******************************

//****************************** Local Functions ******************************

//*************************.possixHandlerThreadCreate.*************************
//Purpose   : Create a thread
//Inputs    : routine - address of thread function
//Inputs    : arguments - arguments to thread if any (can be NULL)
//Outputs   : Thread handle if thread created
//Return    : true - Thread created, false - thread creation failed
//Notes     : Create a mutex and return its address
//*****************************************************************************
bool possixHandlerThreadCreate(pthread_t *pulThread, 
                              void *(*routine)(void*), 
                              void *pvarguments)
{
    bool blReturn = false;

    if((pulThread != NULL) && (routine != NULL))
    {
        if(pthread_create(pulThread, 
                        NULL, 
                        routine, 
                        pvarguments) == RETURN_OK)
        {
            blReturn = true;
        }
        else
        {
            printf("Thread Creation failed\r\n");
        }
    }
    else
    {
        printf("Invalid parameters in Thread creation\r\n");
    }

    return blReturn;

}

//**************************.possixHandlerThreadJoin.*************************
//Purpose   : Join thread to main thread
//Inputs    : pulThread - Handler to thread
//Outputs   : None
//Return    : true - Thread joined, false - thread join failed
//Notes     : ThreadJoin make main thread wait until thread execution 
//            completed or exited
//*****************************************************************************
bool possixHandlerThreadJoin(pthread_t pulThread)
{
    bool blReturn = false;

    if(pulThread != FALSE)
    {
        if(pthread_join(pulThread,NULL) == RETURN_OK)
        {
            blReturn = true;
        }
        else
        {
            printf("Thread join failed\r\n");
        }
    }
    else
    {
        printf("Invalid parameters in Thread joining\r\n");
    }

    return blReturn;
}

//*************************.possixHandlerThreadCancel.*************************
//Purpose   : Cancel request to a thread
//Inputs    : pulThread - Handler to thread
//Outputs   : None
//Return    : true - Thread cancel requested, false - thread cancel failed
//Notes     : Request a thred to cancel. 
//            If thread is in cancellation point thread will exit
//*****************************************************************************
bool possixHandlerThreadCancel(pthread_t pulThread)
{
    bool blReturn = false;

    if(pulThread != FALSE)
    {
        if(pthread_cancel(pulThread) == RETURN_OK)
        {
            blReturn = true;
        }
        else
        {
            printf("Thread cancel failed\r\n");
        }
    }
    else
    {
        printf("Invalid parameters in Thread cancel\r\n");
    }

    return blReturn;
}

//*************************.possixHandlerMutexCreate.**************************
//Purpose   : Create a mutex
//Inputs    : None
//Outputs   : punMutex - Address of created mutex
//Return    : true - MUtex created, false - mutex creation failed
//Notes     : Create a mutex and return its address
//*****************************************************************************
bool possixHandlerMutexCreate(pthread_mutex_t *punMutex)
{
    bool blReturn = false;

    if(punMutex != NULL)
    {
        if(pthread_mutex_init(punMutex, NULL) == RETURN_OK)
        {
            blReturn = true;
        }
    }
    else
    {
        printf("Invalide parameter in Mutex Init\r\n");
    }

    return blReturn;

}

//*************************.possixHandlerMutexDelete.**************************
//Purpose   : Delete a mutex
//Inputs    : punMutex - mutex handler
//Outputs   : None
//Return    : true - Mutex deleted, false - mutex deletion failed
//Notes     : Delete a mutex
//*****************************************************************************
bool possixHandlerMutexDelete(pthread_mutex_t *punMutex)
{
    bool blReturn = false;

    if(punMutex != NULL)
    {
        if(pthread_mutex_destroy(punMutex) == RETURN_OK)
        {
            blReturn = true;
        }
    }
    else
    {
        printf("Invalid parameters in Mutex delete\r\n");
    }

    return blReturn;

}

//**************************.possixHandlerMutexLock.***************************
//Purpose   : Lock a mutex for a shared resource
//Inputs    : punMutex - Address of mutex
//Outputs   : None
//Return    : true - Mutex locked, false - mutex lock failed
//Notes     : Try to lock given mutex. Will wait unti mutex is locked 
//          : or error returns
//*****************************************************************************
bool possixHandlerMutexLock(pthread_mutex_t *punMutex)
{
    bool blReturn = false;

    if(punMutex != NULL)
    {
        if(pthread_mutex_lock(punMutex) == RETURN_OK)
        {
            blReturn = true;
        }
        else
        {
            printf("Unabale to lock Mutex \r\n");
        }
    }
    else
    {
        printf("Mutex Does not exist in MutexLock\r\n");
    }

    return blReturn;

}

//*************************.possixHandlerMutexUnlock.**************************
//Purpose   : unlock a mutex from a shared resource
//Inputs    : punMutex - Address of mutex
//Outputs   : None
//Return    : true - Mutex unlocked, false - mutex unlock failed
//Notes     : Try to unlock given mutex.
//*****************************************************************************
bool possixHandlerMutexUnlock(pthread_mutex_t *punMutex)
{
    bool blReturn = false;

    if(punMutex != NULL)
    {
        if(pthread_mutex_unlock(punMutex) == RETURN_OK)
        {
            blReturn = true;
        }
        else
        {
            printf("Unabale to unlock Mutex \r\n");
        }
    }
    else
    {
        printf("Mutex Does not exist in MutexUnLock\r\n");
    }

    return blReturn;

}

//************************.possixHandlerSemaphoreOpen.*************************
//Purpose   : Open a semaphore
//Inputs    : pcSemName - Name of semaphore
//Inputs    : lOFlags - Optmisation Flags
//Inputs    : LModeFlags - Permission Mode flags
//Inputs    : lInitialValue - Initial value of semaphore
//Outputs   : punSemaphore - Address of semaphore
//Return    : true - Semaphore Opened, false - Semaphore open failed
//Notes     : Open a semaphore and return its address
//*****************************************************************************
bool possixHandlerSemaphoreOpen(sem_t **punSemaphore, 
                                int8 *pcSemName, 
                                int32 lOFlags, 
                                int32 lModeFlags, 
                                int32 lInitialValue)
{
    bool blReturn = false;

    if((*punSemaphore == NULL) && 
       (pcSemName != NULL) && 
       (lOFlags >= TRUE) && 
       (lModeFlags >= TRUE) && 
       (lInitialValue >= FALSE))
    {
        *punSemaphore = sem_open(pcSemName, lOFlags, lModeFlags, lInitialValue);
        if(*punSemaphore == SEM_FAILED)
        {
            sem_unlink(pcSemName);
            *punSemaphore = sem_open(pcSemName, 
                                   O_CREAT, 
                                   lModeFlags, 
                                   lInitialValue);
            if(*punSemaphore != SEM_FAILED)
            {
                blReturn = true;
            }
        }
        else
        {
            blReturn = true;
        }
    }
    else
    {
        printf("Invalid arguments in Semaphore Open\r\n");
    }

    return blReturn;

}

//***********************.possixHandlerSemaphoreClose.*************************
//Purpose   : Close a Semaphore
//Inputs    : punSemaphore - Address to the semaphore
//Inputs    : pcSemName - Name of semaphore
//Outputs   : None
//Return    : true - Semaphore Closed, false - Semaphore close failed
//Notes     : Close a Semaphore
//*****************************************************************************
bool possixHandlerSemaphoreClose(sem_t *punSemaphore, int8 *pcSemName)
{
    bool blReturn = false;
    if((punSemaphore != NULL) && (pcSemName != NULL))
    {
        if(sem_close(punSemaphore) == RETURN_OK)
        {
            if(sem_unlink(pcSemName) == RETURN_OK)
            {
                blReturn = true;
            }
        }
    }
    else
    {
        printf("Invalid arguments in Semaphore close\r\n");
    }
    
    return blReturn;

}

//************************.possixHandlerSemaphoreWait.*************************
//Purpose   : Wait for semaphore 
//Inputs    : punSemaphore - Address of Semaphore
//Outputs   : None
//Return    : true - got semaphore value decremented, false -  failed
//Notes     : None
//*****************************************************************************
bool possixHandlerSemaphoreWait(sem_t *punSemaphore)
{
    bool blReturn = false;

    if(punSemaphore != NULL)
    {
        if(sem_wait(punSemaphore) == RETURN_OK)
        {
            blReturn = true;
        }
        else
        {
            printf("Semaphore Wait Error\r\n");
        }
    }
    else
    {
        printf("Invalid arguments in Semaphore wait\r\n");
    }

    return blReturn;

}

//************************.possixHandlerSemaphorePost.*************************
//Purpose   : Post a semaphore 
//Inputs    : punSemaphore - Address of Semaphore
//Outputs   : None
//Return    : true - semaphore posted value incremented, false -  failed
//Notes     : None
//*****************************************************************************
bool possixHandlerSemaphorePost(sem_t *punSemaphore)
{
    bool blReturn = false;

    if(punSemaphore != NULL)
    {
        if(sem_post(punSemaphore) == RETURN_OK)
        {
            blReturn = true;
        }
        else
        {
            printf("Semaphore post Error\r\n");
        }
    }
    else
    {
        printf("Invalid parameters in Semaphore post\r\n");
    }

    return blReturn;

}

//*************************.possixHandlerMQueueOpen.***************************
//Purpose   : Open a Message Queue
//Inputs    : pcMQueueName - Name of message queue
//Inputs    : lOFlags - Optimisation flags
//Inputs    : lModeFlags - Permission set flags
//Inputs    : lMaxMessages - Maximum number of messages
//Inputs    : lMaxMessageLength - Maximum length of a message
//Outputs   : plMQueue - Descrpiter to message queue
//Return    : true - M Queue opened, false - M Queue open failed
//Notes     : Open a M Queue and return its descripter
//*****************************************************************************
bool possixHandlerMQueueOpen(mqd_t *plMQueue, 
                             int8 *pcMQueueName, 
                             int32 lOFlags, 
                             int32 lModeFlags, 
                             int32 lMaxMessages, 
                             int32 lMaxMessageLength)
{
    bool blReturn = false;

    if((plMQueue != NULL) && 
       (pcMQueueName !=NULL) && 
       (lOFlags >= TRUE) && 
       (lModeFlags >= TRUE) && 
       (lMaxMessages >= TRUE) && 
       (lMaxMessageLength >= TRUE))
    {
        struct mq_attr stAttributes = {
                                .mq_flags = 0,
                                .mq_maxmsg = lMaxMessages, 
                                .mq_msgsize = lMaxMessageLength, 
                                .mq_curmsgs = 0
                                };
        *plMQueue = mq_open(pcMQueueName, lOFlags, lModeFlags, &stAttributes);
        if(*plMQueue != ERROR_VALUE)
        {
            blReturn = true;
        }
        else
        {
            mq_unlink(pcMQueueName);
            *plMQueue = mq_open(pcMQueueName, 
                                lOFlags, 
                                lModeFlags, 
                                &stAttributes);
            if(*plMQueue != ERROR_VALUE)
            {
                blReturn = true;
            }
        }
    }
    else
    {
        printf("Invalid Parameters in MQueue open\r\n");
    }

    return blReturn;

}

//*************************.possixHandlerMQueueClose.**************************
//Purpose   : Close a Message queue
//Inputs    : plMQueue - Pointer to MQueue descripter
//Inputs    : pcMQueueName - Name of message queue
//Outputs   : None
//Return    : true - MQueue Closed, false - MQueue Close failed
//Notes     : Closes a Message queue
//*****************************************************************************
bool possixHandlerMQueueClose(mqd_t *plMQueue, int8 *pcMQueueName)
{
    bool blReturn = false;

    if(plMQueue != NULL && pcMQueueName != NULL)
    {
        if(mq_close(*plMQueue) == RETURN_OK)
        {
            if(mq_unlink(pcMQueueName) == RETURN_OK)
            {
                blReturn = true;
            }
        }
    }
    else
    {
        printf("Invalid Parameters in MQueue close\r\n");
    }

    return blReturn;

}

//*************************.possixHandlerMQueueSend.***************************
//Purpose   : Send message to a message queue
//Inputs    : plMQueue - Pointer to MQueue descripter
//Inputs    : pcMessageStrig - Message string
//Inputs    : lMessageLength - Length of a single message
//Inputs    : ulMessagePriority - Priority of a message
//Outputs   : None
//Return    : true - Message Send to Queue, false - Message send failed
//Notes     : None
//*****************************************************************************
bool possixHandlerMQueueSend(mqd_t *plMQueue, 
                             int8 *pcMessageString, 
                             int32 lMessageLength, 
                             uint32 ulMessagePriority)
{
    bool blReturn = false;

    if((plMQueue != NULL) && 
        (pcMessageString != NULL) && 
        (lMessageLength != FALSE) &&
        (ulMessagePriority >= TRUE))
    {
        if(mq_send(*plMQueue, 
                pcMessageString, 
                lMessageLength, 
                ulMessagePriority) == RETURN_OK)
        {
            blReturn = true;
        }
        else
        {
            printf("Message send failed \r\n");
        }
    }
    else
    {
        printf("Invalid parameters in Message Send\r\n");
    }

    return blReturn;

}

//************************.possixHandlerMQueueReceive.*************************
//Purpose   : Receive message from a message queue
//Inputs    : plMQueue - Pointer to MQueue descripter
//Inputs    : lMessageLength - Length of a single message
//Inputs    : pulMessagePriority - pointer to Priority of message (can be NULL)
//Outputs   : pcMessageString - Message string
//Return    : true - Message received from Queue, false - Receive failed
//Notes     : None
//*****************************************************************************
bool possixHandlerMQueueReceive(mqd_t *plMQueue, 
                                int8 *pcMessageString, 
                                int32 lMessageLength, 
                                uint32 *pulMessagePriority)
{
    bool blReturn = false;

    if((plMQueue != NULL) && 
        (pcMessageString != NULL) && 
        (lMessageLength != FALSE))
    {
        if(mq_receive(*plMQueue, 
                      pcMessageString, 
                      lMessageLength, 
                      pulMessagePriority) == lMessageLength)
        {
            blReturn = true;
        }
        else
        {
            printf("Message receive failed\r\n");
        }
    }

    return blReturn;

}

//***********************.possixHandlerConditionalInit.************************
//Purpose   : Initialise a conditional variable
//Inputs    : puAttribute - Attribute for conditional variable
//          : If its NULL then default values are used
//Outputs   : puConditionalVar - Address to created conditioanl variable
//Return    : true - Conditioanal variable created, false - failed to create
//Notes     : None
//*****************************************************************************
bool possixHandlerConditionalVarInit(pthread_cond_t *puConditionalVar, 
                                  const pthread_condattr_t *puAttribute)
{
    bool blReturn = false;

    if(puConditionalVar != NULL)
    {
        if(pthread_cond_init(puConditionalVar, puAttribute) == RETURN_OK)
        {
            blReturn = true;
        }
    }
    else
    {
        printf("Invalid parameters in Conditioanl variable create\r\n");
    }

    return blReturn;

}

//*********************.possixHandlerConditionalVarDelete.*********************
//Purpose   : Delete a conditional variable
//Inputs    : puConditionalVar - Address of conditional variable
//Outputs   : None
//Return    : true - Conditioanal variable deleted, false - failed to delete
//Notes     : None
//*****************************************************************************
bool possixHandlerConditionalVarDelete(pthread_cond_t *puConditionalVar)
{
    bool blReturn = false;

    if(puConditionalVar != NULL)
    {
        if(pthread_cond_destroy(puConditionalVar) == RETURN_OK)
        {
            blReturn = true;
        }
    }
    else
    {
        printf("Invalid parameters in Conditioanl variable delete\r\n");
    }

    return blReturn;

}

//**********************.possixHandlerConditionalVarWait.**********************
//Purpose   : Enter wait state untill condition met
//Inputs    : puConditionalVar - Address of conditional variable
//Inputs    : punMutex - Mutex which blocks the variable
//Outputs   : None
//Return    : true - Thread enter wait mode, false - failed
//Notes     : None
//*****************************************************************************
bool possixHandlerConditionalVarWait(pthread_cond_t *puConditionalVar, 
                                     pthread_mutex_t *punMutex)
{
    bool blReturn = false;

    if(puConditionalVar != NULL && punMutex != NULL)
    {
        if(pthread_cond_wait(puConditionalVar, punMutex) == RETURN_OK)
        {
            blReturn = true;
        }
    }
    else
    {
        printf("Invalid parameters in Conditioanl variable Wait\r\n");
    }

    return blReturn;

}

//*********************.possixHandlerConditionalVarSignal.*********************
//Purpose   : Signal a thread when Condition is met
//Inputs    : puConditionalVar - Address of conditional variable
//Outputs   : None
//Return    : true - Siganl generated, false - failed to signal
//Notes     : None
//*****************************************************************************
bool possixHandlerConditionalVarSignal(pthread_cond_t *puConditionalVar)
{
    bool blReturn = false;

    if(puConditionalVar != NULL)
    {
        if(pthread_cond_signal(puConditionalVar) == RETURN_OK)
        {
            blReturn = true;
        }
    }
    else
    {
        printf("Invalid parameters in Conditioanl Signaling\r\n");
    }

    return blReturn;

}

//***********************.possixHandlerConditionalInit.************************
//Purpose   : Initialise a conditional variable
//Inputs    : puConditionalVar - Address of conditional variable
//Outputs   : None
//Return    : true - Broadcasted to all threads, false - failed to broadcast
//Notes     : None
//*****************************************************************************
bool possixHandlerConditioanlVarBroadcast(pthread_cond_t *puConditionalVar)
{
    bool blReturn = false;

    if(puConditionalVar != NULL)
    {
        if(pthread_cond_broadcast(puConditionalVar) == RETURN_OK)
        {
            blReturn = true;
        }
    }
    else
    {
        printf("Invalid parameters in Conditioanl Broadcast\r\n");
    }

    return blReturn;

}