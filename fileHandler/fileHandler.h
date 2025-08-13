//****************************** fileHandler **********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//*****************************************************************************
//
// File     : fileHandler.h
// Summary  : Includes necessary file operations
// Note     : None
// Author   : Pragalf T Jose
// Date     : 05/08/2025
//
//*****************************************************************************
#ifndef _FILE_HANDLER_H
#define _FILE_HANDLER_H

//******************************* Include Files *******************************
#include "Common.h"

//******************************* Global Types ********************************
#define MAX_VALUE               999
#define MIN_VALUE               1
#define STRING_LEN              64
#define MAX_STR_LEN             256
#define FILE_NAME               "TestDoc.txt"
#define DATA_SIZE               21

//***************************** Global Constants ******************************
#define OPEN_WR                 "w"
#define OPEN_RD                 "r"
#define OPEN_APND               "a"
#define OPEN_W_PLUS             "w+"
#define OPEN_R_PLUS             "r+"
#define OPEN_A_PLUS             "a+"

//***************************** Global Variables ******************************

//**************************** Forward Declarations ***************************
bool fileHandlerFileOpen(FILE **pstFileHandle, 
                         int8 *pcFileName, 
                         int8 *pcFileOpenMode);
bool fileHandlerFileClose(FILE **pstFileHandle);
bool fileHandlerFileWrite(int8 *pcFileName, 
                          int8 *pcWriteData, 
                          uint32 ulWriteSize, 
                          int8 *pcWriteMode);
bool fileHandlerFileWriteInPosition(int8 *pcFileName, 
                                    int8 *pcWriteData, 
                                    uint32 ulWriteSize, 
                                    uint32 ulPosition);
bool fileHandlerFileRead(int8 *pcFileName, 
                         int8 *pcReadData, 
                         uint32 ulReadSize, 
                         int8 *pcReadMode);
bool fileHandlerFileReadFromPosition(int8 *pcFileName, 
                                     int8 *pcReadData, 
                                     uint32 ulReadSize, 
                                     uint32 ulPosition);
bool fileHandlerFileRemove(int8 *pcFileName);
bool fileHandlerFileCreate(int8 *pcFileName);
bool fileHandlerFileCheckExist(int8 *pcFileName);
bool fileHandlerFilePointerPosition(FILE ** pstFileHandle, int32 *plPosition);
bool fileHandlerFileSeek(FILE **pstFileHandle, int32 lOffset, int32 lPosition);
bool fileHandlerInitialCheck();

#endif // _FILE_HANDLER_H
// EOF
