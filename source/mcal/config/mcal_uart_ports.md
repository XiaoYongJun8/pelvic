# MCAL UART 端口配置

> **使用方式：** 在下方表格中增删 UART 行，保存后执行：
>
> ```bash
> python tools/gen_mcal_uart.py
> ```
>
> 然后手动更新 `mcal_uart.h` 中的 GEN 枚举区块（或复制 `tools/mcal_uart_enum_snippet.txt`），再重新编译 Keil 工程。
>
> **说明：** 外设/GPIO 时钟与 NVIC 中断由系统模块统一配置，本驱动不负责使能。
>
> **初始化顺序：** `Mcal_DmaInit()` → `Mcal_UartInit()`（DMA 发送需在 DMA 初始化后配置串口）

---

## UART 端口列表

| 启用 | 枚举名 | 说明 | USART | 波特率 | 数据位 | 停止位 | 校验 | DMA发送 | DMA通道 | TX端口 | TX引脚 | RX端口 | RX引脚 | 复用 | RX缓冲长度 |
| ---- | ------ | ---- | ----- | ------ | ------ | ------ | ---- | ------- | ------- | ------ | ------ | ------ | ------ | ---- | ---------- |
| yes | eMcal_Uart0 | USART0 | USART0 | 115200 | USART_WL_8BIT | USART_STB_1BIT | USART_PM_NONE | yes | eMcal_DmaUart0Tx | GPIOA | GPIO_PIN_9 | GPIOA | GPIO_PIN_10 | 0 | 256 |
| yes | eMcal_Uart1 | USART1 | USART1 | 115200 | USART_WL_8BIT | USART_STB_1BIT | USART_PM_NONE | yes | eMcal_DmaUart1Tx | GPIOA | GPIO_PIN_2 | GPIOA | GPIO_PIN_3 | 0 | 256 |
| yes | eMcal_Uart2 | USART2 | USART2 | 115200 | USART_WL_8BIT | USART_STB_1BIT | USART_PM_NONE | yes | eMcal_DmaUart2Tx | GPIOB | GPIO_PIN_10 | GPIOB | GPIO_PIN_11 | 0 | 256 |

---

## 字段说明

| 字段 | 必填 | 说明 |
| ---- | ---- | ---- |
| 启用 | 是 | `yes` 参与编译生成；`no` 忽略该行 |
| 枚举名 | 是 | C 枚举成员名，建议 `eMcal_UartX` 格式 |
| USART | 是 | `USART0` / `USART1` / `USART2` |
| 波特率 | 是 | 如 `115200` |
| 数据位 | 是 | `USART_WL_8BIT` / `USART_WL_9BIT` |
| 停止位 | 是 | `USART_STB_1BIT` / `USART_STB_0_5BIT` 等 |
| 校验 | 是 | `USART_PM_NONE` / `USART_PM_EVEN` / `USART_PM_ODD` |
| DMA发送 | 是 | `yes` 使用 `mcal_dma` 发送；`no` 使用轮询发送 |
| DMA通道 | 是 | 对应 `mcal_dma.h` 中的 TX 通道枚举 |
| TX/RX端口/引脚 | 是 | GD32 GPIO 宏 |
| 复用 | 是 | 无复用填 `0`；USART0 重映射填 `GPIO_USART0_REMAP` |
| RX缓冲长度 | 是 | 接收环形缓冲字节数 |

---

## 代码调用示例

```c
#include "mcal_uart.h"
#include "mcal_dma.h"

Mcal_DmaInit();
Mcal_UartInit();

Mcal_UartSend( eMcal_Uart0, (uint8_t*)"hello\r\n", 7U );

/* 系统模块注册 NVIC 后，在中断里调用 Mcal_UartIrqHandler( USART0 ) */
uint8_t ucByte;
if ( Mcal_UartReadChar( eMcal_Uart0, &ucByte ) == 0 ) {
    /* got one byte */
}
```
