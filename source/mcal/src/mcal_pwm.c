/*******************************************************************************
|  File Name:  mcal_pwm.c
|  Description:  Implementation of MCAL PWM driver
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
#include "mcal_pwm.h"
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
static void Mcal_PwmConfigPeriph( xMcal_PwmCfg_t* pxCfg );

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local variables
|    Generated from source/mcal/config/mcal_pwm_channels.md; do not edit GEN blocks
|******************************************************************************/
/* GEN_MCAL_PWM_CLK_BEGIN */
static const uint32_t s_ulMcalPwmTimerClkHz = 96000000U;
/* GEN_MCAL_PWM_CLK_END */

/* GEN_MCAL_PWM_TABLE_BEGIN */
static xMcal_PwmCfg_t s_xMcal_PwmConfigTable[eMcal_PwmMaxNum] = {
    {
        TIMER3,
        TIMER_CH_3,
        8U,
        9999U,
        TIMER_COUNTER_EDGE,
        TIMER_COUNTER_UP,
        TIMER_CKDIV_DIV1,
        0U,
        1U,
    },
};
/* GEN_MCAL_PWM_TABLE_END */

/* GEN_MCAL_PWM_DEFAULT_BEGIN */
static const uint16_t s_usMcalPwmDefaultFreq[eMcal_PwmMaxNum] = { 1000U };
static const uint32_t s_ulMcalPwmDefaultPwUs[eMcal_PwmMaxNum] = { 0U };
/* GEN_MCAL_PWM_DEFAULT_END */

static uint16_t s_usMcalPwmSetFreq[eMcal_PwmMaxNum];

/*******************************************************************************
|    Function Source Code
|******************************************************************************/
/*******************************************************************************
Name            : Mcal_PwmConfigPeriph
Parameters(in)  : pxCfg - PWM channel configuration entry
Parameters(out) : None
Return value    : None
Description     : Configure timer PWM output for one channel
Call By         : Mcal_PwmInit, Mcal_PwmSetFreqDuty
|******************************************************************************/
static void Mcal_PwmConfigPeriph( xMcal_PwmCfg_t* pxCfg )
{
    timer_parameter_struct xTimerInit;
    timer_oc_parameter_struct xOcInit;

    xTimerInit.prescaler = pxCfg->usPrescaler;
    xTimerInit.period = pxCfg->usPeriod;
    xTimerInit.alignedmode = pxCfg->ulAlignedMode;
    xTimerInit.counterdirection = pxCfg->ulCounterDirection;
    xTimerInit.clockdivision = pxCfg->ulClockDivision;
    xTimerInit.repetitioncounter = pxCfg->usRepetitionCounter;

    xOcInit.outputstate = TIMER_CCX_ENABLE;
    xOcInit.outputnstate = TIMER_CCXN_DISABLE;
    xOcInit.ocpolarity = TIMER_OC_POLARITY_HIGH;
    xOcInit.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    xOcInit.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    xOcInit.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_deinit( pxCfg->ulTimer );
    timer_init( pxCfg->ulTimer, &xTimerInit );
    timer_channel_output_config( pxCfg->ulTimer, pxCfg->usChannel, &xOcInit );
    timer_channel_output_pulse_value_config( pxCfg->ulTimer, pxCfg->usChannel, 0U );
    timer_channel_output_mode_config( pxCfg->ulTimer, pxCfg->usChannel, TIMER_OC_MODE_PWM0 );
    timer_channel_output_shadow_config( pxCfg->ulTimer, pxCfg->usChannel, TIMER_OC_SHADOW_DISABLE );
    timer_auto_reload_shadow_disable( pxCfg->ulTimer );

    if ( pxCfg->ucAutoStart != 0U )
    {
        timer_enable( pxCfg->ulTimer );
    }
}

/*******************************************************************************
Name            : Mcal_PwmInit
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Initialize all configured PWM channels
Call By         : Application
|******************************************************************************/
void Mcal_PwmInit( void )
{
    uint8_t ucIdx;

    for ( ucIdx = 0U; ucIdx < (uint8_t)eMcal_PwmMaxNum; ucIdx++ )
    {
        s_usMcalPwmSetFreq[ucIdx] = s_usMcalPwmDefaultFreq[ucIdx];
        Mcal_PwmSetFreqDuty( (eMcal_Pwm_e)ucIdx,
                            s_usMcalPwmDefaultFreq[ucIdx],
                            s_ulMcalPwmDefaultPwUs[ucIdx] );
    }
}

/*******************************************************************************
Name            : Mcal_PwmStart
Parameters(in)  : ePwm - logical PWM channel
Parameters(out) : None
Return value    : None
Description     : Enable PWM timer output
Call By         : Application
|******************************************************************************/
void Mcal_PwmStart( eMcal_Pwm_e ePwm )
{
    if ( ePwm >= eMcal_PwmMaxNum )
    {
        return;
    }

    timer_enable( s_xMcal_PwmConfigTable[ePwm].ulTimer );
}

/*******************************************************************************
Name            : Mcal_PwmStop
Parameters(in)  : ePwm - logical PWM channel
Parameters(out) : None
Return value    : None
Description     : Disable PWM timer output
Call By         : Application
|******************************************************************************/
void Mcal_PwmStop( eMcal_Pwm_e ePwm )
{
    if ( ePwm >= eMcal_PwmMaxNum )
    {
        return;
    }

    timer_disable( s_xMcal_PwmConfigTable[ePwm].ulTimer );
}

/*******************************************************************************
Name            : Mcal_PwmSetDuty
Parameters(in)  : ePwm - logical PWM channel
                  ulDuty - compare value (pulse width count)
Parameters(out) : None
Return value    : None
Description     : Set PWM compare value directly
Call By         : Application
|******************************************************************************/
void Mcal_PwmSetDuty( eMcal_Pwm_e ePwm, uint32_t ulDuty )
{
    xMcal_PwmCfg_t* pxCfg;

    if ( ePwm >= eMcal_PwmMaxNum )
    {
        return;
    }

    pxCfg = &s_xMcal_PwmConfigTable[ePwm];
    timer_channel_output_pulse_value_config( pxCfg->ulTimer, pxCfg->usChannel, ulDuty );
}

/*******************************************************************************
Name            : Mcal_PwmSetFreqDuty
Parameters(in)  : ePwm - logical PWM channel
                  usFreq - PWM frequency in Hz
                  ulPwUs - pulse width in microseconds
Parameters(out) : None
Return value    : 1 on success
Description     : Reconfigure prescaler/period and set pulse width
Call By         : Application
|******************************************************************************/
uint8_t Mcal_PwmSetFreqDuty( eMcal_Pwm_e ePwm, uint16_t usFreq, uint32_t ulPwUs )
{
    xMcal_PwmCfg_t* pxCfg;
    float fBaseFreq;
    uint32_t ulTopVal;
    float fTopUs;
    uint32_t ulDuty;

    if ( ( ePwm >= eMcal_PwmMaxNum ) || ( usFreq == 0U ) )
    {
        return 0U;
    }

    pxCfg = &s_xMcal_PwmConfigTable[ePwm];

    if ( usFreq < 100U )
    {
        pxCfg->usPrescaler = 9600U - 1U;
    }
    else if ( usFreq < 1000U )
    {
        pxCfg->usPrescaler = 960U - 1U;
    }
    else
    {
        pxCfg->usPrescaler = 96U - 1U;
    }

    s_usMcalPwmSetFreq[ePwm] = usFreq;
    fBaseFreq = (float)s_ulMcalPwmTimerClkHz / (float)( pxCfg->usPrescaler + 1U );
    ulTopVal = (uint32_t)( fBaseFreq / (float)usFreq );
    pxCfg->usPeriod = (uint16_t)( ulTopVal - 1U );

    Mcal_PwmConfigPeriph( pxCfg );

    fTopUs = ( (float)ulTopVal / fBaseFreq ) * 1000000.0f;
    ulDuty = (uint32_t)( ( (float)ulPwUs / fTopUs ) * (float)ulTopVal );
    Mcal_PwmSetDuty( ePwm, ulDuty );

    return 1U;
}

/*******************************************************************************
Name            : Mcal_PwmSetPw
Parameters(in)  : ePwm - logical PWM channel
                  ulPwUs - pulse width in microseconds
Parameters(out) : None
Return value    : 1 on success
Description     : Set pulse width using the last configured frequency
Call By         : Application
|******************************************************************************/
uint8_t Mcal_PwmSetPw( eMcal_Pwm_e ePwm, uint32_t ulPwUs )
{
    xMcal_PwmCfg_t* pxCfg;
    float fBaseFreq;
    uint32_t ulTopVal;
    float fTopUs;
    uint32_t ulDuty;

    if ( ePwm >= eMcal_PwmMaxNum )
    {
        return 0U;
    }

    pxCfg = &s_xMcal_PwmConfigTable[ePwm];
    fBaseFreq = (float)s_ulMcalPwmTimerClkHz / (float)( pxCfg->usPrescaler + 1U );
    ulTopVal = (uint32_t)( fBaseFreq / (float)s_usMcalPwmSetFreq[ePwm] );
    fTopUs = ( (float)ulTopVal / fBaseFreq ) * 1000000.0f;
    ulDuty = (uint32_t)( ( (float)ulPwUs / fTopUs ) * (float)ulTopVal );
    Mcal_PwmSetDuty( ePwm, ulDuty );

    return 1U;
}
