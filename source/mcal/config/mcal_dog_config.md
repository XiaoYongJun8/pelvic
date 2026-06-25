# MCAL 看门狗配置

> **使用方式：** 修改下方参数表，保存后执行：
>
> ```bash
> python tools/gen_mcal_dog.py
> ```
>
> 然后重新编译 Keil 工程。
>
> **说明：** IRC40K 时钟由系统模块在调用 `Mcal_DogInit()` 前使能并等待稳定。

---

## 看门狗参数

| 启用 | 看门狗类型 | 重装载值 | 预分频 | 初始化喂狗 |
| ---- | ---------- | -------- | ------ | ---------- |
| yes | FWDGT | 1250 | FWDGT_PSC_DIV64 | yes |

> 超时时间（秒）≈ `重装载值 × 预分频系数 / 40000`（IRC40K 时钟 40kHz）  
> 上表配置：1250 × 64 / 40000 ≈ **2 秒**

---

## 字段说明

| 字段 | 必填 | 说明 |
| ---- | ---- | ---- |
| 启用 | 是 | `yes` 参与编译生成 |
| 看门狗类型 | 是 | 当前支持 `FWDGT`（独立看门狗） |
| 重装载值 | 是 | 0~4095，写入 FWDGT 重装载寄存器 |
| 预分频 | 是 | 见下方「预分频可选值」 |
| 初始化喂狗 | 是 | `yes` 在 `Mcal_DogInit` 末尾喂狗一次 |

---

## 预分频可选值

| 值 | 系数 |
| ---- | ---- |
| FWDGT_PSC_DIV4 | 4 |
| FWDGT_PSC_DIV8 | 8 |
| FWDGT_PSC_DIV16 | 16 |
| FWDGT_PSC_DIV32 | 32 |
| FWDGT_PSC_DIV64 | 64 |
| FWDGT_PSC_DIV128 | 128 |
| FWDGT_PSC_DIV256 | 256 |

---

## 代码调用示例

```c
#include "mcal_dog.h"

Mcal_DogInit();

while ( 1 )
{
    Mcal_FeedDog();
}
```
