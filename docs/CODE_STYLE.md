# 嵌入式固件代码格式规范

> 本文档依据《嵌入式固件代码格式规范》整理，适用于本项目所有嵌入式 C 语言开发。

---

## 1. 规范总则

### 1.1 规范目的

本规范针对嵌入式软件开发的资源受限、实时性强、稳定性要求高、硬件耦合度高、可维护性差等核心特点制定，统一编码风格、命名规则、注释规范、语法使用准则，降低代码缺陷率、提升代码可读性 / 可移植性 / 可复用性，规避嵌入式场景特有 BUG（栈溢出、内存泄漏、中断异常、时序错误等），保障嵌入式软件长期稳定运行，同时便于团队协作与后期维护迭代。

### 1.2 适用范围

本规范适用于所有嵌入式 C 语言开发（主流嵌入式开发语言），包含裸机程序、RTOS（FreeRTOS/RT-Thread/uC/OS）、Linux 驱动 / 应用层嵌入式开发，覆盖 MCU/MPU 全场景，所有嵌入式开发人员必须严格遵守。

### 1.3 核心原则

1. 嵌入式开发优先保证 **代码的安全性、稳定性、可读性**，其次考虑执行效率，最后考虑代码简洁性；
2. 严禁为追求「精简代码」牺牲可读性和稳定性，嵌入式软件的 BUG 往往直接导致硬件故障、系统崩溃，无容错空间；
3. 所有代码必须具备可移植性，尽量减少与具体硬件寄存器的强耦合，预留硬件适配层；
4. 代码逻辑清晰、模块化解耦，单个函数 / 模块职责单一，杜绝「万能函数 / 万能模块」。

---

## 2. 变量定义规范

### 2.1 变量定义格式

变量定义严格按照 `[存储类别] [数据类型] [变量名] [=] [初始化值]` 的格式进行定义。

| 要素 | 说明 |
|------|------|
| 存储类别 | 如 `static`、`extern`、`register`、`volatile`、`const` 等（可选）；中断内和中断外都有使用的全局/静态变量必须加 `volatile` 修饰；仅本文件使用的变量必须加 `static` 修饰 |
| 数据类型 | 优先使用 `int8_t`、`uint8_t`、`uint32_t`、`int32_t` 等跨平台类型；非必要不使用 `char`、`int`、`short` 等 |
| 变量名 | 遵循命名规范（见 2.2） |
| 初始化 | 定义时推荐初始化全局/静态变量；局部变量使用前必须初始化 |
| 定义位置 | 全局/静态变量在文件模板指定位置定义；局部变量在函数体开头定义；禁止在文件非指定位置定义全局/静态变量；禁止在函数体中间穿插定义局部变量（`for` 循环等特殊代码块除外） |
| 栈变量 | 大数组或超大结构体（>256 bytes）禁止定义为局部变量（含任务中变量） |

### 2.2 变量命名

- **清晰且有描述性**：体现「功能+属性」，禁止使用拼音、拼音缩写、中英文混合，全部使用英文单词/英文缩写
- **缩写**：能够清晰表示变量作用（如 `idx` 代替 `index`），避免歧义
- **前/后缀约定**：参考 [附录 7.1](#71-变量前后缀规范表)
- **大小写**：多个单词采用每个单词开头字母大写（大驼峰），例如 `NewListItem`；严格区分大小写，禁止同文件内多种命名风格并存

### 2.2.1 基本数据类型

#### 有符号整型

| 类型 | 类型别名 | 数据范围 | 前缀 | 指针前缀 |
|------|----------|----------|------|----------|
| signed char | int8_t | -128~127 | sc | psc |
| signed short | int16_t | -32768~32767 | ss | pss |
| signed int | int32_t | -2147483648~2147483647 | si | psi |
| signed long long | int64_t | -9223372036854775808~9223372036854775807 | sll | psll |

数组变量在名称后加后缀 `Array`。

```c
// 局部有符号8位整型
int8_t scNewListItem;
int8_t* pscNewListItem;
int8_t scNewListItemArray[];

// 全局有符号8位整型
int8_t g_scNewListItem;
int8_t* g_pscNewListItem;
int8_t g_scNewListItemArray[];

// 静态有符号8位整型
int8_t s_scNewListItem;
int8_t* s_pscNewListItem;
int8_t s_scNewListItemArray[];
```

#### 无符号整型

| 类型 | 类型别名 | 数据范围 | 前缀 | 指针前缀 |
|------|----------|----------|------|----------|
| unsigned char | uint8_t | 0~255 | uc | puc |
| unsigned short | uint16_t | 0~65535 | us | pus |
| unsigned int | uint32_t | 0~4294967295 | ul | pul |
| unsigned long long | uint64_t | 0~18446744073709551615 | ull | pull |

```c
uint8_t ucNewListItem;
uint8_t* pucNewListItem;
uint8_t g_ucNewListItem;
uint8_t s_ucNewListItem;
```

#### 浮点型

| 类型 | 前缀 | 指针前缀 |
|------|------|----------|
| float | f | pf |
| double | d | pd |

#### 布尔型

| 类型 | 前缀 | 指针前缀 |
|------|------|----------|
| bool | b | pb |

### 2.2.2 自定义数据类型

#### 结构体和共用体

- **类型名**：大驼峰命名，前缀 `x`，后缀 `_t`
- **标签名**（可选）：与类型名一致，有前缀无后缀

```c
typedef struct {
    uint8_t ucNewListItem;
} xNewListItem_t;

typedef union {
    uint8_t ucNewListItem;
} xNewListItem_t;
```

- 结构体变量前缀 `x`，指针前缀 `px`，数组后缀 `Array`

```c
xNewListItem_t xNewListItem;
xNewListItem_t* pxNewListItem;
xNewListItem_t g_xNewListItem;
xNewListItem_t s_xNewListItem;
```

#### 枚举类型

- **类型名**：大驼峰命名，前缀 `e`，后缀 `_e`
- **成员**：前缀 `e`，首个成员必须赋值

```c
typedef enum {
    eTaskRunning = 0,
    eTaskInvalid,
} eTaskState_e;

eTaskState_e eNewListItem;
eTaskState_e g_eNewListItem;
```

### 2.2.3 宏定义

#### 常量宏

格式：`模块名称_单个英文单词`，全大写，单词间用下划线分隔。

```c
// 正确
#define MCALUART_MAX_LENGTH     128
#define MCALUART_MIN_LENGTH     12
#define MCALUART_FRAME_LENGTH   50

// 错误
#define MAX_LENGTH 128          // 无模块名
#define mcal_max_length 128     // 未大写
#define MCALUARTMAXLENGTH 128   // 无下划线
```

#### 函数式宏

格式：`模块名称_单个英文单词(参数)`，参数须单独用小括号括起来。

```c
#define OTA_ROUND(x) ((x) + 0.5)
```

#### 功能宏

格式：无参数，全大写。

```c
#define LED_RED_ON() (GPIOB->ODR |= (1 << LED_RED_PIN))
```

#### 多行宏

使用 `do { ... } while(0)` 形式，换行加 `\`。

```c
#define SYS_CRITICAL_REGION_ENTER() do { \
    g_ucInterruptCnt++; \
    DISABLE_ALL_IRQ(); \
} while(0)
```

---

## 3. 函数定义规范

### 3.1 函数定义格式

`[存储类别] [函数返回值] [函数名]([函数参数])`

- 仅本文件使用的函数必须加 `static` 修饰
- 有返回值的函数优先使用预定义枚举类型返回值
- 函数参数遵循局部变量定义规范

### 3.2 函数命名

格式：`模块名_函数功能名`，大驼峰，模块名与功能名之间用下划线连接。

```c
// 正确
int8_t AppStim_ReportStimOff(uint8_t ucPara1);

// 错误
int8_t appstim_reportstimoff(uint8_t ucPara1);   // 无驼峰
int8_t appStim_Reportstimoff(uint8_t ucPara1);   // 大小写混用
int8_t AppStimReportStimOff(uint8_t ucPara1);    // 无下划线
int8_t AppStim_Report_StimOff(uint8_t ucPara1);  // 多用下划线
```

---

## 4. 源文件和头文件规范

### 4.1 文件命名

格式：`模块层级_功能描述.后缀`，全小写 + 下划线分隔。

| 正确示例 | 说明 |
|----------|------|
| `mcal_gpio.c/h` | 板级支持包 - GPIO |
| `driver_gx2431.c/h` | 驱动层 - GX2431 |
| `utils_ringbuffer.c/h` | 工具层 - 环形缓冲区 |
| `app_stim.c/h` | 应用层 - 电刺激 |

### 4.2 编码格式

统一使用 **UTF-8** 编码。

### 4.3 内容模板

#### C 源文件模板

```c
/*******************************************************************************
|  File Name:  app_template.c
|  Description:  Implementation of the application template
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
| 2024-08-23    01.00.00     XYJ       Creation
|******************************************************************************/

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "app_template.h"

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
```

#### C 头文件模板

```c
#ifndef _APP_TEMPLATE_H_
#define _APP_TEMPLATE_H_

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
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
}
#endif

#endif /* _APP_TEMPLATE_H_ */
```

---

## 5. 代码格式规范

### 5.1 函数格式

- 形参在同一行，逗号后加空格
- 第一个形参与左括号、最后一个形参与右括号之间各加空格
- 无形参时使用 `void`
- 返回类型与函数名之间保留一个空格，函数名与左括号之间无空格
- 左开括号 `{` 单独一行（函数体）

```c
void AppStim_UpDownJudgeTime1MsMainFunction( void )
{
    if ( g_xAppStimHandle.xWorkState == APPSTIM_UP_STATE ) {
        // ...
    }
}

void AppStim_TimelyStopSet( uint8_t Enable, uint16_t Time_xs )
{
    // ...
}
```

### 5.2 if / else 格式

- 开括号 `{` 放在条件语句同一行
- 4 个空格缩进
- 表达式括号前后有空格
- 多个表达式使用括号包裹

```c
if ( ( Set_Mode != APPSTIM_CURRMODE_SET ) && ( Set_Mode != APPSTIM_CURRMODE_ADJUST ) ) {
    return 0x1A;
}

if ( Curr_Temp < APPSTIM_CURR_MIN ) {
    return 0x1A;
} else if ( Curr_Temp >= APPSTIM_CURR_MAX ) {
    Curr_Temp = APPSTIM_CURR_MAX;
}
```

### 5.3 while / do-while 格式

- 4 个空格缩进
- 左开括号 `{` 与 `while`/`do` 在同一行

### 5.4 switch / case 格式

- 4 个空格缩进
- `case` 常量紧挨冒号，冒号前后有空格
- 左开括号 `{` 与 `case` 在同一行

```c
switch ( g_xAppStimHandle.xWorkState ) {
case APPSTIM_INIT_STATE: {
    g_xAppStimHandle.xWorkState = APPSTIM_WAIT_STIM_STATE;
    break;
}
case APPSTIM_WAIT_STIM_STATE: {
    // ...
    break;
}
}
```

### 5.5 for 格式

- 左开括号与 `for` 在同一行，右开括号单独一行
- 分号后有空格

```c
for ( uint8_t i = 0; i < 3; i++ ) {
    // ...
}
```

### 5.6 运算符格式

运算符前后保留 1 个空格：`&&`、`||`、`==`、`!=`、`=`、`? :` 等。

### 5.7 自定义类型格式

- 左开括号 `{` 与 `typedef` 在同一行，前面有空格
- 成员缩进 4 个空格

```c
typedef struct {
    uint16_t usFreq1;
    uint16_t usFreq2;
    uint32_t ulUpTime;
} xAppStim_ParaCfg_t;
```

### 5.8 变量初始化格式

- 修饰词、类型、变量名之间加空格
- 赋值等号 `=` 前后空格
- 指针星号 `*` 紧挨数据类型
- 指定成员初始化使用 designated initializer

```c
static xAppStim_ParaCfg_t g_xAppStimParaCfg = { 0 };
uint8_t g_ucTestArray[10] = { 1, 2, 34, 56 };
uint8_t* pucTest = NULL;
static xAppStim_TimerCfg_t g_xAppStimTimerCfg = {
    .ucEnable = 0,
    .ulSetTime = 60000,
    .ulCnt = 0,
};
```

### 5.9 注释格式

- 4 个空格缩进
- 在需要说明的内容上一行使用 `/* 注释内容 */`

```c
    /* 频率设置 */
    g_xAppStimParaCfg.usFreq1 = Freq_Temp[0];
```

### 5.10 VSCode 自动格式化

1. 安装 **clang-format** 扩展
2. 安装 **LLVM**（添加到环境变量）
3. 在项目根目录配置 `.clang-format`（见项目根目录）
4. 快捷键 `Shift+Alt+F` 格式化代码

---

## 6. 代码模块化与工程结构规范

### 6.1 模块化核心原则

- 高内聚、低耦合：一个模块只负责一个独立功能
- 模块之间通过头文件声明接口，源文件实现细节
- 禁止模块间直接访问内部变量 / 函数
- 按硬件驱动、业务功能、公共工具划分模块

### 6.2 工程框架（格式二）

本项目采用 **格式二** 工程框架，目录位于 `source/`：

```
source/
├── app/                           # 用户应用程序代码
│   ├── appstim/                   # 刺激应用模块
│   └── appemg/                    # 肌电应用模块
├── rte/                           # 运行时环境
├── bsw/                           # 基础软件服务
│   ├── systemservice/             # 系统服务（时钟/NVIC/看门狗/延时/初始化/日志/中间件）
│   ├── communicationservice/      # 通信服务（BLE/WiFi/CAN/UART/Ethernet）
│   ├── memoryservice/             # 存储服务（NAND/NOR/EEPROM/SD）
│   ├── complexdriver/             # 复杂设备驱动
│   ├── osservice/                 # 操作系统服务（FreeRTOS/RT-Thread）
│   └── utilsservice/              # 工具服务（FIFO/CRC）
├── mcal/                          # 微控制器抽象层
│   ├── src/                       # 外设驱动源文件
│   └── inc/                       # 外设驱动头文件
├── config/                        # 系统配置文件
├── task/                          # 系统任务管理
└── main.c                         # 入口文件
```

---

## 7. 附录

### 7.1 变量前/后缀规范表

#### 前缀规范表

| 前缀 | 含义 | 示例 |
|------|------|------|
| g_ | 全局变量 | `uint8_t g_ucIndex` |
| s_ | 静态变量 | `uint8_t s_ucIndex` |
| p | 指针类型 | `uint8_t* g_pucIndexPointer` |
| uc | 无符号8位 | `uint8_t ucIndex` |
| sc | 有符号8位 | `int8_t scIndex` |
| us | 无符号16位 | `uint16_t usIndex` |
| ss | 有符号16位 | `int16_t ssIndex` |
| ul | 无符号32位 | `uint32_t ulIndex` |
| si | 有符号32位 | `int32_t siIndex` |
| ull | 无符号64位 | `uint64_t ullIndex` |
| sll | 有符号64位 | `int64_t sllIndex` |
| b | 布尔类型 | `bool bIndexIsValid` |
| f | 单精度浮点 | `float fIndex` |
| d | 双精度浮点 | `double dIndex` |
| x | 结构体类型 | `xNewListItem_t xIndex` |
| px | 结构体指针 | `xNewListItem_t* pxIndex` |
| e | 枚举类型 | `eTaskState_e eIndex` |

#### 后缀规范表

| 后缀 | 含义 | 示例 |
|------|------|------|
| Array | 数组变量 | `uint8_t g_ucTXBuffArray[50]` |

### 7.2 代码格式化工具

- 使用 LLVM clang-format，配置文件见项目根目录 `.clang-format`
- 格式化快捷键：`Shift+Alt+F`

### 7.3 源文件头部注释工具

推荐使用 VSCode 插件 **koroFileHeader** 自动生成文件头注释。

- 默认头部注释快捷键：`Ctrl+Win+I`
- 默认函数注释快捷键：`Ctrl+Win+T`
