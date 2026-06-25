/*******************************************************************************
|  File Name:  mcal_uart.c
|  Description:  Implementation of MCAL UART driver
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

#include "mcal_uart.h"
#include "mcal_dma.h"
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
typedef struct {
    volatile uint16_t usHead;
    volatile uint16_t usTail;
} xMcal_UartRxState_t;

typedef struct {
    uint32_t ulUsart;
    uint32_t ulBaudRate;
    uint32_t ulDataBits;
    uint32_t ulStopBits;
    uint32_t ulParity;
    uint8_t ucUseDmaTx;
    eMcal_DmaCh_e eDmaTxCh;
    uint32_t ulTxPort;
    uint32_t ulTxPin;
    uint32_t ulRxPort;
    uint32_t ulRxPin;
    uint32_t ulRemap;
    uint8_t* pucRxBuf;
    uint16_t usRxBufSize;
    xMcal_UartRxState_t* pxRxState;
} xMcal_UartCfg_t;

/*******************************************************************************
|    Global variables Declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local Functions Declaration
|******************************************************************************/
static void Mcal_UartConfigGpio( const xMcal_UartCfg_t* pxCfg );
static void Mcal_UartConfigPeriph( const xMcal_UartCfg_t* pxCfg );
static xMcal_UartCfg_t* Mcal_UartFindByPeriph( uint32_t ulUsart );
static void Mcal_UartRxPush( xMcal_UartCfg_t* pxCfg, uint8_t ucData );
static int Mcal_UartRxPop( xMcal_UartCfg_t* pxCfg, uint8_t* pucData );

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local variables
|    Generated from source/mcal/config/mcal_uart_ports.md; do not edit GEN blocks
|******************************************************************************/
/* GEN_MCAL_UART_RXBUF_BEGIN */
static xMcal_UartRxState_t s_xMcalUartRxState_Mcal_Uart0 = { 0U, 0U };
static uint8_t s_ucMcalUartRxBuf_Mcal_Uart0[256];
static xMcal_UartRxState_t s_xMcalUartRxState_Mcal_Uart1 = { 0U, 0U };
static uint8_t s_ucMcalUartRxBuf_Mcal_Uart1[256];
static xMcal_UartRxState_t s_xMcalUartRxState_Mcal_Uart2 = { 0U, 0U };
static uint8_t s_ucMcalUartRxBuf_Mcal_Uart2[256];
/* GEN_MCAL_UART_RXBUF_END */

/* GEN_MCAL_UART_TABLE_BEGIN */
static xMcal_UartCfg_t s_xMcal_UartConfigTable[eMcal_UartMaxNum] = {
    {
        USART0,
        115200U,
        USART_WL_8BIT,
        USART_STB_1BIT,
        USART_PM_NONE,
        1U,
        eMcal_DmaUart0Tx,
        GPIOA,
        GPIO_PIN_9,
        GPIOA,
        GPIO_PIN_10,
        0,
        s_ucMcalUartRxBuf_Mcal_Uart0,
        256U,
        &s_xMcalUartRxState_Mcal_Uart0,
    },
    {
        USART1,
        115200U,
        USART_WL_8BIT,
        USART_STB_1BIT,
        USART_PM_NONE,
        1U,
        eMcal_DmaUart1Tx,
        GPIOA,
        GPIO_PIN_2,
        GPIOA,
        GPIO_PIN_3,
        0,
        s_ucMcalUartRxBuf_Mcal_Uart1,
        256U,
        &s_xMcalUartRxState_Mcal_Uart1,
    },
    {
        USART2,
        115200U,
        USART_WL_8BIT,
        USART_STB_1BIT,
        USART_PM_NONE,
        1U,
        eMcal_DmaUart2Tx,
        GPIOB,
        GPIO_PIN_10,
        GPIOB,
        GPIO_PIN_11,
        0,
        s_ucMcalUartRxBuf_Mcal_Uart2,
        256U,
        &s_xMcalUartRxState_Mcal_Uart2,
    },
};
/* GEN_MCAL_UART_TABLE_END */

/*******************************************************************************
|    Function Source Code
|******************************************************************************/

/*******************************************************************************
Name            : Mcal_UartConfigGpio
Parameters(in)  : pxCfg - UART configuration pointer
Parameters(out) : None
Return value    : None
Description     : Configure USART TX/RX GPIO pins and optional remap
Call By         : Mcal_UartInit
|******************************************************************************/
static void Mcal_UartConfigGpio( const xMcal_UartCfg_t* pxCfg )
{
    if ( pxCfg == NULL ) {
        return;
    }

    if ( pxCfg->ulRemap != 0U ) {
        gpio_pin_remap_config( pxCfg->ulRemap, ENABLE );
    }

    gpio_init( pxCfg->ulTxPort, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pxCfg->ulTxPin );
    gpio_init( pxCfg->ulRxPort, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pxCfg->ulRxPin );
}

/*******************************************************************************
Name            : Mcal_UartConfigPeriph
Parameters(in)  : pxCfg - UART configuration pointer
Parameters(out) : None
Return value    : None
Description     : Configure USART baud rate, format, DMA TX and enable peripheral
Call By         : Mcal_UartInit
|******************************************************************************/
static void Mcal_UartConfigPeriph( const xMcal_UartCfg_t* pxCfg )
{
    if ( pxCfg == NULL ) {
        return;
    }

    usart_deinit( pxCfg->ulUsart );
    usart_baudrate_set( pxCfg->ulUsart, pxCfg->ulBaudRate );
    usart_word_length_set( pxCfg->ulUsart, pxCfg->ulDataBits );
    usart_stop_bit_set( pxCfg->ulUsart, pxCfg->ulStopBits );
    usart_parity_config( pxCfg->ulUsart, pxCfg->ulParity );
    usart_receive_config( pxCfg->ulUsart, USART_RECEIVE_ENABLE );
    usart_transmit_config( pxCfg->ulUsart, USART_TRANSMIT_ENABLE );

    if ( pxCfg->ucUseDmaTx == 1U ) {
        usart_dma_transmit_config( pxCfg->ulUsart, USART_TRANSMIT_DMA_ENABLE );
    }

    usart_enable( pxCfg->ulUsart );
}

/*******************************************************************************
Name            : Mcal_UartFindByPeriph
Parameters(in)  : ulUsart - USART peripheral base address
Parameters(out) : None
Return value    : UART config pointer; NULL if not found
Description     : Find UART configuration entry by USART peripheral
Call By         : Mcal_UartIrqHandler
|******************************************************************************/
static xMcal_UartCfg_t* Mcal_UartFindByPeriph( uint32_t ulUsart )
{
    uint8_t ucIdx;

    for ( ucIdx = 0U; ucIdx < (uint8_t)eMcal_UartMaxNum; ucIdx++ ) {
        if ( s_xMcal_UartConfigTable[ucIdx].ulUsart == ulUsart ) {
            return &s_xMcal_UartConfigTable[ucIdx];
        }
    }

    return NULL;
}

/*******************************************************************************
Name            : Mcal_UartRxPush
Parameters(in)  : pxCfg - UART configuration pointer
                  ucData - received byte
Parameters(out) : None
Return value    : None
Description     : Push one byte into UART receive ring buffer
Call By         : Mcal_UartIrqHandler
|******************************************************************************/
static void Mcal_UartRxPush( xMcal_UartCfg_t* pxCfg, uint8_t ucData )
{
    uint16_t usNext;

    if ( ( pxCfg == NULL ) || ( pxCfg->pxRxState == NULL ) || ( pxCfg->pucRxBuf == NULL ) ) {
        return;
    }

    usNext = (uint16_t)( pxCfg->pxRxState->usHead + 1U );
    if ( usNext >= pxCfg->usRxBufSize ) {
        usNext = 0U;
    }

    if ( usNext == pxCfg->pxRxState->usTail ) {
        return;
    }

    pxCfg->pucRxBuf[pxCfg->pxRxState->usHead] = ucData;
    pxCfg->pxRxState->usHead = usNext;
}

/*******************************************************************************
Name            : Mcal_UartRxPop
Parameters(in)  : pxCfg - UART configuration pointer
Parameters(out) : pucData - received byte output
Return value    : 0 on success, -1 if buffer empty or invalid parameter
Description     : Pop one byte from UART receive ring buffer
Call By         : Mcal_UartReadChar
|******************************************************************************/
static int Mcal_UartRxPop( xMcal_UartCfg_t* pxCfg, uint8_t* pucData )
{
    if ( ( pxCfg == NULL ) || ( pucData == NULL ) || ( pxCfg->pxRxState == NULL ) ) {
        return -1;
    }

    if ( pxCfg->pxRxState->usHead == pxCfg->pxRxState->usTail ) {
        return -1;
    }

    *pucData = pxCfg->pucRxBuf[pxCfg->pxRxState->usTail];
    pxCfg->pxRxState->usTail++;
    if ( pxCfg->pxRxState->usTail >= pxCfg->usRxBufSize ) {
        pxCfg->pxRxState->usTail = 0U;
    }

    return 0;
}

/*******************************************************************************
Name            : Mcal_UartIrqHandler
Parameters(in)  : ulUsart - USART peripheral base address
Parameters(out) : None
Return value    : None
Description     : USART receive interrupt service logic, call from system ISR
Call By         : System interrupt module
|******************************************************************************/
void Mcal_UartIrqHandler( uint32_t ulUsart )
{
    xMcal_UartCfg_t* pxCfg;
    uint8_t ucData;

    pxCfg = Mcal_UartFindByPeriph( ulUsart );
    if ( pxCfg == NULL ) {
        return;
    }

    if ( RESET != usart_interrupt_flag_get( ulUsart, USART_INT_FLAG_RBNE ) ) {
        ucData = (uint8_t)usart_data_receive( ulUsart );
        Mcal_UartRxPush( pxCfg, ucData );
    }
}

/*******************************************************************************
Name            : Mcal_UartInit
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Initialize all enabled UART ports (GPIO and USART peripheral)
Call By         : Application
|******************************************************************************/
void Mcal_UartInit( void )
{
    uint8_t ucIdx;

    for ( ucIdx = 0U; ucIdx < (uint8_t)eMcal_UartMaxNum; ucIdx++ ) {
        if ( s_xMcal_UartConfigTable[ucIdx].pxRxState != NULL ) {
            s_xMcal_UartConfigTable[ucIdx].pxRxState->usHead = 0U;
            s_xMcal_UartConfigTable[ucIdx].pxRxState->usTail = 0U;
        }
        Mcal_UartConfigGpio( &s_xMcal_UartConfigTable[ucIdx] );
        Mcal_UartConfigPeriph( &s_xMcal_UartConfigTable[ucIdx] );
    }
}

/*******************************************************************************
Name            : Mcal_UartSend
Parameters(in)  : eCom - UART port index
                  pucData - data buffer to transmit
                  usLen - data length in bytes
Parameters(out) : None
Return value    : 0 on success, -1 on failure
Description     : Send data via DMA or polling depending on configuration
Call By         : Application
|******************************************************************************/
int Mcal_UartSend( eMcal_UartCom_e eCom, uint8_t* pucData, uint16_t usLen )
{
    xMcal_UartCfg_t* pxCfg;
    uint16_t usIdx;

    if ( ( pucData == NULL ) || ( usLen == 0U ) ) {
        return -1;
    }

    if ( eCom >= eMcal_UartMaxNum ) {
        return -1;
    }

    pxCfg = &s_xMcal_UartConfigTable[eCom];
    if ( pxCfg->ucUseDmaTx == 1U ) {
        return Mcal_DmaUartSend( pxCfg->eDmaTxCh, pucData, usLen );
    }

    for ( usIdx = 0U; usIdx < usLen; usIdx++ ) {
        usart_data_transmit( pxCfg->ulUsart, pucData[usIdx] );
        while ( RESET == usart_flag_get( pxCfg->ulUsart, USART_FLAG_TBE ) ) {
        }
    }

    return 0;
}

/*******************************************************************************
Name            : Mcal_UartReadChar
Parameters(in)  : eCom - UART port index
Parameters(out) : pucData - received byte output
Return value    : 0 on success, -1 if no data or invalid parameter
Description     : Read one byte from UART receive ring buffer
Call By         : Application
|******************************************************************************/
int Mcal_UartReadChar( eMcal_UartCom_e eCom, uint8_t* pucData )
{
    if ( eCom >= eMcal_UartMaxNum ) {
        return -1;
    }

    return Mcal_UartRxPop( &s_xMcal_UartConfigTable[eCom], pucData );
}

/*******************************************************************************
Name            : Mcal_UartGetRxCount
Parameters(in)  : eCom - UART port index
Parameters(out) : None
Return value    : Number of bytes pending in receive buffer
Description     : Get the count of unread bytes in receive ring buffer
Call By         : Application
|******************************************************************************/
uint16_t Mcal_UartGetRxCount( eMcal_UartCom_e eCom )
{
    xMcal_UartCfg_t* pxCfg;
    uint16_t usHead;
    uint16_t usTail;

    if ( eCom >= eMcal_UartMaxNum ) {
        return 0U;
    }

    pxCfg = &s_xMcal_UartConfigTable[eCom];
    if ( pxCfg->pxRxState == NULL ) {
        return 0U;
    }

    usHead = pxCfg->pxRxState->usHead;
    usTail = pxCfg->pxRxState->usTail;
    if ( usHead >= usTail ) {
        return (uint16_t)( usHead - usTail );
    }

    return (uint16_t)( pxCfg->usRxBufSize - usTail + usHead );
}

/* GEN_MCAL_UART_IRQ_BEGIN */
/*******************************************************************************
Name            : USART0_IRQHandler
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : USART0 receive interrupt service routine
Call By         : NVIC
|******************************************************************************/
void USART0_IRQHandler( void )
{
    Mcal_UartIrqHandler( USART0 );
}

/*******************************************************************************
Name            : USART1_IRQHandler
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : USART1 receive interrupt service routine
Call By         : NVIC
|******************************************************************************/
void USART1_IRQHandler( void )
{
    Mcal_UartIrqHandler( USART1 );
}

/*******************************************************************************
Name            : USART2_IRQHandler
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : USART2 receive interrupt service routine
Call By         : NVIC
|******************************************************************************/
void USART2_IRQHandler( void )
{
    Mcal_UartIrqHandler( USART2 );
}
/* GEN_MCAL_UART_IRQ_END */
