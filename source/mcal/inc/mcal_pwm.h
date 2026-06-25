/*******************************************************************************
|  File Name:  mcal_pwm.h
|  Description:  Head file of MCAL PWM driver
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

#ifndef _MCAL_PWM_H_
#define _MCAL_PWM_H_

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
|    Generated from source/mcal/config/mcal_pwm_channels.md; edit GEN block only
|    via tools/gen_mcal_pwm.py (enum snippet printed, not auto-written here).
|******************************************************************************/
/* GEN_MCAL_PWM_ENUM_BEGIN */
typedef enum {
    eMcal_PwmPump = 0,
    eMcal_PwmMaxNum,
} eMcal_Pwm_e;
/* GEN_MCAL_PWM_ENUM_END */

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef struct {
    uint32_t ulTimer;
    uint16_t usChannel;
    uint16_t usPrescaler;
    uint16_t usPeriod;
    uint32_t ulAlignedMode;
    uint32_t ulCounterDirection;
    uint32_t ulClockDivision;
    uint16_t usRepetitionCounter;
    uint8_t ucAutoStart;
} xMcal_PwmCfg_t;

/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
void Mcal_PwmInit( void );
void Mcal_PwmStart( eMcal_Pwm_e ePwm );
void Mcal_PwmStop( eMcal_Pwm_e ePwm );
void Mcal_PwmSetDuty( eMcal_Pwm_e ePwm, uint32_t ulDuty );
uint8_t Mcal_PwmSetFreqDuty( eMcal_Pwm_e ePwm, uint16_t usFreq, uint32_t ulPwUs );
uint8_t Mcal_PwmSetPw( eMcal_Pwm_e ePwm, uint32_t ulPwUs );

#ifdef __cplusplus
}
#endif

#endif /* _MCAL_PWM_H_ */
