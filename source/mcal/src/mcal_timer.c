/*******************************************************************************
|  File Name:  mcal_timer.c
|  Description:  Implementation of MCAL timer driver
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
#include <stddef.h>

#include "mcal_timer.h"
#include "gd32f30x_libopt.h"

/*******************************************************************************
|    Macro Definition
|******************************************************************************/

/*******************************************************************************
|    Enum Definition
|******************************************************************************/

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef struct {
    uint32_t ulTimer;
    uint16_t usPrescaler;
    uint32_t ulPeriod;
    uint32_t ulAlignedMode;
    uint32_t ulCounterDirection;
    uint32_t ulClockDivision;
    uint16_t usRepetitionCounter;
    uint8_t ucAutoStart;
    void ( *pCallBackFun )( void );
} xMcal_TimerCfg_t;

/*******************************************************************************
|    Global variables Declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local Functions Declaration
|******************************************************************************/
static void Mcal_TimerConfigPeriph( xMcal_TimerCfg_t* pxCfg );
static xMcal_TimerCfg_t* Mcal_TimerFindByPeriph( uint32_t ulTimer );

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local variables
|    Generated from source/mcal/config/mcal_timer_channels.md; do not edit GEN blocks
|******************************************************************************/
/* GEN_MCAL_TIMER_TABLE_BEGIN */
static xMcal_TimerCfg_t s_xMcal_TimerConfigTable[eMcal_TimerMaxNum] = {
    {
        TIMER5,
        2U,
        15624U,
        TIMER_COUNTER_EDGE,
        TIMER_COUNTER_UP,
        TIMER_CKDIV_DIV1,
        0U,
        0U,
        NULL,
    },
    {
        TIMER2,
        0U,
        959U,
        TIMER_COUNTER_EDGE,
        TIMER_COUNTER_UP,
        TIMER_CKDIV_DIV1,
        0U,
        0U,
        NULL,
    },
    {
        TIMER4,
        0U,
        1916U,
        TIMER_COUNTER_EDGE,
        TIMER_COUNTER_UP,
        TIMER_CKDIV_DIV1,
        0U,
        0U,
        NULL,
    },
    {
        TIMER1,
        999U,
        749U,
        TIMER_COUNTER_EDGE,
        TIMER_COUNTER_UP,
        TIMER_CKDIV_DIV1,
        0U,
        0U,
        NULL,
    },
};
/* GEN_MCAL_TIMER_TABLE_END */

/*******************************************************************************
|    Function Source Code
|******************************************************************************/
/*******************************************************************************
Name            : Mcal_TimerConfigPeriph
Parameters(in)  : pxCfg - timer configuration entry
Parameters(out) : None
Return value    : None
Description     : Configure timer prescaler, period and mode
Call By         : Mcal_TimerInit
|******************************************************************************/
static void Mcal_TimerConfigPeriph( xMcal_TimerCfg_t* pxCfg )
{
    timer_parameter_struct xTimerInit;

    timer_deinit( pxCfg->ulTimer );

    xTimerInit.prescaler = pxCfg->usPrescaler;
    xTimerInit.period = pxCfg->ulPeriod;
    xTimerInit.alignedmode = pxCfg->ulAlignedMode;
    xTimerInit.counterdirection = pxCfg->ulCounterDirection;
    xTimerInit.clockdivision = pxCfg->ulClockDivision;
    xTimerInit.repetitioncounter = pxCfg->usRepetitionCounter;
    timer_init( pxCfg->ulTimer, &xTimerInit );

    timer_interrupt_flag_clear( pxCfg->ulTimer, TIMER_INT_FLAG_UP );

    if ( pxCfg->ucAutoStart != 0U )
    {
        timer_enable( pxCfg->ulTimer );
    }
}

/*******************************************************************************
Name            : Mcal_TimerFindByPeriph
Parameters(in)  : ulTimer - timer peripheral base address
Parameters(out) : None
Return value    : pointer to config entry, or NULL if not found
Description     : Lookup timer configuration by hardware instance
Call By         : Mcal_TimerIrqHandler
|******************************************************************************/
static xMcal_TimerCfg_t* Mcal_TimerFindByPeriph( uint32_t ulTimer )
{
    uint8_t ucIndex;

    for ( ucIndex = 0U; ucIndex < (uint8_t)eMcal_TimerMaxNum; ucIndex++ )
    {
        if ( s_xMcal_TimerConfigTable[ucIndex].ulTimer == ulTimer )
        {
            return &s_xMcal_TimerConfigTable[ucIndex];
        }
    }

    return NULL;
}

/*******************************************************************************
Name            : Mcal_TimerInit
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Initialize all configured hardware timers
Call By         : Application
|******************************************************************************/
void Mcal_TimerInit( void )
{
    uint8_t ucIndex;

    for ( ucIndex = 0U; ucIndex < (uint8_t)eMcal_TimerMaxNum; ucIndex++ )
    {
        Mcal_TimerConfigPeriph( &s_xMcal_TimerConfigTable[ucIndex] );
    }
}

/*******************************************************************************
Name            : Mcal_TimerStart
Parameters(in)  : eTimer - logical timer channel
Parameters(out) : None
Return value    : None
Description     : Reset counter and enable the selected timer
Call By         : Application
|******************************************************************************/
void Mcal_TimerStart( eMcal_Timer_e eTimer )
{
    xMcal_TimerCfg_t* pxCfg;

    if ( eTimer >= eMcal_TimerMaxNum )
    {
        return;
    }

    pxCfg = &s_xMcal_TimerConfigTable[eTimer];
    timer_counter_value_config( pxCfg->ulTimer, 0U );
    timer_enable( pxCfg->ulTimer );
}

/*******************************************************************************
Name            : Mcal_TimerStop
Parameters(in)  : eTimer - logical timer channel
Parameters(out) : None
Return value    : None
Description     : Reset counter and disable the selected timer
Call By         : Application
|******************************************************************************/
void Mcal_TimerStop( eMcal_Timer_e eTimer )
{
    xMcal_TimerCfg_t* pxCfg;

    if ( eTimer >= eMcal_TimerMaxNum )
    {
        return;
    }

    pxCfg = &s_xMcal_TimerConfigTable[eTimer];
    timer_counter_value_config( pxCfg->ulTimer, 0U );
    timer_disable( pxCfg->ulTimer );
}

/*******************************************************************************
Name            : Mcal_TimerSetCallFun
Parameters(in)  : eTimer - logical timer channel
                  pCallBack - update interrupt callback, NULL to clear
Parameters(out) : None
Return value    : None
Description     : Register update interrupt callback for a timer channel
Call By         : Application
|******************************************************************************/
void Mcal_TimerSetCallFun( eMcal_Timer_e eTimer, void ( *pCallBack )( void ) )
{
    if ( eTimer >= eMcal_TimerMaxNum )
    {
        return;
    }

    s_xMcal_TimerConfigTable[eTimer].pCallBackFun = pCallBack;
}

/*******************************************************************************
Name            : Mcal_TimerIrqHandler
Parameters(in)  : ulTimer - timer peripheral base address
Parameters(out) : None
Return value    : None
Description     : Common timer update interrupt handler
Call By         : TIMERx_IRQHandler
|******************************************************************************/
void Mcal_TimerIrqHandler( uint32_t ulTimer )
{
    xMcal_TimerCfg_t* pxCfg;

    if ( RESET == timer_interrupt_flag_get( ulTimer, TIMER_INT_FLAG_UP ) )
    {
        return;
    }

    timer_interrupt_flag_clear( ulTimer, TIMER_INT_FLAG_UP );

    pxCfg = Mcal_TimerFindByPeriph( ulTimer );
    if ( ( pxCfg != NULL ) && ( pxCfg->pCallBackFun != NULL ) )
    {
        pxCfg->pCallBackFun();
    }
}

/* GEN_MCAL_TIMER_IRQ_BEGIN */
/*******************************************************************************
Name            : TIMER5_IRQHandler
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Timer update interrupt service routine
Call By         : NVIC
|******************************************************************************/
void TIMER5_IRQHandler( void )
{
    Mcal_TimerIrqHandler( TIMER5 );
}

/*******************************************************************************
Name            : TIMER2_IRQHandler
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Timer update interrupt service routine
Call By         : NVIC
|******************************************************************************/
void TIMER2_IRQHandler( void )
{
    Mcal_TimerIrqHandler( TIMER2 );
}

/*******************************************************************************
Name            : TIMER4_IRQHandler
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Timer update interrupt service routine
Call By         : NVIC
|******************************************************************************/
void TIMER4_IRQHandler( void )
{
    Mcal_TimerIrqHandler( TIMER4 );
}

/*******************************************************************************
Name            : TIMER1_IRQHandler
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Timer update interrupt service routine
Call By         : NVIC
|******************************************************************************/
void TIMER1_IRQHandler( void )
{
    Mcal_TimerIrqHandler( TIMER1 );
}
/* GEN_MCAL_TIMER_IRQ_END */
