/*******************************************************************************
|  File Name:  mcal_dma.h
|  Description:  Head file of MCAL DMA driver
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

#ifndef _MCAL_DMA_H_
#define _MCAL_DMA_H_

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
|    Generated from source/mcal/config/mcal_dma_channels.md; edit GEN block only
|    via tools/gen_mcal_dma.py (enum snippet printed, not auto-written here).
|******************************************************************************/
/* GEN_MCAL_DMA_ENUM_BEGIN */
typedef enum {
    eMcal_DmaAdc0 = 0,
    eMcal_DmaUart0Tx,
    eMcal_DmaUart1Tx,
    eMcal_DmaUart2Tx,
    eMcal_DmaMaxNum,
} eMcal_DmaCh_e;
/* GEN_MCAL_DMA_ENUM_END */

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef struct {
    uint8_t ucCircMode;
    uint32_t ulDmaPort;
    dma_channel_enum xChan;
    uint32_t ulPeriphAddr;
    uint32_t ulPeriphWidth;
    uint32_t ulMemoryWidth;
    uint16_t usTransferNum;
    uint32_t ulPriority;
    uint32_t ulPeriphInc;
    uint32_t ulMemoryInc;
    uint32_t ulDirection;
    void* pvMemBuffer;
    uint32_t ulMemBufferBytes;
    uint32_t ulUsartPeriph;
} xMcal_DmaChCfg_t;

/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
void Mcal_DmaInit( void );
void* Mcal_GetDmaBuffer( eMcal_DmaCh_e eCh );
uint16_t* Mcal_GetDmaMemory16( eMcal_DmaCh_e eCh );
uint8_t* Mcal_GetDmaMemory8( eMcal_DmaCh_e eCh );
int Mcal_DmaUartSend( eMcal_DmaCh_e eCh, uint8_t* pucData, uint16_t usLen );
int Mcal_DmaUart0Send( uint8_t* pucData, uint16_t usLen );
int Mcal_DmaUart1Send( uint8_t* pucData, uint16_t usLen );
int Mcal_DmaUart2Send( uint8_t* pucData, uint16_t usLen );
xMcal_DmaChCfg_t* Mcal_GetDmaCfgTable( eMcal_DmaCh_e eCh );

#ifdef __cplusplus
}
#endif

#endif /* _MCAL_DMA_H_ */
