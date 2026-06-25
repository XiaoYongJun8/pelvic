/*******************************************************************************
|  File Name:  mcal_dma.c
|  Description:  Implementation of MCAL DMA driver
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
#include <string.h>

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

/*******************************************************************************
|    Global variables Declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local Functions Declaration
|******************************************************************************/
static void Mcal_DmaConfigChannel( xMcal_DmaChCfg_t* pxCfg );

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local variables
|    Generated from source/mcal/config/mcal_dma_channels.md; do not edit GEN blocks
|******************************************************************************/
/* GEN_MCAL_DMA_BUFFERS_BEGIN */
static uint16_t s_aMcalDmaBuf_Mcal_DmaAdc0[1] = { 0 };
static uint8_t s_aMcalDmaBuf_Mcal_DmaUart0Tx[156] = { 0 };
static uint8_t s_aMcalDmaBuf_Mcal_DmaUart1Tx[156] = { 0 };
static uint8_t s_aMcalDmaBuf_Mcal_DmaUart2Tx[156] = { 0 };
/* GEN_MCAL_DMA_BUFFERS_END */

/* GEN_MCAL_DMA_TABLE_BEGIN */
static xMcal_DmaChCfg_t s_xMcal_DmaConfigTable[eMcal_DmaMaxNum] = {
    {
        1U,
        DMA0,
        DMA_CH0,
        0x4001244CU,
        DMA_PERIPHERAL_WIDTH_16BIT,
        DMA_MEMORY_WIDTH_16BIT,
        1U,
        DMA_PRIORITY_HIGH,
        DMA_PERIPH_INCREASE_DISABLE,
        DMA_MEMORY_INCREASE_ENABLE,
        DMA_PERIPHERAL_TO_MEMORY,
        s_aMcalDmaBuf_Mcal_DmaAdc0,
        sizeof( s_aMcalDmaBuf_Mcal_DmaAdc0 ),
        0,
    },
    {
        0U,
        DMA0,
        DMA_CH3,
        0x40013804U,
        DMA_PERIPHERAL_WIDTH_8BIT,
        DMA_MEMORY_WIDTH_8BIT,
        156U,
        DMA_PRIORITY_HIGH,
        DMA_PERIPH_INCREASE_DISABLE,
        DMA_MEMORY_INCREASE_ENABLE,
        DMA_MEMORY_TO_PERIPHERAL,
        s_aMcalDmaBuf_Mcal_DmaUart0Tx,
        sizeof( s_aMcalDmaBuf_Mcal_DmaUart0Tx ),
        USART0,
    },
    {
        0U,
        DMA0,
        DMA_CH5,
        0x40004404U,
        DMA_PERIPHERAL_WIDTH_8BIT,
        DMA_MEMORY_WIDTH_8BIT,
        156U,
        DMA_PRIORITY_HIGH,
        DMA_PERIPH_INCREASE_DISABLE,
        DMA_MEMORY_INCREASE_ENABLE,
        DMA_MEMORY_TO_PERIPHERAL,
        s_aMcalDmaBuf_Mcal_DmaUart1Tx,
        sizeof( s_aMcalDmaBuf_Mcal_DmaUart1Tx ),
        USART1,
    },
    {
        0U,
        DMA0,
        DMA_CH1,
        0x40004804U,
        DMA_PERIPHERAL_WIDTH_8BIT,
        DMA_MEMORY_WIDTH_8BIT,
        156U,
        DMA_PRIORITY_HIGH,
        DMA_PERIPH_INCREASE_DISABLE,
        DMA_MEMORY_INCREASE_ENABLE,
        DMA_MEMORY_TO_PERIPHERAL,
        s_aMcalDmaBuf_Mcal_DmaUart2Tx,
        sizeof( s_aMcalDmaBuf_Mcal_DmaUart2Tx ),
        USART2,
    },
};
/* GEN_MCAL_DMA_TABLE_END */

/*******************************************************************************
|    Function Source Code
|******************************************************************************/

/*******************************************************************************
Name            : Mcal_DmaConfigChannel
Parameters(in)  : pxCfg - DMA channel configuration pointer
Parameters(out) : None
Return value    : None
Description     : Deinit, init and enable one DMA channel per configuration
Call By         : Mcal_DmaInit
|******************************************************************************/
static void Mcal_DmaConfigChannel( xMcal_DmaChCfg_t* pxCfg )
{
    dma_parameter_struct xDmaPara;

    if ( pxCfg == NULL ) {
        return;
    }

    dma_deinit( pxCfg->ulDmaPort, pxCfg->xChan );

    xDmaPara.periph_addr = pxCfg->ulPeriphAddr;
    xDmaPara.periph_width = pxCfg->ulPeriphWidth;
    xDmaPara.memory_addr = (uint32_t)pxCfg->pvMemBuffer;
    xDmaPara.memory_width = pxCfg->ulMemoryWidth;
    xDmaPara.number = pxCfg->usTransferNum;
    xDmaPara.priority = pxCfg->ulPriority;
    xDmaPara.periph_inc = pxCfg->ulPeriphInc;
    xDmaPara.memory_inc = pxCfg->ulMemoryInc;
    xDmaPara.direction = pxCfg->ulDirection;

    dma_init( pxCfg->ulDmaPort, pxCfg->xChan, &xDmaPara );

    if ( pxCfg->ucCircMode == 1U ) {
        dma_circulation_enable( pxCfg->ulDmaPort, pxCfg->xChan );
    } else {
        dma_circulation_disable( pxCfg->ulDmaPort, pxCfg->xChan );
    }

    dma_channel_enable( pxCfg->ulDmaPort, pxCfg->xChan );
}

/*******************************************************************************
Name            : Mcal_DmaInit
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Enable DMA clock, init all channels and USART TX DMA requests
Call By         : Application
|******************************************************************************/
void Mcal_DmaInit( void )
{
    uint8_t ucIdx;

    rcu_periph_clock_enable( RCU_DMA0 );

    for ( ucIdx = 0U; ucIdx < (uint8_t)eMcal_DmaMaxNum; ucIdx++ ) {
        Mcal_DmaConfigChannel( &s_xMcal_DmaConfigTable[ucIdx] );

        if ( s_xMcal_DmaConfigTable[ucIdx].ulUsartPeriph != 0U ) {
            usart_dma_transmit_config( s_xMcal_DmaConfigTable[ucIdx].ulUsartPeriph,
                                       USART_TRANSMIT_DMA_ENABLE );
        }
    }
}

/*******************************************************************************
Name            : Mcal_GetDmaBuffer
Parameters(in)  : eCh - DMA channel index
Parameters(out) : None
Return value    : Pointer to channel memory buffer; NULL if eCh is invalid
Description     : Get the memory buffer address of the specified DMA channel
Call By         : Application
|******************************************************************************/
void* Mcal_GetDmaBuffer( eMcal_DmaCh_e eCh )
{
    if ( eCh >= eMcal_DmaMaxNum ) {
        return NULL;
    }

    return s_xMcal_DmaConfigTable[eCh].pvMemBuffer;
}

/*******************************************************************************
Name            : Mcal_GetDmaMemory16
Parameters(in)  : eCh - DMA channel index
Parameters(out) : None
Return value    : Pointer to 16-bit DMA buffer; NULL if eCh is invalid
Description     : Get the 16-bit memory buffer of the specified DMA channel
Call By         : Application
|******************************************************************************/
uint16_t* Mcal_GetDmaMemory16( eMcal_DmaCh_e eCh )
{
    return (uint16_t*)Mcal_GetDmaBuffer( eCh );
}

/*******************************************************************************
Name            : Mcal_GetDmaMemory8
Parameters(in)  : eCh - DMA channel index
Parameters(out) : None
Return value    : Pointer to 8-bit DMA buffer; NULL if eCh is invalid
Description     : Get the 8-bit memory buffer of the specified DMA channel
Call By         : Application
|******************************************************************************/
uint8_t* Mcal_GetDmaMemory8( eMcal_DmaCh_e eCh )
{
    return (uint8_t*)Mcal_GetDmaBuffer( eCh );
}

/*******************************************************************************
Name            : Mcal_DmaUartSend
Parameters(in)  : eCh - DMA channel index
                  pucData - data buffer to transmit
                  usLen - data length in bytes
Parameters(out) : None
Return value    : 0 on success, -1 on invalid parameter or channel type
Description     : Blocking DMA transmit on memory-to-peripheral UART TX channel
Call By         : Mcal_DmaUart0Send, Mcal_DmaUart1Send, Mcal_DmaUart2Send
|******************************************************************************/
int Mcal_DmaUartSend( eMcal_DmaCh_e eCh, uint8_t* pucData, uint16_t usLen )
{
    xMcal_DmaChCfg_t* pxCfg;

    if ( ( pucData == NULL ) || ( usLen == 0U ) ) {
        return -1;
    }

    if ( eCh >= eMcal_DmaMaxNum ) {
        return -1;
    }

    pxCfg = &s_xMcal_DmaConfigTable[eCh];
    if ( pxCfg->ulDirection != DMA_MEMORY_TO_PERIPHERAL ) {
        return -1;
    }

    if ( usLen > pxCfg->usTransferNum ) {
        usLen = pxCfg->usTransferNum;
    }

    dma_flag_clear( pxCfg->ulDmaPort, pxCfg->xChan, DMA_FLAG_FTF );
    dma_channel_disable( pxCfg->ulDmaPort, pxCfg->xChan );
    dma_transfer_number_config( pxCfg->ulDmaPort, pxCfg->xChan, usLen );
    memcpy( pxCfg->pvMemBuffer, pucData, usLen );
    dma_channel_enable( pxCfg->ulDmaPort, pxCfg->xChan );

    while ( RESET == dma_flag_get( pxCfg->ulDmaPort, pxCfg->xChan, DMA_FLAG_FTF ) ) {
    }

    return 0;
}

/*******************************************************************************
Name            : Mcal_DmaUart0Send
Parameters(in)  : pucData - data buffer to transmit
                  usLen - data length in bytes
Parameters(out) : None
Return value    : 0 on success, -1 on failure
Description     : Blocking DMA transmit on USART0 TX channel
Call By         : Application
|******************************************************************************/
int Mcal_DmaUart0Send( uint8_t* pucData, uint16_t usLen )
{
    return Mcal_DmaUartSend( eMcal_DmaUart0Tx, pucData, usLen );
}

/*******************************************************************************
Name            : Mcal_DmaUart1Send
Parameters(in)  : pucData - data buffer to transmit
                  usLen - data length in bytes
Parameters(out) : None
Return value    : 0 on success, -1 on failure
Description     : Blocking DMA transmit on USART1 TX channel
Call By         : Application
|******************************************************************************/
int Mcal_DmaUart1Send( uint8_t* pucData, uint16_t usLen )
{
    return Mcal_DmaUartSend( eMcal_DmaUart1Tx, pucData, usLen );
}

/*******************************************************************************
Name            : Mcal_DmaUart2Send
Parameters(in)  : pucData - data buffer to transmit
                  usLen - data length in bytes
Parameters(out) : None
Return value    : 0 on success, -1 on failure
Description     : Blocking DMA transmit on USART2 TX channel
Call By         : Application
|******************************************************************************/
int Mcal_DmaUart2Send( uint8_t* pucData, uint16_t usLen )
{
    return Mcal_DmaUartSend( eMcal_DmaUart2Tx, pucData, usLen );
}

/*******************************************************************************
Name            : Mcal_GetDmaCfgTable
Parameters(in)  : eCh - DMA channel index
Parameters(out) : None
Return value    : Pointer to channel config; NULL if eCh is invalid
Description     : Get the configuration entry for the specified DMA channel
Call By         : Application
|******************************************************************************/
xMcal_DmaChCfg_t* Mcal_GetDmaCfgTable( eMcal_DmaCh_e eCh )
{
    if ( eCh >= eMcal_DmaMaxNum ) {
        return NULL;
    }

    return &s_xMcal_DmaConfigTable[eCh];
}
