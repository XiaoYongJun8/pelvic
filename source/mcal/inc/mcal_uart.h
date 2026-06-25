/*******************************************************************************
|  File Name:  mcal_uart.h
|  Description:  Head file of MCAL UART driver
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

#ifndef _MCAL_UART_H_
#define _MCAL_UART_H_

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
|    Generated from source/mcal/config/mcal_uart_ports.md; edit GEN block only
|    via tools/gen_mcal_uart.py (enum snippet printed, not auto-written here).
|******************************************************************************/
/* GEN_MCAL_UART_ENUM_BEGIN */
typedef enum {
    eMcal_Uart0 = 0,
    eMcal_Uart1,
    eMcal_Uart2,
    eMcal_UartMaxNum,
} eMcal_UartCom_e;
/* GEN_MCAL_UART_ENUM_END */

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/

/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
void Mcal_UartInit( void );
int Mcal_UartSend( eMcal_UartCom_e eCom, uint8_t* pucData, uint16_t usLen );
void Mcal_UartIrqHandler( uint32_t ulUsart );
int Mcal_UartReadChar( eMcal_UartCom_e eCom, uint8_t* pucData );
uint16_t Mcal_UartGetRxCount( eMcal_UartCom_e eCom );

#ifdef __cplusplus
}
#endif

#endif /* _MCAL_UART_H_ */
