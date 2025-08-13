//**************************** ProjectConsole *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//*****************************************************************************
//
// File     : main.c
// Summary  : Fetch and process system time and print. Also toggle LED state
// Note     : None
// Author   : Pragalf T Jose
// Date     : 21/07/2025
//
//*****************************************************************************

//******************************* Include Files *******************************
#include "Common.h"
#include "Console.h"
#include "appTimer.h"
#include "appLed.h"
#include "fileHandler.h"
#include "possixOperation.h"

//******************************* Global Types ********************************
typedef struct
{
    uint8 pucData[5];
    uint8 pucUID[5];
    uint8 ucCommand;
    uint8 pucRESERVED[53];
}REQUEST;
typedef struct
{
    uint8 pucData[5];
    uint8 pucUID[5];
    uint8 ucCommand;
    uint8 ucState;
    uint8 pucRESERVED[52];
}ACKNOWLEDGE;


//***************************** Global Constants ******************************

//***************************** Global Variables ******************************
static uint32 ulDataCount = 0;
static uint8 ucBreakFlag = FALSE;

//*******************************.PollerThread.********************************
//Purpose   : Detect GPIO input state change
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : Check GPIO for input changes and notify TransportThread
//          : through Message Queue
//*****************************************************************************
void* PollerThread()
{
    uint8 ucInputValue = 0;
    static uint32 ulKeyPressCount = 0;
    REQUEST stInData = {0};
    ACKNOWLEDGE stAckData = {0};

    while(true)
    {
        #ifdef _RPIBOARD
        if(appLedRpiCheckGpioPin(GPIO_SWITCH) == true)
        {
            ucInputValue = 'T';
            memset(&stAckData, 0, sizeof(ACKNOWLEDGE));
            possixOperationInputMessageReceive(&stAckData);
            if(stAckData.ucState == 0X01)
            {
                printf("[Poller] Invalid data in Transfer\r\n");
            }
            memset(&stInData, 0, sizeof(REQUEST));
            ulKeyPressCount++;
            sprintf((char*)stInData.pucUID, "%04u", ulKeyPressCount);
            sprintf((char*)stInData.pucData, "-%c--", ucInputValue);
            stInData.ucCommand = 0X01;
            possixOperationInputMessageSend(&stInData);
            possixOperationConditionalMutexLock();
            possixOperationConditionSetValue();
            possixOperationConditionalVarBroadcast();
            possixOperationConditionalMutexUnlock();
        }
        #else
        possixOperationPollerSemaphoreWait();
        printf("[Poller] Enter a number between 1 to 10 : ");
        scanf("%c", &ucInputValue);
        getchar();
        if(ucInputValue < '0' || ucInputValue > '9')
        {
            printf("[Poller] Exit from loop\r\n");
            ucBreakFlag = TRUE;
            break;
        }
        memset(&stAckData, 0, sizeof(ACKNOWLEDGE));
        possixOperationInputMessageReceive(&stAckData);
        if(stAckData.ucState == 0X01)
        {
            printf("[Poller] Invalid data in Transfer\r\n");
        }
        memset(&stInData, 0, sizeof(REQUEST));
        ulKeyPressCount++;
        sprintf((char*)stInData.pucUID, "%04u", ulKeyPressCount);
        sprintf((char*)stInData.pucData, "-%c--", ucInputValue);
        stInData.ucCommand = 0X01;
        possixOperationInputMessageSend(&stInData);
        possixOperationConditionalMutexLock();
        possixOperationConditionSetValue();
        possixOperationConditionalVarBroadcast();
        possixOperationConditionalMutexUnlock();
        #endif //_RPIBOARD
    }

    pthread_exit((void *)0);

}

//******************************.TransportThread.******************************
//Purpose   : Transfer message from PollerThread to LoggerThread
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : Receive message from PollerThread and send acknowledge to it.
//          : Send a message to Loger thread regarding GPIO state change
//*****************************************************************************
void* TransportThread()
{
    REQUEST stReadData = {0};
    ACKNOWLEDGE stAckData = {0};

    while(TRUE)
    {
        possixOperationConditionalMutexLock();
        while(possixOperationConditionCheckValue() == false)
        {
            possixOperationConditionalVarWait();
        }
        possixOperationConditionClearValue();
        possixOperationConditionalMutexUnlock();
        memset(&stReadData, 0, sizeof(REQUEST));
        possixOperationInputMessageReceive(&stReadData);
        if((stReadData.ucCommand == 0X01) && 
           (strlen((char*)stReadData.pucUID)) &&
           (strlen((char*)stReadData.pucData)))
        {
            printf("[Transfer] UID : %s  DTA :%s  CMD : 0X%02x\r\n", 
                   stReadData.pucUID, 
                   stReadData.pucData, 
                   stReadData.ucCommand);
            stReadData.ucCommand = 0X02;
            memset(&stAckData, 0, sizeof(ACKNOWLEDGE));
            strncpy((char*)stAckData.pucUID, (char*)stReadData.pucUID, 4);
            strncpy((char*)stAckData.pucData, (char*)stReadData.pucData, 4);
            stAckData.ucCommand = 0X00;
            stAckData.ucState = 0x00;
            possixOperationInputMessageSend(&stAckData);
            memset(&stAckData, 0, sizeof(ACKNOWLEDGE));
            possixOperationOutputMessageReceive(&stAckData);
            printf("[Transfer] UID : %s  DTA :%s  CMD : 0X%02x  ST : 0X%02x", 
                   stAckData.pucUID, 
                   stAckData.pucData, 
                   stAckData.ucCommand, 
                   stAckData.ucState);
            printf("\r\n");
            possixOperationOutputMessageSend(&stReadData);
        }
        else
        {
            printf("[Transfer] Invalid Data from Poller\r\n");
            memset(&stAckData, 0, sizeof(ACKNOWLEDGE));
            stAckData.ucCommand = 0X00;
            stAckData.ucState = 0x01;
            possixOperationInputMessageSend(&stAckData);
        }
        possixOperationLoggerSemaphorePost();
    }

    pthread_exit((void *)0);

}

//******************************.LoggerThread.*********************************
//Purpose   : Controls state of GPIO according to message from TramsportThread
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : Receive a message from TransportThread and change GPIO state.
//          : Send an acknowledge to TransportThread
//*****************************************************************************
void* LoggerThread()
{
    int8 pcMessageString[MAX_STR_LEN] = {0};
    REQUEST stReadData = {0};
    ACKNOWLEDGE stAckData = {0};

    while(TRUE)
    {
        possixOperationLoggerSemaphoreWait();
        memset(&stReadData, 0, sizeof(REQUEST));
        possixOperationOutputMessageReceive(&stReadData);
        if((stReadData.ucCommand == 0X02) && 
           (strlen((char*)stReadData.pucData)) && 
           (strlen((char*)stReadData.pucUID)))
        {
            printf("[Logger] UID : %s  DTA : %s  CMD : 0X%02x\r\n", 
                   stReadData.pucUID, 
                   stReadData.pucData, 
                   stReadData.ucCommand);
            #ifdef _RP_BOARD
            appLedStateToggle(GPIO_LED);
            #else
            memset(pcMessageString, 0, sizeof(pcMessageString));
            sprintf((char*)pcMessageString,
                    "%s  : %s : 0X%02x\r\n", 
                    stReadData.pucUID, 
                    stReadData.pucData, 
                    stReadData.ucCommand);
            ulDataCount++;
            fileHandlerFileWrite(FILE_NAME,
                                 pcMessageString,
                                 DATA_SIZE,
                                 OPEN_APND);
            #endif // _RPIBOARD
            memset(&stAckData, 0, sizeof(ACKNOWLEDGE));
            strncpy((char*)stAckData.pucUID, (char*)stReadData.pucUID, 4);
            strncpy((char*)stAckData.pucData, (char*)stReadData.pucData, 4);
            stAckData.ucCommand = 0X00;
            stAckData.ucState = 0x00;
            possixOperationOutputMessageSend(&stAckData);
        }
        else
        {
            printf("[Logger] Invalid Data from Transfer\r\n");
            memset(&stAckData, 0, sizeof(ACKNOWLEDGE));
            stAckData.ucCommand = 0X00;
            stAckData.ucState = 0x01;
            possixOperationOutputMessageSend(&stAckData);
        }
        possixOperationPollerSemaphorePost();
    }

    pthread_exit((void *)0);
}

//******************************.main.*****************************************
//Purpose   : Fetch and process time from system and control Ouput of a GPIO
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : Get sytem time and print it in GMT, IST and PST.
//            Creating multi threaded system to contolling the output of a GPIO
//            when another GPIO input changesthrough IPC.
//*****************************************************************************
int main()
{
    pthread_t ulPollerThread = 0;
    pthread_t ulTransportThread = 0;
    pthread_t ulLoggerThread = 0;

    #ifdef _RPIBOARD
    bool bReturnStatus = false;
    
    bReturnStatus = appLedRpiGpioInit();
    if(bReturnStatus == false)
    {
        consolePrint((uint8*)"GPIO Initialistion Failed\r\n");
        exit(ERROR_VALUE);
    }
    #else
    fileHandlerInitialCheck();
    #endif /*_RPIBOARD*/
    possixOperationSystemInit();
    if(possixHandlerThreadCreate(&ulPollerThread, PollerThread, NULL) != true)
    {
        printf("Poller Thread creation failed\r\n");
        exit(ERROR_VALUE);
    }
    if(possixHandlerThreadCreate(&ulLoggerThread, LoggerThread, NULL) != true)
    {
        printf("Logger Thread creation failed\r\n");
        exit(ERROR_VALUE);
    }
    if(possixHandlerThreadCreate(&ulTransportThread,
                                 TransportThread,
                                 NULL) != true)
    {
        printf("Transport Thread creation failed\r\n");
        exit(ERROR_VALUE);
    }

    while(true)
    {
        // appTimerProcessTime();
        // appTimerDelay(1000);
        #ifndef _RPIBOARD
        // appLedStateToggle(LED_PIN);
        #endif /*_RPIBOARD*/
        if(ucBreakFlag == TRUE)
        {
            break;
        }
    }

    #ifdef _RPIBOARD
    appLedRpiReleaseChip();
    #endif /*_RPIBOARD*/
    possixHandlerThreadJoin(ulPollerThread);
    printf("In here1\r\n");
    possixHandlerThreadCancel(ulLoggerThread);
    printf("In here2\r\n");
    possixHandlerThreadJoin(ulLoggerThread);
    printf("In here6\r\n");
    possixHandlerThreadCancel(ulTransportThread);
    printf("In here7\r\n");
    possixHandlerThreadJoin(ulTransportThread);
    printf("In here3\r\n");
    possixOperationSystemDeinit();
    printf("In here4\r\n");

    return 0;
    
}