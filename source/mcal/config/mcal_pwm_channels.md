# MCAL PWM 配置

> **使用方式：** 在下方表格中增删 PWM 通道行，保存后执行：
>
> ```bash
> python tools/gen_mcal_pwm.py
> ```
>
> 然后手动更新 `mcal_pwm.h` 中的 GEN 枚举区块（或复制 `tools/mcal_pwm_enum_snippet.txt`），再重新编译 Keil 工程。
>
> **说明：** 定时器/GPIO 时钟与引脚复用由系统模块或 `mcal_gpio` 配置；本驱动仅完成 PWM 输出参数。

---

## PWM 全局参数

| 定时器时钟Hz |
| ------------ |
| 96000000 |

---

## PWM 通道列表

| 启用 | 枚举名 | 说明 | 定时器 | 通道 | 预分频 | 周期 | 对齐模式 | 计数方向 | 时钟分频 | 默认启动 | 默认频率Hz | 默认脉宽us |
| ---- | ------ | ---- | ------ | ---- | ------ | ---- | -------- | -------- | -------- | -------- | ---------- | ---------- |
| yes | eMcal_PwmPump | 迷你泵 PWM PB9 | TIMER3 | TIMER_CH_3 | 8 | 9999 | TIMER_COUNTER_EDGE | TIMER_COUNTER_UP | TIMER_CKDIV_DIV1 | yes | 1000 | 0 |

> 频率计算（96MHz）：`f = 定时器时钟 / ((预分频+1) * (周期+1))`  
> 也可运行时调用 `Mcal_PwmSetFreqDuty()` 自动重算预分频与周期。

---

## 字段说明

| 字段 | 必填 | 说明 |
| ---- | ---- | ---- |
| 启用 | 是 | `yes` 参与编译生成 |
| 枚举名 | 是 | C 枚举成员名，建议 `eMcal_PwmXxx` |
| 定时器 | 是 | 如 `TIMER3` |
| 通道 | 是 | 如 `TIMER_CH_3` |
| 预分频 | 是 | 定时器预分频值 |
| 周期 | 是 | 自动重装载值 |
| 默认启动 | 是 | `yes` 初始化后启动 PWM |
| 默认频率Hz | 是 | `Mcal_PwmInit` 末尾调用的默认频率 |
| 默认脉宽us | 是 | 默认脉宽（微秒） |

---

## 代码调用示例

```c
#include "mcal_pwm.h"

Mcal_PwmInit();
Mcal_PwmSetFreqDuty( eMcal_PwmPump, 1000U, 500U );
Mcal_PwmStop( eMcal_PwmPump );
Mcal_PwmStart( eMcal_PwmPump );
```
