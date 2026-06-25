/*******************************************************************************
|  File Name:  mcal_adc.c
|  Description:  Implementation of MCAL ADC driver
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
#include "mcal_adc.h"
#include "mcal_dma.h"

/*******************************************************************************
|    Macro Definition
|******************************************************************************/

/*******************************************************************************
|    Enum Definition
|******************************************************************************/

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef void ( *Mcal_AdcCallBack_t )( void );

/*******************************************************************************
|    Global variables Declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local Functions Declaration
|******************************************************************************/
static void Mcal_AdcParaConfig( void );

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local variables
|    Generated from source/mcal/config/mcal_adc_channels.md; do not edit GEN blocks
|******************************************************************************/
/* GEN_MCAL_ADC_INST_BEGIN */
static const xMcal_AdcInstCfg_t s_xMcal_AdcInstCfg = {
    ADC0,
    ADC_MODE_FREE,
    ADC_RESOLUTION_12B,
    ADC_SCAN_MODE,
    ADC_CONTINUOUS_MODE,
    ADC_DATAALIGN_RIGHT,
};
/* GEN_MCAL_ADC_INST_END */

/* GEN_MCAL_ADC_CHAN_BEGIN */
static const uint8_t s_ucMcalAdcChanBuffer[eMcal_AdcMaxNum] = { ADC_CHANNEL_0 };
/* GEN_MCAL_ADC_CHAN_END */

/* GEN_MCAL_ADC_SAMPLE_BEGIN */
static const uint32_t s_ulMcalAdcSampleTime[eMcal_AdcMaxNum] = { ADC_SAMPLETIME_13POINT5 };
/* GEN_MCAL_ADC_SAMPLE_END */

static Mcal_AdcCallBack_t s_pMcalAdcCallBack = NULL;

/*******************************************************************************
|    Function Source Code
|******************************************************************************/

/*******************************************************************************
Name            : Mcal_AdcParaConfig
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Configure ADC instance, channels, DMA and start conversion
Call By         : Mcal_AdcInit
|******************************************************************************/
static void Mcal_AdcParaConfig( void )
{
    uint8_t ucIdx;
    const xMcal_AdcInstCfg_t* pxCfg = &s_xMcal_AdcInstCfg;

    adc_mode_config( pxCfg->ulMode );
    adc_resolution_config( pxCfg->ulAdcPort, pxCfg->ulResolution );
    adc_data_alignment_config( pxCfg->ulAdcPort, pxCfg->ulDataAlign );
    adc_special_function_config( pxCfg->ulAdcPort,
                                 pxCfg->ulScanMode | pxCfg->ulContinuousMode,
                                 ENABLE );
    adc_channel_length_config( pxCfg->ulAdcPort, ADC_REGULAR_CHANNEL, (uint32_t)eMcal_AdcMaxNum );

    for ( ucIdx = 0U; ucIdx < (uint8_t)eMcal_AdcMaxNum; ucIdx++ ) {
        adc_regular_channel_config( pxCfg->ulAdcPort,
                                    ucIdx,
                                    s_ucMcalAdcChanBuffer[ucIdx],
                                    s_ulMcalAdcSampleTime[ucIdx] );
    }

    adc_external_trigger_source_config( pxCfg->ulAdcPort,
                                        ADC_REGULAR_CHANNEL,
                                        ADC0_1_2_EXTTRIG_REGULAR_NONE );
    adc_external_trigger_config( pxCfg->ulAdcPort, ADC_REGULAR_CHANNEL, ENABLE );
    adc_enable( pxCfg->ulAdcPort );
    adc_calibration_enable( pxCfg->ulAdcPort );
    adc_dma_mode_enable( pxCfg->ulAdcPort );
    adc_software_trigger_enable( pxCfg->ulAdcPort, ADC_REGULAR_CHANNEL );
}

/*******************************************************************************
Name            : Mcal_AdcInit
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Initialize ADC scan mode with DMA result buffer
Call By         : Application
|******************************************************************************/
void Mcal_AdcInit( void )
{
    Mcal_AdcParaConfig();
}

/*******************************************************************************
Name            : Mcal_AdcDeInit
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Disable and deinitialize the ADC peripheral
Call By         : Application
|******************************************************************************/
void Mcal_AdcDeInit( void )
{
    adc_disable( s_xMcal_AdcInstCfg.ulAdcPort );
    adc_deinit( s_xMcal_AdcInstCfg.ulAdcPort );
}

/*******************************************************************************
Name            : Mcal_GetAdcChValue
Parameters(in)  : eCh - logical ADC channel index
Parameters(out) : None
Return value    : Latest sampled value; 0 if eCh is invalid
Description     : Read the DMA buffer value of the specified ADC channel
Call By         : Application
|******************************************************************************/
uint16_t Mcal_GetAdcChValue( eMcal_AdcCh_e eCh )
{
    uint16_t* pusBuffer;

    if ( eCh >= eMcal_AdcMaxNum ) {
        return 0U;
    }

    pusBuffer = Mcal_GetDmaMemory16( eMcal_DmaAdc0 );
    if ( pusBuffer == NULL ) {
        return 0U;
    }

    return pusBuffer[eCh];
}

/*******************************************************************************
Name            : Mcal_AdcSetCallFun
Parameters(in)  : pCallBack - ADC completion callback, NULL to clear
Parameters(out) : None
Return value    : None
Description     : Register optional ADC callback function
Call By         : Application
|******************************************************************************/
void Mcal_AdcSetCallFun( void ( *pCallBack )( void ) )
{
    s_pMcalAdcCallBack = pCallBack;
}
