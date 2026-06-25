# Pelvic 嵌入式固件工程

本项目遵循 [嵌入式固件代码格式规范](docs/CODE_STYLE.md) 开发。

## 工程结构（格式二）

```
source/
├── app/                    # 用户应用程序
├── rte/                    # 运行时环境
├── bsw/                    # 基础软件服务
├── mcal/                   # 微控制器抽象层
├── config/                 # 系统配置
├── task/                   # 任务管理
└── main.c                  # 入口文件
```

## 代码格式化

- 安装 LLVM clang-format 与 VSCode clang-format 扩展
- 使用项目根目录 `.clang-format` 配置
- 快捷键 `Shift+Alt+F` 格式化当前文件

## 开发规范

详见 [docs/CODE_STYLE.md](docs/CODE_STYLE.md)
