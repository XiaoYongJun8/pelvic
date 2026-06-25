/*******************************************************************************
|  File Name:  mcal_adc.h
|  Description:  Head file of MCAL ADC driver
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

#ifndef _MCAL_ADC_H_
#define _MCAL_ADC_H_

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
|    Generated from source/mcal/config/mcal_adc_channels.md; edit GEN block only
|    via tools/gen_mcal_adc.py (enum snippet printed, not auto-written here).
|******************************************************************************/
/* GEN_MCAL_ADC_ENUM_BEGIN */
typedef enum {
    eMcal_AdcCh0 = 0,
    eMcal_AdcMaxNum,
} eMcal_AdcCh_e;
/* GEN_MCAL_ADC_ENUM_END */

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef struct {
    uint32_t ulAdcPort;
    uint32_t ulMode;
    uint32_t ulResolution;
    uint32_t ulScanMode;
    uint32_t ulContinuousMode;
    uint32_t ulDataAlign;
} xMcal_AdcInstCfg_t;

/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
void Mcal_AdcInit( void );
void Mcal_AdcDeInit( void );
uint16_t Mcal_GetAdcChValue( eMcal_AdcCh_e eCh );
void Mcal_AdcSetCallFun( void ( *pCallBack )( void ) );

#ifdef __cplusplus
}
#endif

#endif /* _MCAL_ADC_H_ */
