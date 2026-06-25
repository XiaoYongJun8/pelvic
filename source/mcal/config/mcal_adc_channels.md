# MCAL ADC 通道配置

> **使用方式：** 在下方表格中增删 ADC 通道行，保存后执行：
>
> ```bash
> python tools/gen_mcal_adc.py
> ```
>
> 脚本会自动同步 `mcal_dma_channels.md` 中 `eMcal_DmaAdc0` 的缓冲长度，并运行 `gen_mcal_dma.py`。
> 然后手动更新 `mcal_adc.h` 中的 GEN 枚举区块（或复制 `tools/mcal_adc_enum_snippet.txt`），再重新编译 Keil 工程。

---

## ADC 实例参数

| ADC端口 | 工作模式 | 分辨率 | 扫描模式 | 连续模式 | 数据对齐 |
| ------- | -------- | ------ | -------- | -------- | -------- |
| ADC0 | ADC_MODE_FREE | ADC_RESOLUTION_12B | ADC_SCAN_MODE | ADC_CONTINUOUS_MODE | ADC_DATAALIGN_RIGHT |

---

## ADC 通道列表

| 启用 | 枚举名 | 说明 | 硬件通道 | 采样时间 |
| ---- | ------ | ---- | -------- | -------- |
| yes | eMcal_AdcCh0 | 示例 ADC 通道 0 | ADC_CHANNEL_0 | ADC_SAMPLETIME_13POINT5 |

---

## 字段说明

| 字段 | 必填 | 说明 |
| ---- | ---- | ---- |
| 启用 | 是 | `yes` 参与编译生成；`no` 忽略该行 |
| 枚举名 | 是 | C 枚举成员名，建议 `eMcal_AdcXxx` 格式 |
| 说明 | 否 | 注释说明 |
| 硬件通道 | 是 | GD32 ADC 通道宏，如 `ADC_CHANNEL_0` |
| 采样时间 | 是 | 见下方「采样时间可选值」 |

---

## 采样时间可选值

| 值 | 说明 |
| ---- | ---- |
| ADC_SAMPLETIME_1POINT5 | 1.5 周期 |
| ADC_SAMPLETIME_7POINT5 | 7.5 周期 |
| ADC_SAMPLETIME_13POINT5 | 13.5 周期（常用） |
| ADC_SAMPLETIME_28POINT5 | 28.5 周期 |
| ADC_SAMPLETIME_41POINT5 | 41.5 周期 |
| ADC_SAMPLETIME_55POINT5 | 55.5 周期 |
| ADC_SAMPLETIME_71POINT5 | 71.5 周期 |
| ADC_SAMPLETIME_239POINT5 | 239.5 周期 |

---

## 添加通道示例

| 启用 | 枚举名 | 说明 | 硬件通道 | 采样时间 |
| ---- | ------ | ---- | -------- | -------- |
| yes | eMcal_AdcBattery | 电池电压 | ADC_CHANNEL_1 | ADC_SAMPLETIME_13POINT5 |

保存后运行 `python tools/gen_mcal_adc.py`，再编译工程。

---

## 代码调用示例

```c
#include "mcal_adc.h"
#include "mcal_dma.h"

Mcal_DmaInit();
Mcal_AdcInit();

uint16_t usValue = Mcal_GetAdcChValue( eMcal_AdcCh0 );
```
