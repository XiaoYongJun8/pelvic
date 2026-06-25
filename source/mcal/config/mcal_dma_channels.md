# MCAL DMA 通道配置

> **使用方式：** 在下方表格中增删 DMA 行，保存后执行：
>
> ```bash
> python tools/gen_mcal_dma.py
> ```
>
> 然后手动更新 `mcal_dma.h` 中的 GEN 枚举区块（或复制 `tools/mcal_dma_enum_snippet.txt`），再重新编译 Keil 工程。

---

## DMA 通道列表

| 启用 | 枚举名 | 说明 | DMA端口 | 通道 | 循环 | 方向 | 外设宽度 | 内存宽度 | 传输数量 | 优先级 | 外设增量 | 内存增量 | 外设地址 | 缓冲类型 | 缓冲长度 | USART外设 |
| ---- | ------ | ---- | ------- | ---- | ---- | ---- | -------- | -------- | -------- | ------ | -------- | -------- | -------- | -------- | -------- | --------- |
| yes | eMcal_DmaAdc0 | ADC0 规则通道 DMA | DMA0 | DMA_CH0 | 1 | DMA_PERIPHERAL_TO_MEMORY | DMA_PERIPHERAL_WIDTH_16BIT | DMA_MEMORY_WIDTH_16BIT | 1 | DMA_PRIORITY_HIGH | DMA_PERIPH_INCREASE_DISABLE | DMA_MEMORY_INCREASE_ENABLE | 0x4001244CU | uint16_t | 1 | 0 |
| yes | eMcal_DmaUart0Tx | USART0 发送 DMA | DMA0 | DMA_CH3 | 0 | DMA_MEMORY_TO_PERIPHERAL | DMA_PERIPHERAL_WIDTH_8BIT | DMA_MEMORY_WIDTH_8BIT | 156 | DMA_PRIORITY_HIGH | DMA_PERIPH_INCREASE_DISABLE | DMA_MEMORY_INCREASE_ENABLE | 0x40013804U | uint8_t | 156 | USART0 |
| yes | eMcal_DmaUart1Tx | USART1 发送 DMA | DMA0 | DMA_CH5 | 0 | DMA_MEMORY_TO_PERIPHERAL | DMA_PERIPHERAL_WIDTH_8BIT | DMA_MEMORY_WIDTH_8BIT | 156 | DMA_PRIORITY_HIGH | DMA_PERIPH_INCREASE_DISABLE | DMA_MEMORY_INCREASE_ENABLE | 0x40004404U | uint8_t | 156 | USART1 |
| yes | eMcal_DmaUart2Tx | USART2 发送 DMA | DMA0 | DMA_CH1 | 0 | DMA_MEMORY_TO_PERIPHERAL | DMA_PERIPHERAL_WIDTH_8BIT | DMA_MEMORY_WIDTH_8BIT | 156 | DMA_PRIORITY_HIGH | DMA_PERIPH_INCREASE_DISABLE | DMA_MEMORY_INCREASE_ENABLE | 0x40004804U | uint8_t | 156 | USART2 |

---

## 字段说明

| 字段 | 必填 | 说明 |
| ---- | ---- | ---- |
| 启用 | 是 | `yes` 参与编译生成；`no` 忽略该行 |
| 枚举名 | 是 | C 枚举成员名，建议 `eMcal_DmaXxx` 格式 |
| 说明 | 否 | 注释说明 |
| DMA端口 | 是 | `DMA0` 或 `DMA1` |
| 通道 | 是 | `DMA_CH0` ~ `DMA_CH6` |
| 循环 | 是 | `1` 循环模式；`0` 普通模式 |
| 方向 | 是 | 见下方「方向可选值」 |
| 外设宽度 | 是 | 见下方「宽度可选值」 |
| 内存宽度 | 是 | 见下方「宽度可选值」 |
| 传输数量 | 是 | 单次传输数据个数 |
| 优先级 | 是 | `DMA_PRIORITY_LOW` / `MEDIUM` / `HIGH` / `ULTRA_HIGH` |
| 外设增量 | 是 | `DMA_PERIPH_INCREASE_ENABLE` / `DISABLE` |
| 内存增量 | 是 | `DMA_MEMORY_INCREASE_ENABLE` / `DISABLE` |
| 外设地址 | 是 | 外设寄存器地址，如 `0x4001244CU`（ADC0_RDATA）、`0x40013804U`（USART0_DATA） |
| 缓冲类型 | 是 | DMA 内存缓冲元素类型：`uint16_t` 或 `uint8_t` |
| 缓冲长度 | 是 | 缓冲数组元素个数，应 >= 传输数量 |
| USART外设 | 是 | 非串口填 `0`；串口 TX 填 `USART0` / `USART1` / `USART2` |

---

## 方向可选值

| 值 | 说明 |
| ---- | ---- |
| DMA_PERIPHERAL_TO_MEMORY | 外设到内存（ADC 采集常用） |
| DMA_MEMORY_TO_PERIPHERAL | 内存到外设（UART TX 常用） |

---

## 宽度可选值

| 值 | 说明 |
| ---- | ---- |
| DMA_PERIPHERAL_WIDTH_8BIT | 外设 8 位 |
| DMA_PERIPHERAL_WIDTH_16BIT | 外设 16 位 |
| DMA_PERIPHERAL_WIDTH_32BIT | 外设 32 位 |
| DMA_MEMORY_WIDTH_8BIT | 内存 8 位 |
| DMA_MEMORY_WIDTH_16BIT | 内存 16 位 |
| DMA_MEMORY_WIDTH_32BIT | 内存 32 位 |

---

## USART TX DMA 通道映射（GD32F303）

| 串口 | DMA | 通道 | 外设地址 |
| ---- | --- | ---- | -------- |
| USART0 TX | DMA0 | DMA_CH3 | 0x40013804U |
| USART1 TX | DMA0 | DMA_CH5 | 0x40004404U |
| USART2 TX | DMA0 | DMA_CH1 | 0x40004804U |

---

## 添加 UART TX DMA 示例

（已在上方「DMA 通道列表」中配置 USART0/1/2 TX，此处仅作字段参考。）

| 启用 | 枚举名 | 说明 | DMA端口 | 通道 | 循环 | 方向 | 外设宽度 | 内存宽度 | 传输数量 | 优先级 | 外设增量 | 内存增量 | 外设地址 | 缓冲类型 | 缓冲长度 | USART外设 |
| ---- | ------ | ---- | ------- | ---- | ---- | ---- | -------- | -------- | -------- | ------ | -------- | -------- | -------- | -------- | -------- | --------- |
| yes | eMcal_DmaUart0Tx | USART0 发送 DMA | DMA0 | DMA_CH3 | 0 | DMA_MEMORY_TO_PERIPHERAL | DMA_PERIPHERAL_WIDTH_8BIT | DMA_MEMORY_WIDTH_8BIT | 156 | DMA_PRIORITY_HIGH | DMA_PERIPH_INCREASE_DISABLE | DMA_MEMORY_INCREASE_ENABLE | 0x40013804U | uint8_t | 156 | USART0 |

保存 `.md` 后运行 `python tools/gen_mcal_dma.py`，更新头文件枚举，再编译工程。

---

## 代码调用示例

```c
#include "mcal_dma.h"

Mcal_DmaInit();
uint16_t* pusAdcBuf = Mcal_GetDmaMemory16( eMcal_DmaAdc0 );

Mcal_DmaUartSend( eMcal_DmaUart0Tx, (uint8_t*)"OK\r\n", 4U );
Mcal_DmaUartSend( eMcal_DmaUart1Tx, (uint8_t*)"OK\r\n", 4U );
Mcal_DmaUartSend( eMcal_DmaUart2Tx, (uint8_t*)"OK\r\n", 4U );
```
