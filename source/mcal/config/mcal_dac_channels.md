# MCAL DAC 配置

> **使用方式：** 在下方表格中增删 DAC 输出通道行，保存后执行：
>
> ```bash
> python tools/gen_mcal_dac.py
> ```
>
> 然后手动更新 `mcal_dac.h` 中的 GEN 枚举区块（或复制 `tools/mcal_dac_enum_snippet.txt`），再重新编译 Keil 工程。
>
> **说明：** DAC 外设时钟由系统模块统一配置；本驱动仅完成 DAC 参数与输出通道初始化。

---

## DAC 实例参数

| DAC端口 | 数据对齐 |
| ------- | -------- |
| DAC0 | DAC_ALIGN_12B_R |

---

## DAC 输出通道列表

| 启用 | 枚举名 | 说明 | 输出通道 | 输出缓冲 | 初始值 |
| ---- | ------ | ---- | -------- | -------- | ------ |
| yes | eMcal_DacStimCur | 刺激电流 DAC | DAC_OUT0 | yes | 0 |
| yes | eMcal_DacStimBoost | 刺激升压 DAC | DAC_OUT1 | yes | 0 |

---

## 字段说明

| 字段 | 必填 | 说明 |
| ---- | ---- | ---- |
| 启用 | 是 | `yes` 参与编译生成；`no` 忽略该行 |
| 枚举名 | 是 | C 枚举成员名，建议 `eMcal_DacXxx` |
| 输出通道 | 是 | `DAC_OUT0` 或 `DAC_OUT1` |
| 输出缓冲 | 是 | `yes` 使能输出缓冲；`no` 关闭 |
| 初始值 | 是 | 12 位 DAC 初始输出值（0~4095） |

---

## 数据对齐可选值

| 值 | 说明 |
| ---- | ---- |
| DAC_ALIGN_12B_R | 12 位右对齐（常用） |
| DAC_ALIGN_12B_L | 12 位左对齐 |
| DAC_ALIGN_8B_R | 8 位右对齐 |

---

## 代码调用示例

```c
#include "mcal_dac.h"

Mcal_DacInit();
Mcal_DacSetOutValue( eMcal_DacStimCur, 2048U );
Mcal_DacSetOutValue( eMcal_DacStimBoost, 0U );
```
