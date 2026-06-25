/*******************************************************************************
|  File Name:  mcal_dac.c
|  Description:  Implementation of MCAL DAC driver
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
#include "mcal_dac.h"
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

/*******************************************************************************
|    Global variables Declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local Functions Declaration
|******************************************************************************/
static void Mcal_DacConfigPeriph( void );

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local variables
|    Generated from source/mcal/config/mcal_dac_channels.md; do not edit GEN blocks
|******************************************************************************/
/* GEN_MCAL_DAC_INST_BEGIN */
static const xMcal_DacInstCfg_t s_xMcal_DacInstCfg = {
    DAC0,
    DAC_ALIGN_12B_R,
};
/* GEN_MCAL_DAC_INST_END */

/* GEN_MCAL_DAC_OUT_CHAN_BEGIN */
static const uint8_t s_ucMcalDacOutChan[eMcal_DacMaxNum] = { DAC_OUT0, DAC_OUT1 };
/* GEN_MCAL_DAC_OUT_CHAN_END */

/* GEN_MCAL_DAC_OUT_BUF_BEGIN */
static const uint8_t s_ucMcalDacOutBufEnable[eMcal_DacMaxNum] = { 1U, 1U };
/* GEN_MCAL_DAC_OUT_BUF_END */

/* GEN_MCAL_DAC_INIT_VAL_BEGIN */
static const uint16_t s_usMcalDacInitValue[eMcal_DacMaxNum] = { 0U, 0U };
/* GEN_MCAL_DAC_INIT_VAL_END */

/*******************************************************************************
|    Function Source Code
|******************************************************************************/
/*******************************************************************************
Name            : Mcal_DacConfigPeriph
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Configure DAC instance and all enabled output channels
Call By         : Mcal_DacInit
|******************************************************************************/
static void Mcal_DacConfigPeriph( void )
{
    uint8_t ucIdx;
    const xMcal_DacInstCfg_t* pxInst = &s_xMcal_DacInstCfg;
    uint8_t ucOutChan;

    dac_deinit( pxInst->ulPort );

    for ( ucIdx = 0U; ucIdx < (uint8_t)eMcal_DacMaxNum; ucIdx++ )
    {
        ucOutChan = s_ucMcalDacOutChan[ucIdx];
        dac_trigger_disable( pxInst->ulPort, ucOutChan );
    }

    dac_concurrent_disable( pxInst->ulPort );

    for ( ucIdx = 0U; ucIdx < (uint8_t)eMcal_DacMaxNum; ucIdx++ )
    {
        ucOutChan = s_ucMcalDacOutChan[ucIdx];

        if ( s_ucMcalDacOutBufEnable[ucIdx] != 0U )
        {
            dac_output_buffer_enable( pxInst->ulPort, ucOutChan );
        }
        else
        {
            dac_output_buffer_disable( pxInst->ulPort, ucOutChan );
        }

        dac_enable( pxInst->ulPort, ucOutChan );
        dac_data_set( pxInst->ulPort,
                      ucOutChan,
                      pxInst->ulDataAlign,
                      s_usMcalDacInitValue[ucIdx] );
    }
}

/*******************************************************************************
Name            : Mcal_DacInit
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Initialize configured DAC output channels
Call By         : Application
|******************************************************************************/
void Mcal_DacInit( void )
{
    Mcal_DacConfigPeriph();
}

/*******************************************************************************
Name            : Mcal_DacDeInit
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Disable and deinitialize the DAC peripheral
Call By         : Application
|******************************************************************************/
void Mcal_DacDeInit( void )
{
    uint8_t ucIdx;
    const xMcal_DacInstCfg_t* pxInst = &s_xMcal_DacInstCfg;

    for ( ucIdx = 0U; ucIdx < (uint8_t)eMcal_DacMaxNum; ucIdx++ )
    {
        dac_disable( pxInst->ulPort, s_ucMcalDacOutChan[ucIdx] );
    }

    dac_deinit( pxInst->ulPort );
}

/*******************************************************************************
Name            : Mcal_DacSetOutValue
Parameters(in)  : eCh - logical DAC channel
                  usData - 12-bit output value
Parameters(out) : None
Return value    : None
Description     : Set DAC output value for the selected channel
Call By         : Application
|******************************************************************************/
void Mcal_DacSetOutValue( eMcal_DacCh_e eCh, uint16_t usData )
{
    const xMcal_DacInstCfg_t* pxInst = &s_xMcal_DacInstCfg;

    if ( eCh >= eMcal_DacMaxNum )
    {
        return;
    }

    dac_data_set( pxInst->ulPort,
                  s_ucMcalDacOutChan[eCh],
                  pxInst->ulDataAlign,
                  usData );
}

/*******************************************************************************
Name            : Mcal_DacGetOutValue
Parameters(in)  : eCh - logical DAC channel
Parameters(out) : None
Return value    : Current DAC output value; 0 if eCh is invalid
Description     : Read DAC output value for the selected channel
Call By         : Application
|******************************************************************************/
uint16_t Mcal_DacGetOutValue( eMcal_DacCh_e eCh )
{
    const xMcal_DacInstCfg_t* pxInst = &s_xMcal_DacInstCfg;

    if ( eCh >= eMcal_DacMaxNum )
    {
        return 0U;
    }

    return dac_output_value_get( pxInst->ulPort, s_ucMcalDacOutChan[eCh] );
}
