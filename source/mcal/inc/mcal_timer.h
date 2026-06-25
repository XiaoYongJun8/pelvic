/*******************************************************************************
|  File Name:  mcal_timer.h
|  Description:  Head file of MCAL timer driver
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

#ifndef _MCAL_TIMER_H_
#define _MCAL_TIMER_H_

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
|    Macro Definition
|******************************************************************************/

/*******************************************************************************
|    Enum Definition
|    Generated from source/mcal/config/mcal_timer_channels.md; edit GEN block only
|    via tools/gen_mcal_timer.py (enum snippet printed, not auto-written here).
|******************************************************************************/
/* GEN_MCAL_TIMER_ENUM_BEGIN */
typedef enum {
    eMcal_TimerEmg = 0,
    eMcal_TimerStim,
    eMcal_TimerStimOff,
    eMcal_TimerPress,
    eMcal_TimerMaxNum,
} eMcal_Timer_e;
/* GEN_MCAL_TIMER_ENUM_END */

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/

/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
void Mcal_TimerInit( void );
void Mcal_TimerStart( eMcal_Timer_e eTimer );
void Mcal_TimerStop( eMcal_Timer_e eTimer );
void Mcal_TimerSetCallFun( eMcal_Timer_e eTimer, void ( *pCallBack )( void ) );
void Mcal_TimerIrqHandler( uint32_t ulTimer );

#ifdef __cplusplus
}
#endif

#endif /* _MCAL_TIMER_H_ */
