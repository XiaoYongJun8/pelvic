#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate mcal_timer config table and IRQ handlers from mcal_timer_channels.md."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MD_PATH = ROOT / "source" / "mcal" / "config" / "mcal_timer_channels.md"
H_PATH = ROOT / "source" / "mcal" / "inc" / "mcal_timer.h"
C_PATH = ROOT / "source" / "mcal" / "src" / "mcal_timer.c"
ENUM_SNIPPET_PATH = ROOT / "tools" / "mcal_timer_enum_snippet.txt"

ENUM_BEGIN = "/* GEN_MCAL_TIMER_ENUM_BEGIN */"
ENUM_END = "/* GEN_MCAL_TIMER_ENUM_END */"
TABLE_BEGIN = "/* GEN_MCAL_TIMER_TABLE_BEGIN */"
TABLE_END = "/* GEN_MCAL_TIMER_TABLE_END */"
IRQ_BEGIN = "/* GEN_MCAL_TIMER_IRQ_BEGIN */"
IRQ_END = "/* GEN_MCAL_TIMER_IRQ_END */"

REQUIRED = [
    "枚举名",
    "定时器",
    "预分频",
    "周期",
    "对齐模式",
    "计数方向",
    "时钟分频",
    "重复计数",
    "默认启动",
]

IRQ_MAP = {
    "TIMER0": "TIMER0_BRK_TIMER8_IRQHandler",
    "TIMER1": "TIMER1_IRQHandler",
    "TIMER2": "TIMER2_IRQHandler",
    "TIMER3": "TIMER3_IRQHandler",
    "TIMER4": "TIMER4_IRQHandler",
    "TIMER5": "TIMER5_IRQHandler",
    "TIMER6": "TIMER6_IRQHandler",
}


def _split_row(line: str) -> list[str]:
    line = line.strip()
    if not line.startswith("|"):
        return []
    return [p.strip() for p in line.strip("|").split("|")]


def _is_separator(row: list[str]) -> bool:
    return all(re.fullmatch(r":?-+:?", cell.replace(" ", "")) or cell == "" for cell in row)


def _auto_start_flag(value: str) -> str:
    return "1U" if value.lower() in {"yes", "y", "1", "true", "是"} else "0U"


def parse_timer_channels(md_text: str) -> list[dict[str, str]]:
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
        raise SystemExit("错误：mcal_timer_channels.md 中未找到启用的 Timer 行。")

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
    lines.append("    eMcal_TimerMaxNum,")
    lines.append("} eMcal_Timer_e;")
    return "\n".join(lines)


def gen_table(rows: list[dict[str, str]]) -> str:
    lines = ["static xMcal_TimerCfg_t s_xMcal_TimerConfigTable[eMcal_TimerMaxNum] = {"]
    for row in rows:
        lines.append("    {")
        lines.append(f"        {row['定时器']},")
        lines.append(f"        {row['预分频']}U,")
        lines.append(f"        {row['周期']}U,")
        lines.append(f"        {row['对齐模式']},")
        lines.append(f"        {row['计数方向']},")
        lines.append(f"        {row['时钟分频']},")
        lines.append(f"        {row['重复计数']}U,")
        lines.append(f"        {_auto_start_flag(row['默认启动'])},")
        lines.append("        NULL,")
        lines.append("    },")
    lines.append("};")
    return "\n".join(lines)


def gen_irq_handlers(rows: list[dict[str, str]]) -> str:
    lines: list[str] = []
    used: set[str] = set()
    for row in rows:
        timer = row["定时器"]
        handler = IRQ_MAP.get(timer)
        if handler is None:
            raise SystemExit(f"错误：未定义定时器「{timer}」的中断函数映射。")
        if handler in used:
            continue
        used.add(handler)
        lines.append("/*******************************************************************************")
        lines.append(f"Name            : {handler}")
        lines.append("Parameters(in)  : None")
        lines.append("Parameters(out) : None")
        lines.append("Return value    : None")
        lines.append("Description     : Timer update interrupt service routine")
        lines.append("Call By         : NVIC")
        lines.append("|******************************************************************************/")
        lines.append(f"void {handler}( void )")
        lines.append("{")
        lines.append(f"    Mcal_TimerIrqHandler( {timer} );")
        lines.append("}")
        lines.append("")
    return "\n".join(lines).rstrip()


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
        print(f"错误：找不到 {C_PATH}，请先创建 mcal_timer.c 模板。", file=sys.stderr)
        return 1

    rows = parse_timer_channels(MD_PATH.read_text(encoding="utf-8"))
    c_text = C_PATH.read_text(encoding="utf-8")
    c_text = replace_block(c_text, TABLE_BEGIN, TABLE_END, gen_table(rows))
    if IRQ_BEGIN in c_text and IRQ_END in c_text:
        c_text = replace_block(c_text, IRQ_BEGIN, IRQ_END, gen_irq_handlers(rows))
    C_PATH.write_text(c_text, encoding="utf-8", newline="\n")

    enum_snippet = f"{ENUM_BEGIN}\n{gen_enum(rows)}\n{ENUM_END}\n"
    ENUM_SNIPPET_PATH.write_text(enum_snippet, encoding="utf-8", newline="\n")

    names = ", ".join(r["枚举名"] for r in rows)
    print(f"已生成 {len(rows)} 个 Timer：{names}")
    print(f"已更新 {C_PATH.relative_to(ROOT)}")
    print("注意：mcal_timer.h 枚举区块请手动粘贴（Esafenet 会加密 Python 写入的 .h 文件）：")
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
