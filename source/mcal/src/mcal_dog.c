/*******************************************************************************
|  File Name:  mcal_dog.c
|  Description:  Implementation of MCAL watchdog driver
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
#include "mcal_dog.h"
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
static void Mcal_DogConfigPeriph( void );

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Static Local variables
|    Generated from source/mcal/config/mcal_dog_config.md; do not edit GEN blocks
|******************************************************************************/
/* GEN_MCAL_DOG_CFG_BEGIN */
static const xMcal_DogCfg_t s_xMcal_DogCfg = {
    1250U,
    FWDGT_PSC_DIV64,
    1U,
};
/* GEN_MCAL_DOG_CFG_END */

/*******************************************************************************
|    Function Source Code
|******************************************************************************/
/*******************************************************************************
Name            : Mcal_DogConfigPeriph
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Configure and enable the free watchdog timer
Call By         : Mcal_DogInit
|******************************************************************************/
static void Mcal_DogConfigPeriph( void )
{
    fwdgt_write_enable();
    fwdgt_config( s_xMcal_DogCfg.usReload, s_xMcal_DogCfg.ucPrescaler );
    fwdgt_write_disable();
    fwdgt_enable();
}

/*******************************************************************************
Name            : Mcal_DogInit
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Initialize and start the configured watchdog
Call By         : Application
|******************************************************************************/
void Mcal_DogInit( void )
{
    Mcal_DogConfigPeriph();

    if ( s_xMcal_DogCfg.ucFeedOnInit != 0U )
    {
        Mcal_FeedDog();
    }
}

/*******************************************************************************
Name            : Mcal_FeedDog
Parameters(in)  : None
Parameters(out) : None
Return value    : None
Description     : Reload the watchdog counter
Call By         : Application
|******************************************************************************/
void Mcal_FeedDog( void )
{
    fwdgt_write_enable();
    fwdgt_counter_reload();
}
