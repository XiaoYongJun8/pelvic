#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate template source files per embedded firmware code style spec."""

import os

BASE = os.path.join(os.path.dirname(os.path.dirname(__file__)), "source")


def make_guard(name: str) -> str:
    return f"_{name.upper().replace('.', '_').replace('-', '_')}_H_"


def h_template(fname: str, desc: str, includes=None) -> str:
    guard = make_guard(fname.replace(".h", ""))
    inc_list = includes or ["<stdint.h>"]
    inc_block = "\n".join(f"#include {i}" for i in inc_list)
    return f"""/*******************************************************************************
|  File Name:  {fname}
|  Description:  {desc}
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

#ifndef {guard}
#define {guard}

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
{inc_block}

#ifdef __cplusplus
extern "C" {{
#endif

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
|    Global Function Prototypes
|******************************************************************************/

#ifdef __cplusplus
}}
#endif

#endif /* {guard} */
"""


def c_template(fname: str, hname: str, desc: str) -> str:
    return f"""/*******************************************************************************
|  File Name:  {fname}
|  Description:  {desc}
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
#include "{hname}"

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

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Function Source Code
|******************************************************************************/
"""


FILES = {
    "app/appstim/appstim.h": h_template("appstim.h", "Head file of the stimulation application module"),
    "app/appstim/appstim.c": c_template("appstim.c", "appstim.h", "Implementation of the stimulation application module"),
    "app/appemg/appemg.h": h_template("appemg.h", "Head file of the EMG application module"),
    "app/appemg/appemg.c": c_template("appemg.c", "appemg.h", "Implementation of the EMG application module"),
    "rte/rte.h": h_template("rte.h", "Head file of the runtime environment"),
    "rte/rte.c": c_template("rte.c", "rte.h", "Implementation of the runtime environment"),
    "bsw/systemservice/sysmclock/sysm_clock.h": h_template("sysm_clock.h", "Head file of system clock configuration"),
    "bsw/systemservice/sysmclock/sysm_clock.c": c_template("sysm_clock.c", "sysm_clock.h", "Implementation of system clock configuration"),
    "bsw/systemservice/sysmnvic/sysm_nvic.h": h_template("sysm_nvic.h", "Head file of system NVIC configuration"),
    "bsw/systemservice/sysmnvic/sysm_nvic.c": c_template("sysm_nvic.c", "sysm_nvic.h", "Implementation of system NVIC configuration"),
    "bsw/systemservice/sysmdog/sysm_dog.h": h_template("sysm_dog.h", "Head file of system watchdog configuration"),
    "bsw/systemservice/sysmdog/sysm_dog.c": c_template("sysm_dog.c", "sysm_dog.h", "Implementation of system watchdog configuration"),
    "bsw/systemservice/sysmdelay/sysm_delay.h": h_template("sysm_delay.h", "Head file of system delay functions"),
    "bsw/systemservice/sysmdelay/sysm_delay.c": c_template("sysm_delay.c", "sysm_delay.h", "Implementation of system delay functions"),
    "bsw/systemservice/sysminit/sysm_init.h": h_template("sysm_init.h", "Head file of system power-on initialization"),
    "bsw/systemservice/sysminit/sysm_init.c": c_template("sysm_init.c", "sysm_init.h", "Implementation of system power-on initialization"),
    "bsw/systemservice/log/log.h": h_template("log.h", "Head file of system log module"),
    "bsw/systemservice/log/log.c": c_template("log.c", "log.h", "Implementation of system log module"),
    "bsw/complexdriver/gx2431/driver_gx2431.h": h_template("driver_gx2431.h", "Head file of GX2431 sensor driver"),
    "bsw/complexdriver/gx2431/driver_gx2431.c": c_template("driver_gx2431.c", "driver_gx2431.h", "Implementation of GX2431 sensor driver"),
    "bsw/complexdriver/dht11/driver_dht11.h": h_template("driver_dht11.h", "Head file of DHT11 sensor driver"),
    "bsw/complexdriver/dht11/driver_dht11.c": c_template("driver_dht11.c", "driver_dht11.h", "Implementation of DHT11 sensor driver"),
    "bsw/utilsservice/utilsfifo/utils_fifo.h": h_template("utils_fifo.h", "Head file of FIFO utility"),
    "bsw/utilsservice/utilsfifo/utils_fifo.c": c_template("utils_fifo.c", "utils_fifo.h", "Implementation of FIFO utility"),
    "bsw/utilsservice/utilscrc/utils_crc.h": h_template("utils_crc.h", "Head file of CRC utility"),
    "bsw/utilsservice/utilscrc/utils_crc.c": c_template("utils_crc.c", "utils_crc.h", "Implementation of CRC utility"),
    "mcal/inc/mcal_gpio.h": h_template("mcal_gpio.h", "Head file of MCAL GPIO driver"),
    "mcal/src/mcal_gpio.c": c_template("mcal_gpio.c", "mcal_gpio.h", "Implementation of MCAL GPIO driver"),
    "mcal/inc/mcal_adc.h": h_template("mcal_adc.h", "Head file of MCAL ADC driver"),
    "mcal/src/mcal_adc.c": c_template("mcal_adc.c", "mcal_adc.h", "Implementation of MCAL ADC driver"),
    "mcal/inc/mcal_uart.h": h_template("mcal_uart.h", "Head file of MCAL UART driver"),
    "mcal/src/mcal_uart.c": c_template("mcal_uart.c", "mcal_uart.h", "Implementation of MCAL UART driver"),
    "config/sys_config.h": h_template("sys_config.h", "System global configuration header"),
    "task/mytask.h": h_template("mytask.h", "Head file of system task management"),
    "task/mytask.c": c_template("mytask.c", "mytask.h", "Implementation of system task management"),
}

MAIN_C = """/*******************************************************************************
|  File Name:  main.c
|  Description:  System entry point - initialization and task scheduling
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
#include "sysm_init.h"
#include "mytask.h"
#include "rte.h"

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
static void Main_SystemInit( void );

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Function Source Code
|******************************************************************************/

int main( void )
{
    Main_SystemInit();
    MyTask_StartScheduler();

    while ( 1 ) {
    }

    return 0;
}

static void Main_SystemInit( void )
{
    Sysm_Init();
    Rte_Init();
}
"""


def main():
    for rel, content in FILES.items():
        path = os.path.join(BASE, rel.replace("/", os.sep))
        with open(path, "w", encoding="utf-8", newline="\n") as f:
            f.write(content)

    with open(os.path.join(BASE, "main.c"), "w", encoding="utf-8", newline="\n") as f:
        f.write(MAIN_C)

    print(f"Generated {len(FILES) + 1} template files under {BASE}")


if __name__ == "__main__":
    main()
