//************************** possixOperation **********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//*****************************************************************************
//
// File     : possixOperation.c
// Summary  : Includes necessary possix operations
// Note     : POSSIX operations which uses basic POSSIX functions
// Author   : Pragalf T Jose
// Date     : 07/08/2025
//
//*****************************************************************************

//******************************* Include Files *******************************
#include "possixOperation.h"

//******************************* Local Types *********************************

//***************************** Local Constants *******************************

//***************************** Local Variables *******************************
static mqd_t lInputMessageQueue = 0;
static mqd_t lOutputMessageQueue = 0;
static sem_t *punPollerSemaphore = NULL;
static sem_t *punLoggerSemaphore = NULL;
static sem_t *punTransferSemaphore = NULL;
static pthread_cond_t unCondition = {0};
static pthread_mutex_t unFileMutex = {0};
static pthread_mutex_t unConditionMutex ={0};
static uint8 ucConditionReady = 0;

//****************************** Local Functions ******************************

//*************************.possixOperationSystemInit.*************************
//Purpose   : Initialise possix related operations
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : INitialise wanted semaphores mutex and message queue
//*****************************************************************************
void possixOperationSystemInit()
{
    possixOperationFileMutexCreate();
    possixOperationConditionalMutexCreate();
    possixOperationInputMQueueOpen();
    possixOperationOutputMQueueOpen();
    possixOperationPollerSemaphoreOpen();
    possixOperationTransportSemaphoreOpen();
    possixOperationLoggerSemaphoreOpen();
    possixOperationConditionalVarInit();
}

//************************.possixOperationSystemDeinit.************************
//Purpose   : Deinit possix related operations
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : Deinitialise wanted semaphores mutex and message queue
//*****************************************************************************
void possixOperationSystemDeinit()
{
    possixOperationInputMQueueClose();
    possixOperationOutputMQueueClose();
    possixOperationFileMutexDelete();
    possixOperationPollerSemaphoreClose();
    possixOperationTransferSemaphoreClose();
    possixOperationLoggerSemaphoreClose();
    possixOperationConditionalVarDelete();
}

//**********************.possixOperationInputMQueueOpen.***********************
//Purpose   : Open a message queue for input threads
//Inputs    : None
//Outputs   : None
//Return    : true - MQ open success, false - MQ open failed
//Notes     : Opens a mesage queue for communication between Poller thread 
//            and transport thread
//*****************************************************************************
bool possixOperationInputMQueueOpen()
{
    bool blReturn = false;

    if(possixHandlerMQueueOpen(&lInputMessageQueue, 
                               INPUT_MQ_NAME, 
                               O_CREAT | O_RDWR | O_NONBLOCK, 
                               MODE_FLAGS,
                               MAX_MESSAGES, 
                               MAX_MSG_LEN) != true)
    {
        printf("Input Message Queue Open failed \r\n");
        exit(ERROR_VALUE);
    }
    else
    {
        blReturn = true;
    }

    return blReturn;

}

//**********************.possixOperationOutputMQueueOpen.**********************
//Purpose   : Open meaasage queue for output threads
//Inputs    : None
//Outputs   : None
//Return    : true - MQ open success, false - MQ open failed
//Notes     : Opens a mesage queue for communication between Logger thread 
//            and Transport thread
//*****************************************************************************
bool possixOperationOutputMQueueOpen()
{
    bool blReturn = false;

    if(possixHandlerMQueueOpen(&lOutputMessageQueue, 
                               OUTPUT_MQ_NAME, 
                               O_CREAT | O_RDWR | O_NONBLOCK, 
                               MODE_FLAGS,
                               MAX_MESSAGES, 
                               MAX_MSG_LEN) != true)
    {
        printf("Output Message Queue Open failed \r\n");
        exit(ERROR_VALUE);
    }
    else
    {
        blReturn = true;
    }

    return blReturn;

}

//**********************.possixOperationInputMQueueClose.**********************
//Purpose   : Close Message Queue between Poller and Transport threads
//Inputs    : None
//Outputs   : None
//Return    : true - MQueue close success, false - MQueue close failed
//Notes     : None
//*****************************************************************************
bool possixOperationInputMQueueClose()
{
    bool bleReturn = false;

    if(possixHandlerMQueueClose(&lInputMessageQueue, INPUT_MQ_NAME) != true)
    {
        printf("Failed to close Input MQueue %s\r\n",INPUT_MQ_NAME);
    }
    else
    {
        bleReturn = true;
    }

    return bleReturn;
    
}

//**********************.possixOperationOutputMQueueClose.*********************
//Purpose   : Close MQueue between Transport and Logger thread
//Inputs    : None
//Outputs   : None
//Return    : true - MQueue close success, false - MQueue close failed
//Notes     : None
//*****************************************************************************
bool possixOperationOutputMQueueClose()
{
    bool bleReturn = false;

    if(possixHandlerMQueueClose(&lOutputMessageQueue, OUTPUT_MQ_NAME) != true)
    {
        printf("Failed to close Output MQueue %s\r\n",OUTPUT_MQ_NAME);
    }
    else
    {
        bleReturn = true;
    }

    return bleReturn;
    
}

//**********************.possixOperationInputMessageSend.**********************
//Purpose   : Send Message to input MQueue
//Inputs    : pvMessage - pointer to message data
//Outputs   : None
//Return    : true - Data send to Mqueue, false - failed to send to MQueue
//Notes     : None
//*****************************************************************************
bool possixOperationInputMessageSend(void *pvMessage)
{
    bool blReturn = false;

    if(possixHandlerMQueueSend(&lInputMessageQueue, 
                                (char*)pvMessage, 
                                MAX_MSG_LEN, 
                                MSG_PRIORITY) == true)
    {
        blReturn = true;
    }

    return blReturn;

}

//*********************.possixOperationOutputMessageSend.**********************
//Purpose   : Sen message to Output MQueue
//Inputs    : pvMessage - pointer to message data
//Outputs   : None
//Return    : true - Data send to Mqueue, false - failed to send to MQueue
//Notes     : None
//*****************************************************************************
bool possixOperationOutputMessageSend(void *pvMessage)
{
    bool blReturn = false;
    int8 *pcMessageString = NULL;

    pcMessageString = (char*)pvMessage;
    if(possixHandlerMQueueSend(&lOutputMessageQueue, 
                               pcMessageString, 
                               MAX_MSG_LEN, 
                               MSG_PRIORITY) == true)
    {
        blReturn = true;
    }

    return blReturn;
}

//*********************.possixOperationInputMessageReceive.********************
//Purpose   : Receive Message from Input MQueue
//Inputs    : None
//Outputs   : pvMessage - pointer to message data
//Return    : true - Data received to Mqueue, false - failed to receive
//Notes     : None
//*****************************************************************************
bool possixOperationInputMessageReceive(void *pvMessage)
{
    bool blReturn = false;

    if(possixHandlerMQueueReceive(&lInputMessageQueue, 
                                  (char*)pvMessage, 
                                  MAX_MSG_LEN, 
                                  NULL) == true)
    {

        blReturn = true;
    }

    return blReturn;
}

//********************.possixOperationOutputMessageReceive.********************
//Purpose   : Receive message from Output Mqueue
//Inputs    : None
//Outputs   : pvMessage - pointer to message data
//Return    : true - Data received to Mqueue, false - failed to receive
//Notes     : None
//*****************************************************************************
bool possixOperationOutputMessageReceive(void *pvMessage)
{
    bool blReturn = false;

    if(possixHandlerMQueueReceive(&lOutputMessageQueue, 
                                  (char*)pvMessage, 
                                  MAX_MSG_LEN, 
                                  NULL) == true)
    {

        blReturn = true;
    }

    return blReturn;

}

//***********************.possixOperationFileMutexCreate.**********************
//Purpose   : Create a mutex for file operations
//Inputs    : None
//Outputs   : None
//Return    : true - mutex creation success, false - Mutex creation failed
//Notes     : None
//*****************************************************************************
bool possixOperationFileMutexCreate()
{
    bool blReturn = false;

    if(possixHandlerMutexCreate(&unFileMutex) != true)
    {
        printf("FileMutex creation failed \r\n");
        exit(ERROR_VALUE);
    }
    else
    {
        blReturn = true;
    }

    return blReturn;

}

//*******************.possixOperationConditionalMutexCreate.*******************
//Purpose   : Craete a mutex for Conditional variables
//Inputs    : None
//Outputs   : None
//Return    : true - mutex creation success, false - Mutex creation failed
//Notes     : None
//*****************************************************************************
bool possixOperationConditionalMutexCreate()
{
    bool blReturn = false;

    if(possixHandlerMutexCreate(&unConditionMutex) != true)
    {
        printf("ConditionalMutex creation failed \r\n");
        exit(ERROR_VALUE);
    }
    else
    {
        blReturn = true;
    }

    return blReturn;

}

//***********************.possixOperationFileMutexDelete.**********************
//Purpose   : Delete mutex for file variables
//Inputs    : None
//Outputs   : None
//Return    : true - mutex creation success, false - Mutex creation failed
//Notes     : None
//*****************************************************************************
bool possixOperationFileMutexDelete()
{
    bool bleReturn = false;

    if(possixHandlerMutexDelete(&unFileMutex) != true)
    {
        printf("File Mutex deletion failed\r\n");
    }
    else
    {
        bleReturn = true;
    }

    return bleReturn;
}

//******************.possixOperationsConditionalMutexDelete.*******************
//Purpose   : Delete mutex for Conditional variables
//Inputs    : None
//Outputs   : None
//Return    : true - mutex creation success, false - Mutex creation failed
//Notes     : None
//*****************************************************************************
bool possixOperationsConditionalMutexDelete()
{
    bool bleReturn = false;

    if(possixHandlerMutexDelete(&unConditionMutex) != true)
    {
        printf("Conditional Mutex deletion failed\r\n");
    }
    else
    {
        bleReturn = true;
    }

    return bleReturn;
}

//***********************.possixOperationFileMutexLock.************************
//Purpose   : Lock file mutex
//Inputs    : None
//Outputs   : None
//Return    : true - mutex locked for file operations, false - failed
//Notes     : None
//*****************************************************************************
bool possixOperationFileMutexLock()
{
    bool bleReturn = false;

    if(possixHandlerMutexLock(&unFileMutex) == true)
    {
        bleReturn = true;
    }

    return bleReturn;
}

//**********************.possixOperationFileMutexUnlock.***********************
//Purpose   : Unlock file mutex
//Inputs    : None
//Outputs   : None
//Return    : true - mutex unlocked, false - failed
//Notes     : None
//*****************************************************************************
bool possixOperationFileMutexUnlock()
{
    bool bleReturn = false;

    if(possixHandlerMutexUnlock(&unFileMutex) == true)
    {
        bleReturn = true;
    }

    return bleReturn;
}

//********************.possixOperationConditionalMutexLock.********************
//Purpose   : Deinit possix related operations
//Inputs    : None
//Outputs   : None
//Return    : true - mutex locked for conditional operations, false - failed
//Notes     : None
//*****************************************************************************
bool possixOperationConditionalMutexLock()
{
    bool bleReturn = false;

    if(possixHandlerMutexLock(&unConditionMutex) == true)
    {
        bleReturn = true;
    }

    return bleReturn;
}

//********************.possixOperationConditionalMutexUnlock.******************
//Purpose   : Deinit possix related operations
//Inputs    : None
//Outputs   : None
//Return    : true - mutex unlocked, false - fail
//Notes     : None
//*****************************************************************************
bool possixOperationConditionalMutexUnlock()
{
    bool bleReturn = false;

    if(possixHandlerMutexUnlock(&unConditionMutex) == true)
    {
        bleReturn = true;
    }

    return bleReturn;
}

//*********************.possixOperationPollerSemaphoreOpen.********************
//Purpose   : Open a semaphore for Poller thread
//Inputs    : None
//Outputs   : None
//Return    : true - Samaphore opened, false - semaphore open failed
//Notes     : None
//*****************************************************************************
bool possixOperationPollerSemaphoreOpen()
{
    bool bleReturn = false;
    int32 lInitialValue = TRUE;
    #ifdef _RPIBOARD
    lInitialValue = FALSE;
    #endif // _RPIBOARD

    if(possixHandlerSemaphoreOpen(&punPollerSemaphore, 
                                  POLER_SEM, 
                                  O_CREAT | O_EXCL, 
                                  MODE_FLAGS, 
                                  lInitialValue) != true)
    {
        printf("Poller Semaphore open failed \r\n");
        exit(ERROR_VALUE);
    }
    else
    {
        bleReturn = true;
    }

    return bleReturn;

}

//********************.possixOperationTransportSemaphoreOpen.******************
//Purpose   : Create a semaphore for Transfer thread
//Inputs    : None
//Outputs   : None
//Return    : true - Samaphore opened, false - semaphore open failed
//Notes     : None
//*****************************************************************************
bool possixOperationTransportSemaphoreOpen()
{
    bool bleReturn = false;

    if(possixHandlerSemaphoreOpen(&punTransferSemaphore, 
                                  TRNSFR_SEM, 
                                  O_CREAT | O_EXCL, 
                                  MODE_FLAGS, 
                                  FALSE) != true)
    {
        printf("Transfer Semaphore open failed \r\n");
        exit(ERROR_VALUE);
    }
    else
    {
        bleReturn = true;
    }

    return bleReturn;

}

//*********************.possixOperationLoggerSemaphoreOpen.********************
//Purpose   : Create a semaphore for Logger Thread
//Inputs    : None
//Outputs   : None
//Return    : true - Samaphore opened, false - semaphore open failed
//Notes     : None
//*****************************************************************************
bool possixOperationLoggerSemaphoreOpen()
{
    bool bleReturn = false;

    if(possixHandlerSemaphoreOpen(&punLoggerSemaphore, 
                                  LOGER_SEM, 
                                  O_CREAT | O_EXCL, 
                                  MODE_FLAGS, 
                                  FALSE) != true)
    {
        printf("Logger Semaphore open failed \r\n");
        exit(ERROR_VALUE);
    }
    else
    {
        bleReturn = true;
    }

    return bleReturn;
    
}

//*********************.possixOperationPollerSemaphoreClose.*******************
//Purpose   : Close semaphore for Poller thread
//Inputs    : None
//Outputs   : None
//Return    : true - Samaphore closed, false - semaphore close failed
//Notes     : None
//*****************************************************************************
bool possixOperationPollerSemaphoreClose()
{
    bool bleReturn = false;

    if(possixHandlerSemaphoreClose(punPollerSemaphore, POLER_SEM) != true)
    {
        printf("Failed to Close Poller Semaphore %s\r\n", POLER_SEM);
    }
    else
    {
        bleReturn = true;
    }

    return bleReturn;

}

//*******************.possixOperationTransferSemaphoreClose.*******************
//Purpose   : Close semaphore for Transport thread
//Inputs    : None
//Outputs   : None
//Return    : true - Samaphore closed, false - semaphore close failed
//Notes     : None
//*****************************************************************************
bool possixOperationTransferSemaphoreClose()
{
    bool bleReturn = false;

    if(possixHandlerSemaphoreClose(punTransferSemaphore, TRNSFR_SEM) != true)
    {
        printf("Failed to Close Transfer Semaphore %s\r\n", TRNSFR_SEM);
    }
    else
    {
        bleReturn = true;
    }

    return bleReturn;

}

//*********************.possixOperationLoggerSemaphoreClose.*******************
//Purpose   : Close semaphore for Logger thread
//Inputs    : None
//Outputs   : None
//Return    : true - Samaphore closed, false - semaphore close failed
//Notes     : None
//*****************************************************************************
bool possixOperationLoggerSemaphoreClose()
{
    bool bleReturn = false;

    if(possixHandlerSemaphoreClose(punLoggerSemaphore, LOGER_SEM) != true)
    {
        printf("Failed to Close Logger Semaphore %s\r\n", LOGER_SEM);
    }
    else
    {
        bleReturn = true;
    }

    return bleReturn;

}

//*********************.possixOperationPollerSemaphoreWait.********************
//Purpose   : Wait for poller Semaphore
//Inputs    : None
//Outputs   : None
//Return    : true - Semaphore taken, false - failed
//Notes     : None
//*****************************************************************************
bool possixOperationPollerSemaphoreWait()
{
    bool bleReturn = false;

    if(possixHandlerSemaphoreWait(punPollerSemaphore) == true)
    {
        bleReturn = true;
    }
    else
    {
        printf("Poller Semaphore Wait Failed\r\n");
    }

    return bleReturn;
    
}

//********************.possixOperationTransportSemaphoreWait.******************
//Purpose   : Deinit possix related operations
//Inputs    : None
//Outputs   : None
//Return    : true - Semaphore taken, false - failed
//Notes     : None
//*****************************************************************************
bool possixOperationTransportSemaphoreWait()
{
    bool bleReturn = false;

    if(possixHandlerSemaphoreWait(punTransferSemaphore) == true)
    {
        bleReturn = true;
    }
    else
    {
        printf("Poller Semaphore Wait Failed\r\n");
    }

    return bleReturn;
}

//*********************.possixOperationLoggerSemaphoreWait.********************
//Purpose   : Deinit possix related operations
//Inputs    : None
//Outputs   : None
//Return    : true - Semaphore taken, false - failed
//Notes     : None
//*****************************************************************************
bool possixOperationLoggerSemaphoreWait()
{
    bool bleReturn = false;

    if(possixHandlerSemaphoreWait(punLoggerSemaphore) == true)
    {
        bleReturn = true;
    }
    else
    {
        printf("Poller Semaphore Wait Failed\r\n");
    }

    return bleReturn;
}

//*********************.possixOperationPollerSemaphorePost.********************
//Purpose   : Release Poller Semaphore
//Inputs    : None
//Outputs   : None
//Return    : true - Semaphore released, false - failed
//Notes     : None
//*****************************************************************************
bool possixOperationPollerSemaphorePost()
{
    bool bleReturn = false;

    if(possixHandlerSemaphorePost(punPollerSemaphore) == true)
    {
        bleReturn = true;
    }
    else
    {
        printf("Poller Semaphore Wait Failed\r\n");
    }

    return bleReturn;

}

//********************.possixOperationTransportSemaphorePost.******************
//Purpose   : Post Transport semaphore
//Inputs    : None
//Outputs   : None
//Return    : true - Semaphore released, false - failed
//Notes     : None
//*****************************************************************************
bool possixOperationTransportSemaphorePost()
{
    bool bleReturn = false;

    if(possixHandlerSemaphorePost(punTransferSemaphore) == true)
    {
        bleReturn = true;
    }
    else
    {
        printf("Poller Semaphore Wait Failed\r\n");
    }

    return bleReturn;

}

//*********************.possixOperationLoggerSemaphorePost.********************
//Purpose   : Post Logger semaphore
//Inputs    : None
//Outputs   : None
//Return    : true - Semaphore released, false - failed
//Notes     : None
//*****************************************************************************
bool possixOperationLoggerSemaphorePost()
{
    bool bleReturn = false;

    if(possixHandlerSemaphorePost(punLoggerSemaphore) == true)
    {
        bleReturn = true;
    }
    else
    {
        printf("Poller Semaphore Wait Failed\r\n");
    }

    return bleReturn;

}

//*********************.possixOperationConditionalVarInit.*********************
//Purpose   : Initialise a conditional variable
//Inputs    : None
//Outputs   : None
//Return    : true - Conditional varibale crated, false - failed to create
//Notes     : None
//*****************************************************************************
bool possixOperationConditionalVarInit()
{
    bool blReturn = false;

    if(possixHandlerConditionalVarInit(&unCondition, NULL) == true)
    {
        blReturn = true;
    }
    else
    {
        printf("conditional variable initialisation failef\r\n");
    }

    return blReturn;

}

//********************.possixOperationConditionalVarDelete.********************
//Purpose   : Delete a conditional variable
//Inputs    : None
//Outputs   : None
//Return    : true - Conditional varibale deleted, false - failed to delete
//Notes     : None
//*****************************************************************************
bool possixOperationConditionalVarDelete()
{
    bool blReturn = false;

    if(possixHandlerConditionalVarDelete(&unCondition) == true)
    {
        blReturn = true;
    }
    else
    {
        printf("conditional variable initialisation failef\r\n");
    }

    return blReturn;

}

//*********************.possixOperationConditionalVarWait.*********************
//Purpose   : Wait for a conditional to met
//Inputs    : None
//Outputs   : None
//Return    : true - Enter to wait state, false - failed
//Notes     : None
//*****************************************************************************
bool possixOperationConditionalVarWait()
{
    bool blReturn = false;

    if(possixHandlerConditionalVarWait(&unCondition, &unConditionMutex) == true)
    {
        blReturn = true;
    }
    else
    {
        printf("conditional variable initialisation failef\r\n");
    }

    return blReturn;

}

//********************.possixOperationConditionalVarSignal.********************
//Purpose   : Signal Waiting thread id conditions Met
//Inputs    : None
//Outputs   : None
//Return    : true - Signalling Success, false - failed to signal
//Notes     : None
//*****************************************************************************
bool possixOperationConditionalVarSignal()
{
    bool blReturn = false;

    if(possixHandlerConditionalVarSignal(&unCondition) == true)
    {
        blReturn = true;
    }
    else
    {
        printf("conditional variable initialisation failef\r\n");
    }

    return blReturn;

}

//*******************.possixOperationConditionalVarBroadcast.******************
//Purpose   : Broadcast to every thread when condition met
//Inputs    : None
//Outputs   : None
//Return    : true - Broadcast success, false - failed to broadcast
//Notes     : None
//*****************************************************************************
bool possixOperationConditionalVarBroadcast()
{
    bool blReturn = false;

    if(possixHandlerConditioanlVarBroadcast(&unCondition) == true)
    {
        blReturn = true;
    }
    else
    {
        printf("conditional variable initialisation failef\r\n");
    }

    return blReturn;

}

//**********************.possixOperationConditionSetValue.*********************
//Purpose   : Set ucConditionReady
//Outputs   : None
//Return    : true - Value set, false - failed to set
//Notes     : None
//*****************************************************************************
bool possixOperationConditionSetValue()
{
    bool blReturn = false;

    if(ucConditionReady == FALSE)
    {
        ucConditionReady = 1;
        blReturn = true;
    }

    return blReturn;
}

//*********************.possixOperationConditionClearValue.********************
//Purpose   : Set ucConditionReady
//Inputs    : None
//Outputs   : None
//Return    : true - ucConditionReady is cleared, false - failed to clear
//Notes     : None
//*****************************************************************************
bool possixOperationConditionClearValue()
{
    bool blReturn = false;

    if(ucConditionReady == TRUE)
    {
        ucConditionReady = 0;
        blReturn = true;
    }

    return blReturn;

}

//*********************.possixOperationConditionCheckValue.********************
//Purpose   : Check ucConditionReady
//Inputs    : None
//Outputs   : None
//Return    : true -ucConditionReady is set, false - ucConditionReady is cleard
//Notes     : None
//*****************************************************************************
bool possixOperationConditionCheckValue()
{
    bool blReturn = false;

    if(ucConditionReady == TRUE)
    {
        blReturn = true;
    }

    return blReturn;
}
