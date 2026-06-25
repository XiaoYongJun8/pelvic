# MCAL Timer 配置

> **使用方式：** 在下方表格中增删 Timer 行，保存后执行：
>
> ```bash
> python tools/gen_mcal_timer.py
> ```
>
> 然后手动更新 `mcal_timer.h` 中的 GEN 枚举区块（或复制 `tools/mcal_timer_enum_snippet.txt`），再重新编译 Keil 工程。
>
> **说明：** Timer 外设时钟与 NVIC 由系统模块统一配置；本驱动提供定时器参数配置与中断服务函数。

---

## Timer 列表

| 启用 | 枚举名 | 说明 | 定时器 | 预分频 | 周期 | 对齐模式 | 计数方向 | 时钟分频 | 重复计数 | 默认启动 |
| ---- | ------ | ---- | ------ | ------ | ---- | -------- | -------- | -------- | -------- | -------- |
| yes | eMcal_TimerEmg | 肌电采样定时 2048Hz | TIMER5 | 2 | 15624 | TIMER_COUNTER_EDGE | TIMER_COUNTER_UP | TIMER_CKDIV_DIV1 | 0 | no |
| yes | eMcal_TimerStim | 电刺激波形 100kHz | TIMER2 | 0 | 959 | TIMER_COUNTER_EDGE | TIMER_COUNTER_UP | TIMER_CKDIV_DIV1 | 0 | no |
| yes | eMcal_TimerStimOff | 电刺激脱落检测 | TIMER4 | 0 | 1916 | TIMER_COUNTER_EDGE | TIMER_COUNTER_UP | TIMER_CKDIV_DIV1 | 0 | no |
| yes | eMcal_TimerPress | 压力上报 128Hz | TIMER1 | 999 | 749 | TIMER_COUNTER_EDGE | TIMER_COUNTER_UP | TIMER_CKDIV_DIV1 | 0 | no |

> 频率计算（APB1 定时器时钟 96MHz 时）：`f = 96000000 / ((预分频+1) * (周期+1))`

---

## 字段说明

| 字段 | 必填 | 说明 |
| ---- | ---- | ---- |
| 启用 | 是 | `yes` 参与编译生成；`no` 忽略该行 |
| 枚举名 | 是 | C 枚举成员名，建议 `eMcal_TimerXxx` |
| 定时器 | 是 | `TIMER1` ~ `TIMER6` 等 GD32 定时器宏 |
| 预分频 | 是 | 定时器预分频值（写入 `prescaler`） |
| 周期 | 是 | 自动重装载值（写入 `period`） |
| 对齐模式 | 是 | 如 `TIMER_COUNTER_EDGE` |
| 计数方向 | 是 | 如 `TIMER_COUNTER_UP` |
| 时钟分频 | 是 | 如 `TIMER_CKDIV_DIV1` |
| 重复计数 | 是 | 高级定时器重复计数，普通定时器填 `0` |
| 默认启动 | 是 | `yes` 初始化后立即启动；`no` 需调用 `Mcal_TimerStart` |

---

## 代码调用示例

```c
#include "mcal_timer.h"

Mcal_TimerInit();
Mcal_TimerSetCallFun( eMcal_TimerStim, AppStim_WaveTick );
Mcal_TimerStart( eMcal_TimerStim );
```

系统模块使能 NVIC 与 `timer_interrupt_enable( TIMERx, TIMER_INT_UP )` 后，更新中断将触发对应 `TIMERx_IRQHandler`。
