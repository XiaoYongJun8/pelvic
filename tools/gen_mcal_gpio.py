#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate mcal_gpio enum and config table from mcal_gpio_ports.md."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MD_PATH = ROOT / "source" / "mcal" / "config" / "mcal_gpio_ports.md"
H_PATH = ROOT / "source" / "mcal" / "inc" / "mcal_gpio.h"
C_PATH = ROOT / "source" / "mcal" / "src" / "mcal_gpio.c"
ENUM_SNIPPET_PATH = ROOT / "tools" / "mcal_gpio_enum_snippet.txt"

ENUM_BEGIN = "/* GEN_MCAL_GPIO_ENUM_BEGIN */"
ENUM_END = "/* GEN_MCAL_GPIO_ENUM_END */"
TABLE_BEGIN = "/* GEN_MCAL_GPIO_TABLE_BEGIN */"
TABLE_END = "/* GEN_MCAL_GPIO_TABLE_END */"


def _split_row(line: str) -> list[str]:
    line = line.strip()
    if not line.startswith("|"):
        return []
    return [p.strip() for p in line.strip("|").split("|")]


def _is_separator(row: list[str]) -> bool:
    return all(re.fullmatch(r":?-+:?", cell.replace(" ", "")) or cell == "" for cell in row)


def parse_gpio_ports(md_text: str) -> list[dict[str, str]]:
    in_table = False
    headers: list[str] = []
    rows: list[dict[str, str]] = []

    for raw in md_text.splitlines():
        cells = _split_row(raw)
        if not cells:
            if in_table and rows:
                break
            continue

        if not in_table:
            if cells[0] == "启用" and "枚举名" in cells:
                in_table = True
                headers = cells
            continue

        if _is_separator(cells):
            continue

        item = {headers[i]: cells[i] if i < len(cells) else "" for i in range(len(headers))}
        enable = item.get("启用", "").lower()
        if enable in {"yes", "y", "1", "true", "是"}:
            rows.append(item)

    if not rows:
        raise SystemExit("错误：mcal_gpio_ports.md 中未找到启用的 GPIO 端口行。")

    required = ["枚举名", "端口", "引脚", "模式", "速度", "默认电平", "复用"]
    for idx, row in enumerate(rows, start=1):
        for key in required:
            if not row.get(key, "").strip():
                raise SystemExit(f"错误：第 {idx} 行缺少字段「{key}」。")

    return rows


def gen_enum(rows: list[dict[str, str]]) -> str:
    lines = ["typedef enum {"]
    for i, row in enumerate(rows):
        suffix = " = 0," if i == 0 else ","
        lines.append(f"    {row['枚举名']}{suffix}")
    lines.append("    eMcal_GpioMaxNum,")
    lines.append("} eMcal_GpioPin_e;")
    return "\n".join(lines)


def gen_table(rows: list[dict[str, str]]) -> str:
    lines = ["static xMcal_GpioPinCfg_t s_xMcal_GpioConfigTable[eMcal_GpioMaxNum] = {"]
    for row in rows:
        lines.append(
            "    { "
            f"{row['端口']}, {row['引脚']}, {row['模式']}, {row['速度']}, "
            f"{row['默认电平']}, {row['复用']} }},"
        )
    lines.append("};")
    return "\n".join(lines)


def replace_block(text: str, begin: str, end: str, content: str) -> str:
    pattern = re.compile(re.escape(begin) + r".*?" + re.escape(end), re.DOTALL)
    replacement = f"{begin}\n{content}\n{end}"
    if not pattern.search(text):
        raise SystemExit(f"错误：未找到标记 {begin} ... {end}")
    return pattern.sub(replacement, text, count=1)


def main() -> int:
    if not MD_PATH.is_file():
        print(f"错误：找不到配置文件 {MD_PATH}", file=sys.stderr)
        return 1

    rows = parse_gpio_ports(MD_PATH.read_text(encoding="utf-8"))
    enum_block = gen_enum(rows)
    table_block = gen_table(rows)

    c_text = replace_block(C_PATH.read_text(encoding="utf-8"), TABLE_BEGIN, TABLE_END, table_block)
    C_PATH.write_text(c_text, encoding="utf-8", newline="\n")

    enum_snippet = f"{ENUM_BEGIN}\n{enum_block}\n{ENUM_END}\n"
    ENUM_SNIPPET_PATH.write_text(enum_snippet, encoding="utf-8", newline="\n")

    names = ", ".join(r["枚举名"] for r in rows)
    print(f"已生成 {len(rows)} 个 GPIO 端口：{names}")
    print(f"已更新 {C_PATH.relative_to(ROOT)}")
    print(
        "注意：mcal_gpio.h 枚举区块请手动粘贴（Esafenet 会加密 Python 写入的 .h 文件）："
    )
    print(f"  {ENUM_SNIPPET_PATH.relative_to(ROOT)}")
    if H_PATH.is_file():
        try:
            h_head = H_PATH.read_bytes()[:3]
        except OSError:
            h_head = b""
        if h_head != b"/**":
            print(f"警告：{H_PATH.relative_to(ROOT)} 可能已损坏，请用 IDE 还原 GEN 区块。")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
