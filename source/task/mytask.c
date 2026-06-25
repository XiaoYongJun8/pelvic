/*******************************************************************************
|  File Name:  mytask.c
|  Description:  Implementation of system task management
|-------------------------------------------------------------------------------
| (c) This software is the proprietary of MD.
|          All rights are reserved by MD.
|-------------------------------------------------------------------------------
| Initials      Name                   Company
| --------      --------------------   -----------------------------------------
| XYJ           XiaoYongJun            MD
|-------------------------------------------------------------------------------
|               R E V I S I O N   H I S T O R Y
|-------------------------------------------------------------------------------
| Date          Version      Author    Description
| ------------  --------     -------   -----------------------------------------
| 2026-06-25    01.00.00     XYJ       Creation
|******************************************************************************/

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "mytask.h"

#include "FreeRTOS.h"
#include "task.h"

/*******************************************************************************
|    Macro Definition
|******************************************************************************/
#define MYTASK_IDLE_STACK_SIZE  ( 128U )
#define MYTASK_IDLE_PRIORITY    ( 1U )

/*******************************************************************************
|    Enum Definition
|******************************************************************************/

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/

/*******************************************************************************
|    Global variables Declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local Functions Declaration
|******************************************************************************/
static void MyTask_IdleApp( void* pvParameters );

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/
extern void xPortSysTickHandler( void );

/*******************************************************************************
|    Function Source Code
|******************************************************************************/
/*******************************************************************************
Name            : MyTask_IdleApp
Parameters(in)  : pvParameters - unused task parameter
Parameters(out) : None
Return value    : None
Description     : Default idle application task
Call By         : MyTask_StartScheduler
|******************************************************************************/
static void MyTask_IdleApp( void* pvParameters )
{
    (void)pvParameters;

    for ( ;; )
    {
        vTaskDelay( pdMS_TO_TICKS( 1000U ) );
    }
}

/*******************************************************************************
Name            : MyTask_StartScheduler
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Create default tasks and start FreeRTOS scheduler
Call By         : main
|******************************************************************************/
void MyTask_StartScheduler( void )
{
    (void)xTaskCreate( MyTask_IdleApp,
                       "IdleApp",
                       MYTASK_IDLE_STACK_SIZE,
                       NULL,
                       MYTASK_IDLE_PRIORITY,
                       NULL );

    vTaskStartScheduler();
}

/*******************************************************************************
Name            : SysTick_Handler
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Route SysTick to FreeRTOS tick handler after scheduler start
Call By         : NVIC
|******************************************************************************/
void SysTick_Handler( void )
{
#if ( INCLUDE_xTaskGetSchedulerState == 1 )
    if ( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED )
    {
#endif
        xPortSysTickHandler();
#if ( INCLUDE_xTaskGetSchedulerState == 1 )
    }
#endif
}
