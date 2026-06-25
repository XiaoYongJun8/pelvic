#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate mcal_pwm config table from mcal_pwm_channels.md."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MD_PATH = ROOT / "source" / "mcal" / "config" / "mcal_pwm_channels.md"
H_PATH = ROOT / "source" / "mcal" / "inc" / "mcal_pwm.h"
C_PATH = ROOT / "source" / "mcal" / "src" / "mcal_pwm.c"
ENUM_SNIPPET_PATH = ROOT / "tools" / "mcal_pwm_enum_snippet.txt"

ENUM_BEGIN = "/* GEN_MCAL_PWM_ENUM_BEGIN */"
ENUM_END = "/* GEN_MCAL_PWM_ENUM_END */"
CLK_BEGIN = "/* GEN_MCAL_PWM_CLK_BEGIN */"
CLK_END = "/* GEN_MCAL_PWM_CLK_END */"
TABLE_BEGIN = "/* GEN_MCAL_PWM_TABLE_BEGIN */"
TABLE_END = "/* GEN_MCAL_PWM_TABLE_END */"
DEFAULT_BEGIN = "/* GEN_MCAL_PWM_DEFAULT_BEGIN */"
DEFAULT_END = "/* GEN_MCAL_PWM_DEFAULT_END */"

REQUIRED = [
    "枚举名",
    "定时器",
    "通道",
    "预分频",
    "周期",
    "对齐模式",
    "计数方向",
    "时钟分频",
    "默认启动",
    "默认频率Hz",
    "默认脉宽us",
]


def _split_row(line: str) -> list[str]:
    line = line.strip()
    if not line.startswith("|"):
        return []
    return [p.strip() for p in line.strip("|").split("|")]


def _is_separator(row: list[str]) -> bool:
    return all(re.fullmatch(r":?-+:?", cell.replace(" ", "")) or cell == "" for cell in row)


def _parse_table(md_text: str, header_first: str) -> tuple[list[str], list[dict[str, str]]]:
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
            if cells[0] == header_first:
                in_table = True
                headers = cells
            continue

        if _is_separator(cells):
            continue

        item = {headers[i]: cells[i] if i < len(cells) else "" for i in range(len(headers))}
        rows.append(item)

    return headers, rows


def _auto_start_flag(value: str) -> str:
    return "1U" if value.lower() in {"yes", "y", "1", "true", "是"} else "0U"


def parse_pwm_clock(md_text: str) -> str:
    _, rows = _parse_table(md_text, "定时器时钟Hz")
    if not rows or not rows[0].get("定时器时钟Hz", "").strip():
        raise SystemExit("错误：mcal_pwm_channels.md 中未找到 PWM 全局参数表。")
    return f"{rows[0]['定时器时钟Hz']}U"


def parse_pwm_channels(md_text: str) -> list[dict[str, str]]:
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
        raise SystemExit("错误：mcal_pwm_channels.md 中未找到启用的 PWM 通道行。")

    for idx, row in enumerate(rows, start=1):
        for key in REQUIRED:
            if not row.get(key, "").strip():
                raise SystemExit(f"错误：第 {idx} 行缺少字段「{key}」。")
    return rows


def gen_enum(rows: list[dict[str, str]]) -> str:
    lines = ["typedef enum {"]
    for i, row in enumerate(rows):
        suffix = " = 0," if i == 0 else ","
        lines.append(f"    {row['枚举名']}{suffix}")
    lines.append("    eMcal_PwmMaxNum,")
    lines.append("} eMcal_Pwm_e;")
    return "\n".join(lines)


def gen_table(rows: list[dict[str, str]]) -> str:
    lines = ["static xMcal_PwmCfg_t s_xMcal_PwmConfigTable[eMcal_PwmMaxNum] = {"]
    for row in rows:
        lines.append("    {")
        lines.append(f"        {row['定时器']},")
        lines.append(f"        {row['通道']},")
        lines.append(f"        {row['预分频']}U,")
        lines.append(f"        {row['周期']}U,")
        lines.append(f"        {row['对齐模式']},")
        lines.append(f"        {row['计数方向']},")
        lines.append(f"        {row['时钟分频']},")
        lines.append(f"        0U,")
        lines.append(f"        {_auto_start_flag(row['默认启动'])},")
        lines.append("    },")
    lines.append("};")
    return "\n".join(lines)


def gen_defaults(rows: list[dict[str, str]]) -> str:
    freq = ", ".join(f"{row['默认频率Hz']}U" for row in rows)
    pw = ", ".join(f"{row['默认脉宽us']}U" for row in rows)
    return "\n".join(
        [
            f"static const uint16_t s_usMcalPwmDefaultFreq[eMcal_PwmMaxNum] = {{ {freq} }};",
            f"static const uint32_t s_ulMcalPwmDefaultPwUs[eMcal_PwmMaxNum] = {{ {pw} }};",
        ]
    )


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

    if not C_PATH.is_file():
        print(f"错误：找不到 {C_PATH}，请先创建 mcal_pwm.c 模板。", file=sys.stderr)
        return 1

    md_text = MD_PATH.read_text(encoding="utf-8")
    rows = parse_pwm_channels(md_text)
    clk = parse_pwm_clock(md_text)

    c_text = C_PATH.read_text(encoding="utf-8")
    c_text = replace_block(c_text, CLK_BEGIN, CLK_END, f"static const uint32_t s_ulMcalPwmTimerClkHz = {clk};")
    c_text = replace_block(c_text, TABLE_BEGIN, TABLE_END, gen_table(rows))
    c_text = replace_block(c_text, DEFAULT_BEGIN, DEFAULT_END, gen_defaults(rows))
    C_PATH.write_text(c_text, encoding="utf-8", newline="\n")

    enum_snippet = f"{ENUM_BEGIN}\n{gen_enum(rows)}\n{ENUM_END}\n"
    ENUM_SNIPPET_PATH.write_text(enum_snippet, encoding="utf-8", newline="\n")

    names = ", ".join(r["枚举名"] for r in rows)
    print(f"已生成 {len(rows)} 个 PWM 通道：{names}")
    print(f"已更新 {C_PATH.relative_to(ROOT)}")
    print("注意：mcal_pwm.h 枚举区块请手动粘贴（Esafenet 会加密 Python 写入的 .h 文件）：")
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
