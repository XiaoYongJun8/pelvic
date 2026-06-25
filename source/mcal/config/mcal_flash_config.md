# MCAL Flash 配置

> **使用方式：** 修改下方参数表，保存后执行：
>
> ```bash
> python tools/gen_mcal_flash.py
> ```
>
> 然后手动更新 `mcal_flash.h` 中的 GEN 宏区块（或复制 `tools/mcal_flash_macro_snippet.txt`），再重新编译 Keil 工程。
>
> **说明：** FMC 外设时钟由系统模块统一配置。

---

## Flash 参数

| Flash基址 | 页大小 | 用户区页数 | 长操作喂狗 |
| ----------- | ------ | ---------- | ---------- |
| 0x08000000 | 0x800 | 7 | yes |

> 用户区结束地址 = `Flash基址 + 页大小 × 用户区页数 - 1`  
> 上表：用户区 `0x08000000` ~ `0x080037FF`（14 KB）

---

## 字段说明

| 字段 | 必填 | 说明 |
| ---- | ---- | ---- |
| Flash基址 | 是 | 片上 Flash 起始地址，GD32 通常为 `0x08000000` |
| 页大小 | 是 | GD32F303 页大小为 `0x800`（2 KB） |
| 用户区页数 | 是 | 应用可擦写区域占用的页数（从基址起连续） |
| 长操作喂狗 | 是 | `yes` 在擦除/读写循环中调用 `Mcal_FeedDog()` |

---

## 擦除地址说明

`Mcal_FlashErase( ulStartAddr, ulStopAddr )` 中：

- `ulStartAddr`：待擦除区域起始地址（页对齐）
- `ulStopAddr`：待擦除区域**下一页**的起始地址（不包含该页）

示例：擦除第 0、1 页 → `Mcal_FlashErase( 0x08000000, 0x08001000 )`

---

## 代码调用示例

```c
#include "mcal_flash.h"

uint8_t aucBuf[16];

Mcal_FlashInit();
Mcal_FlashErase( MCAL_FLASH_START_ADDR, MCAL_FLASH_START_ADDR + MCAL_FLASH_PAGE_SIZE );
Mcal_FlashWrite( MCAL_FLASH_START_ADDR, aucBuf, sizeof( aucBuf ) );
Mcal_FlashRead( MCAL_FLASH_START_ADDR, aucBuf, sizeof( aucBuf ) );
```
