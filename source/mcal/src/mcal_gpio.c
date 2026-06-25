/*******************************************************************************
|  File Name:  mcal_gpio.c
|  Description:  Implementation of MCAL GPIO driver
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
#include "mcal_gpio.h"

/*******************************************************************************
|    Macro Definition
|******************************************************************************/

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

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local variables
|    由 source/mcal/config/mcal_gpio_ports.md 生成，勿手动修改 GEN 区块
|******************************************************************************/
/* GEN_MCAL_GPIO_TABLE_BEGIN */
static xMcal_GpioPinCfg_t s_xMcal_GpioConfigTable[eMcal_GpioMaxNum] = {
    { GPIOA, GPIO_PIN_0, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, 0, 0 },
    { GPIOB, GPIO_PIN_2, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, 0, 0 },
};
/* GEN_MCAL_GPIO_TABLE_END */

/*******************************************************************************
|    Function Source Code
|******************************************************************************/

/*******************************************************************************
Name            : Mcal_GpioConfigPin
Parameters(in)  : pxCfg - GPIO pin configuration pointer
Parameters(out) : None
Return value    : None
Description     : Configure a single GPIO pin (remap, mode, speed, default level)
Call By         : Mcal_GpioInit
|******************************************************************************/
void Mcal_GpioConfigPin( xMcal_GpioPinCfg_t* pxCfg )
{
    if ( pxCfg == NULL ) {
        return;
    }

    if ( pxCfg->ulRemap != 0U ) {
        gpio_pin_remap_config( pxCfg->ulRemap, ENABLE );
    }

    gpio_init( pxCfg->ulPort, pxCfg->ucMode, pxCfg->ucSpeed, pxCfg->ulPin );

    if ( ( pxCfg->ucMode == GPIO_MODE_OUT_PP ) || ( pxCfg->ucMode == GPIO_MODE_OUT_OD ) ) {
        if ( pxCfg->ucDefaultLevel == 1U ) {
            gpio_bit_set( pxCfg->ulPort, pxCfg->ulPin );
        } else {
            gpio_bit_reset( pxCfg->ulPort, pxCfg->ulPin );
        }
    }
}

/*******************************************************************************
Name            : Mcal_GpioInit
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Initialize all GPIO pins from the config table
Call By         : Application
|******************************************************************************/
void Mcal_GpioInit( void )
{
    uint8_t ucIdx;

    for ( ucIdx = 0U; ucIdx < (uint8_t)eMcal_GpioMaxNum; ucIdx++ ) {
        Mcal_GpioConfigPin( &s_xMcal_GpioConfigTable[ucIdx] );
    }
}

/*******************************************************************************
Name            : Mcal_SetGpioPin
Parameters(in)  : ePin - GPIO pin index
Parameters(out) : None
Return value    : None
Description     : Set the specified GPIO output pin to high level
Call By         : Application
|******************************************************************************/
void Mcal_SetGpioPin( eMcal_GpioPin_e ePin )
{
    if ( ePin >= eMcal_GpioMaxNum ) {
        return;
    }

    gpio_bit_set( s_xMcal_GpioConfigTable[ePin].ulPort, s_xMcal_GpioConfigTable[ePin].ulPin );
}

/*******************************************************************************
Name            : Mcal_ResetGpioPin
Parameters(in)  : ePin - GPIO pin index
Parameters(out) : None
Return value    : None
Description     : Set the specified GPIO output pin to low level
Call By         : Application
|******************************************************************************/
void Mcal_ResetGpioPin( eMcal_GpioPin_e ePin )
{
    if ( ePin >= eMcal_GpioMaxNum ) {
        return;
    }

    gpio_bit_reset( s_xMcal_GpioConfigTable[ePin].ulPort, s_xMcal_GpioConfigTable[ePin].ulPin );
}

/*******************************************************************************
Name            : Mcal_GetGpioPinStatus
Parameters(in)  : ePin - GPIO pin index
Parameters(out) : None
Return value    : Pin level (0=low, 1=high); 0 if ePin is invalid
Description     : Read the input/output level of the specified GPIO pin
Call By         : Application
|******************************************************************************/
uint8_t Mcal_GetGpioPinStatus( eMcal_GpioPin_e ePin )
{
    if ( ePin >= eMcal_GpioMaxNum ) {
        return 0U;
    }

    return (uint8_t)gpio_input_bit_get( s_xMcal_GpioConfigTable[ePin].ulPort,
                                        s_xMcal_GpioConfigTable[ePin].ulPin );
}

/*******************************************************************************
Name            : Mcal_GetGpioCfgTable
Parameters(in)  : ePin - GPIO pin index
Parameters(out) : None
Return value    : Pointer to pin config; NULL if ePin is invalid
Description     : Get the configuration entry for the specified GPIO pin
Call By         : Application
|******************************************************************************/
xMcal_GpioPinCfg_t* Mcal_GetGpioCfgTable( eMcal_GpioPin_e ePin )
{
    if ( ePin >= eMcal_GpioMaxNum ) {
        return NULL;
    }

    return &s_xMcal_GpioConfigTable[ePin];
}
