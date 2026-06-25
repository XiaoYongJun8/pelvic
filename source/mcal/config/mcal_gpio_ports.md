# MCAL GPIO 端口配置

> **使用方式：** 在下方表格中增删 GPIO 行，保存后执行：
>
> ```bash
> python tools/gen_mcal_gpio.py
> ```
>
> 然后重新编译 Keil 工程即可生效。

---

## GPIO 端口列表

| 启用 | 枚举名 | 说明 | 端口 | 引脚 | 模式 | 速度 | 默认电平 | 复用 |
| ---- | ------ | ---- | ---- | ---- | ---- | ---- | -------- | ---- |
| yes | eMcal_GpioLedOp | 状态指示灯推挽输出 | GPIOA | GPIO_PIN_0 | GPIO_MODE_OUT_PP | GPIO_OSPEED_50MHZ | 0 | 0 |
| yes | eMcal_GpioPb2In | PB2 上拉输入 | GPIOB | GPIO_PIN_2 | GPIO_MODE_IPU | GPIO_OSPEED_50MHZ | 0 | 0 |

---

## 字段说明

| 字段 | 必填 | 说明 |
| ---- | ---- | ---- |
| 启用 | 是 | `yes` 参与编译生成；`no` 忽略该行 |
| 枚举名 | 是 | C 枚举成员名，建议 `eMcal_GpioXxx` 格式 |
| 说明 | 否 | 注释，会写入生成的枚举注释 |
| 端口 | 是 | GD32 端口宏，如 `GPIOA`、`GPIOB`、`GPIOC` |
| 引脚 | 是 | GD32 引脚宏，如 `GPIO_PIN_0` |
| 模式 | 是 | 见下方「模式可选值」 |
| 速度 | 是 | 见下方「速度可选值」 |
| 默认电平 | 是 | 输出模式初始电平：`0` 低电平，`1` 高电平 |
| 复用 | 是 | 无复用填 `0`；USART 等填对应 remap 宏 |

---

## 模式可选值

| 值 | 说明 |
| ---- | ---- |
| GPIO_MODE_OUT_PP | 推挽输出（常用） |
| GPIO_MODE_OUT_OD | 开漏输出 |
| GPIO_MODE_IN_FLOATING | 浮空输入 |
| GPIO_MODE_IPU | 上拉输入 |
| GPIO_MODE_IPD | 下拉输入 |
| GPIO_MODE_AIN | 模拟输入 |
| GPIO_MODE_AF_PP | 复用推挽 |
| GPIO_MODE_AF_OD | 复用开漏 |

---

## 速度可选值

| 值 | 说明 |
| ---- | ---- |
| GPIO_OSPEED_2MHZ | 2MHz |
| GPIO_OSPEED_10MHZ | 10MHz |
| GPIO_OSPEED_50MHZ | 50MHz |

---

## 复用可选值（常用）

| 值 | 说明 |
| ---- | ---- |
| 0 | 不复用 |
| GPIO_USART0_REMAP | USART0 重映射 |

---

## 添加端口示例

在「GPIO 端口列表」表格末尾新增一行，例如：

| 启用 | 枚举名 | 说明 | 端口 | 引脚 | 模式 | 速度 | 默认电平 | 复用 |
| ---- | ------ | ---- | ---- | ---- | ---- | ---- | -------- | ---- |
| yes | eMcal_GpioStimPowerEnOp | 电刺激电源使能 | GPIOB | GPIO_PIN_8 | GPIO_MODE_OUT_PP | GPIO_OSPEED_50MHZ | 0 | 0 |

保存 `.md` 后运行 `python tools/gen_mcal_gpio.py`，再编译工程。

---

## 代码调用示例

```c
#include "mcal_gpio.h"

Mcal_GpioInit();
Mcal_SetGpioPin( eMcal_GpioLedOp );
Mcal_ResetGpioPin( eMcal_GpioLedOp );
uint8_t ucLevel = Mcal_GetGpioPinStatus( eMcal_GpioLedOp );
```
