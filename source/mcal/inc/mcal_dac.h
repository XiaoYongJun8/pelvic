/*******************************************************************************
|  File Name:  mcal_dac.h
|  Description:  Head file of MCAL DAC driver
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

#ifndef _MCAL_DAC_H_
#define _MCAL_DAC_H_

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
|    Generated from source/mcal/config/mcal_dac_channels.md; edit GEN block only
|    via tools/gen_mcal_dac.py (enum snippet printed, not auto-written here).
|******************************************************************************/
/* GEN_MCAL_DAC_ENUM_BEGIN */
typedef enum {
    eMcal_DacStimCur = 0,
    eMcal_DacStimBoost,
    eMcal_DacMaxNum,
} eMcal_DacCh_e;
/* GEN_MCAL_DAC_ENUM_END */

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef struct {
    uint32_t ulPort;
    uint32_t ulDataAlign;
} xMcal_DacInstCfg_t;

/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
void Mcal_DacInit( void );
void Mcal_DacDeInit( void );
void Mcal_DacSetOutValue( eMcal_DacCh_e eCh, uint16_t usData );
uint16_t Mcal_DacGetOutValue( eMcal_DacCh_e eCh );

#ifdef __cplusplus
}
#endif

#endif /* _MCAL_DAC_H_ */
