/*******************************************************************************
|  File Name:  mcal_flash.h
|  Description:  Head file of MCAL flash driver
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

#ifndef _MCAL_FLASH_H_
#define _MCAL_FLASH_H_

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
|    Macro Definition
|    Generated from source/mcal/config/mcal_flash_config.md; edit GEN block only
|    via tools/gen_mcal_flash.py (macro snippet printed, not auto-written here).
|******************************************************************************/
/* GEN_MCAL_FLASH_MACRO_BEGIN */
#define MCAL_FLASH_START_ADDR    ( 0x08000000U )
#define MCAL_FLASH_PAGE_SIZE     ( 0x800U )
#define MCAL_FLASH_USER_PAGES    ( 7U )
#define MCAL_FLASH_USER_END_ADDR ( MCAL_FLASH_START_ADDR + MCAL_FLASH_PAGE_SIZE * MCAL_FLASH_USER_PAGES - 1U )
/* GEN_MCAL_FLASH_MACRO_END */

/*******************************************************************************
|    Enum Definition
|******************************************************************************/

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef struct {
    uint32_t ulBaseAddr;
    uint32_t ulPageSize;
    uint32_t ulUserPages;
    uint8_t ucFeedDogOnLongOp;
} xMcal_FlashCfg_t;

/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
void Mcal_FlashInit( void );
int Mcal_FlashErase( uint32_t ulStartAddr, uint32_t ulStopAddr );
int Mcal_FlashWrite( uint32_t ulWrAddr, uint8_t* pucData, uint32_t ulLen );
void Mcal_FlashRead( uint32_t ulRdAddr, uint8_t* pucData, uint32_t ulLen );

#ifdef __cplusplus
}
#endif

#endif /* _MCAL_FLASH_H_ */
