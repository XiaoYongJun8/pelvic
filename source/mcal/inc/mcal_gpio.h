/*******************************************************************************
|  File Name:  mcal_gpio.h
|  Description:  Head file of MCAL GPIO driver
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

#ifndef _MCAL_GPIO_H_
#define _MCAL_GPIO_H_

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include <stdint.h>
#include <stddef.h>

#include "gd32f30x_libopt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
|    Macro Definition
|******************************************************************************/

/*******************************************************************************
|    Enum Definition
|    Generated from source/mcal/config/mcal_gpio_ports.md; edit GEN block only
|    via tools/gen_mcal_gpio.py (enum snippet printed, not auto-written here).
|******************************************************************************/
/* GEN_MCAL_GPIO_ENUM_BEGIN */
typedef enum {
    eMcal_GpioLedOp = 0,
    eMcal_GpioPb2In,
    eMcal_GpioMaxNum,
} eMcal_GpioPin_e;
/* GEN_MCAL_GPIO_ENUM_END */

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef struct {
    uint32_t ulPort;
    uint32_t ulPin;
    uint8_t ucMode;
    uint8_t ucSpeed;
    uint8_t ucDefaultLevel;
    uint32_t ulRemap;
} xMcal_GpioPinCfg_t;

/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
void Mcal_GpioInit( void );
void Mcal_SetGpioPin( eMcal_GpioPin_e ePin );
void Mcal_ResetGpioPin( eMcal_GpioPin_e ePin );
uint8_t Mcal_GetGpioPinStatus( eMcal_GpioPin_e ePin );
void Mcal_GpioConfigPin( xMcal_GpioPinCfg_t* pxCfg );
xMcal_GpioPinCfg_t* Mcal_GetGpioCfgTable( eMcal_GpioPin_e ePin );

#ifdef __cplusplus
}
#endif

#endif /* _MCAL_GPIO_H_ */
