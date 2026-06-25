/*******************************************************************************
|  File Name:  mcal_flash.c
|  Description:  Implementation of MCAL flash driver
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
#include <stddef.h>
#include <string.h>

#include "mcal_flash.h"
#include "mcal_dog.h"
#include "gd32f30x_libopt.h"

/*******************************************************************************
|    Macro Definition
|******************************************************************************/
#define MCAL_FLASH_OK    1
#define MCAL_FLASH_FAIL  0

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
static void Mcal_FlashClearBank0Flags( void );
static uint32_t Mcal_FlashGetPageIndex( uint32_t ulAddr );
static void Mcal_FlashFeedDogIfNeeded( void );

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local variables
|    Generated from source/mcal/config/mcal_flash_config.md; do not edit GEN blocks
|******************************************************************************/
/* GEN_MCAL_FLASH_CFG_BEGIN */
static const xMcal_FlashCfg_t s_xMcal_FlashCfg = {
    0X08000000U,
    0X800U,
    7U,
    1U,
};
/* GEN_MCAL_FLASH_CFG_END */

/*******************************************************************************
|    Function Source Code
|******************************************************************************/
/*******************************************************************************
Name            : Mcal_FlashClearBank0Flags
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Clear FMC bank0 status flags
Call By         : Mcal_FlashInit, Mcal_FlashErase
|******************************************************************************/
static void Mcal_FlashClearBank0Flags( void )
{
    fmc_flag_clear( FMC_FLAG_BANK0_END );
    fmc_flag_clear( FMC_FLAG_BANK0_WPERR );
    fmc_flag_clear( FMC_FLAG_BANK0_PGERR );
}

/*******************************************************************************
Name            : Mcal_FlashGetPageIndex
Parameters(in)  : ulAddr - flash address
Parameters(out) : None
Return value    : Page index relative to flash base
Description     : Convert flash address to page index
Call By         : Mcal_FlashErase
|******************************************************************************/
static uint32_t Mcal_FlashGetPageIndex( uint32_t ulAddr )
{
    return ( ulAddr - s_xMcal_FlashCfg.ulBaseAddr ) / s_xMcal_FlashCfg.ulPageSize;
}

/*******************************************************************************
Name            : Mcal_FlashFeedDogIfNeeded
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Feed watchdog during long flash operations when enabled
Call By         : Mcal_FlashErase, Mcal_FlashWrite, Mcal_FlashRead
|******************************************************************************/
static void Mcal_FlashFeedDogIfNeeded( void )
{
    if ( s_xMcal_FlashCfg.ucFeedDogOnLongOp != 0U )
    {
        Mcal_FeedDog();
    }
}

/*******************************************************************************
Name            : Mcal_FlashInit
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Unlock flash, clear status flags and lock flash
Call By         : Application
|******************************************************************************/
void Mcal_FlashInit( void )
{
    fmc_unlock();
    Mcal_FlashClearBank0Flags();
    fmc_lock();
}

/*******************************************************************************
Name            : Mcal_FlashErase
Parameters(in)  : ulStartAddr - erase region start address
                  ulStopAddr - start address of page after erase region
Parameters(out) : None
Return value    : MCAL_FLASH_OK on success, MCAL_FLASH_FAIL on failure
Description     : Erase flash pages from start to stop (exclusive)
Call By         : Application
|******************************************************************************/
int Mcal_FlashErase( uint32_t ulStartAddr, uint32_t ulStopAddr )
{
    fmc_state_enum eFmcState = FMC_READY;
    uint32_t ulEraseIdx;
    uint32_t ulPageNum;

    if ( ulStopAddr <= ulStartAddr )
    {
        return MCAL_FLASH_FAIL;
    }

    ulPageNum = Mcal_FlashGetPageIndex( ulStopAddr ) - Mcal_FlashGetPageIndex( ulStartAddr );

    fmc_unlock();
    Mcal_FlashClearBank0Flags();

    for ( ulEraseIdx = 0U; ulEraseIdx < ulPageNum; ulEraseIdx++ )
    {
        Mcal_FlashFeedDogIfNeeded();
        eFmcState = fmc_page_erase( ulStartAddr + ( s_xMcal_FlashCfg.ulPageSize * ulEraseIdx ) );
        Mcal_FlashClearBank0Flags();
        if ( eFmcState != FMC_READY )
        {
            break;
        }
    }

    fmc_lock();

    return ( eFmcState == FMC_READY ) ? MCAL_FLASH_OK : MCAL_FLASH_FAIL;
}

/*******************************************************************************
Name            : Mcal_FlashWrite
Parameters(in)  : ulWrAddr - write start address (word aligned)
                  pucData - source buffer
                  ulLen - write length in bytes (multiple of 4)
Parameters(out) : None
Return value    : MCAL_FLASH_OK on success, MCAL_FLASH_FAIL on failure
Description     : Program flash in 32-bit words
Call By         : Application
|******************************************************************************/
int Mcal_FlashWrite( uint32_t ulWrAddr, uint8_t* pucData, uint32_t ulLen )
{
    fmc_state_enum eFmcState = FMC_READY;
    uint32_t ulIdx;
    uint32_t ulDataWord;

    if ( ( pucData == NULL ) || ( ulLen == 0U ) || ( ( ulLen % 4U ) != 0U ) )
    {
        return MCAL_FLASH_FAIL;
    }

    fmc_unlock();

    for ( ulIdx = 0U; ulIdx < ulLen; ulIdx += 4U )
    {
        if ( ( ( ulWrAddr + ulIdx ) % 4U ) != 0U )
        {
            eFmcState = FMC_PGERR;
            break;
        }

        Mcal_FlashFeedDogIfNeeded();
        (void)memcpy( &ulDataWord, pucData + ulIdx, sizeof( ulDataWord ) );

        if ( fmc_word_program( ulWrAddr + ulIdx, ulDataWord ) == FMC_READY )
        {
            if ( *(volatile uint32_t*)( ulWrAddr + ulIdx ) != ulDataWord )
            {
                eFmcState = FMC_PGERR;
                break;
            }
        }
        else
        {
            eFmcState = FMC_PGERR;
            break;
        }
    }

    fmc_lock();

    return ( eFmcState == FMC_READY ) ? MCAL_FLASH_OK : MCAL_FLASH_FAIL;
}

/*******************************************************************************
Name            : Mcal_FlashRead
Parameters(in)  : ulRdAddr - read start address
                  ulLen - read length in bytes
Parameters(out) : pucData - destination buffer
Return value    : None
Description     : Read flash data byte by byte
Call By         : Application
|******************************************************************************/
void Mcal_FlashRead( uint32_t ulRdAddr, uint8_t* pucData, uint32_t ulLen )
{
    uint32_t ulIdx;

    if ( pucData == NULL )
    {
        return;
    }

    for ( ulIdx = 0U; ulIdx < ulLen; ulIdx++ )
    {
        Mcal_FlashFeedDogIfNeeded();
        pucData[ulIdx] = *(volatile uint8_t*)( ulRdAddr + ulIdx );
    }
}
