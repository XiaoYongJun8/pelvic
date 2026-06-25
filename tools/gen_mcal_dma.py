#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate mcal_dma buffers and config table from mcal_dma_channels.md."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MD_PATH = ROOT / "source" / "mcal" / "config" / "mcal_dma_channels.md"
H_PATH = ROOT / "source" / "mcal" / "inc" / "mcal_dma.h"
C_PATH = ROOT / "source" / "mcal" / "src" / "mcal_dma.c"
ENUM_SNIPPET_PATH = ROOT / "tools" / "mcal_dma_enum_snippet.txt"

ENUM_BEGIN = "/* GEN_MCAL_DMA_ENUM_BEGIN */"
ENUM_END = "/* GEN_MCAL_DMA_ENUM_END */"
BUFFERS_BEGIN = "/* GEN_MCAL_DMA_BUFFERS_BEGIN */"
BUFFERS_END = "/* GEN_MCAL_DMA_BUFFERS_END */"
TABLE_BEGIN = "/* GEN_MCAL_DMA_TABLE_BEGIN */"
TABLE_END = "/* GEN_MCAL_DMA_TABLE_END */"

REQUIRED = [
    "枚举名",
    "DMA端口",
    "通道",
    "循环",
    "方向",
    "外设宽度",
    "内存宽度",
    "传输数量",
    "优先级",
    "外设增量",
    "内存增量",
    "外设地址",
    "缓冲类型",
    "缓冲长度",
    "USART外设",
]


def _split_row(line: str) -> list[str]:
    line = line.strip()
    if not line.startswith("|"):
        return []
    return [p.strip() for p in line.strip("|").split("|")]


def _is_separator(row: list[str]) -> bool:
    return all(re.fullmatch(r":?-+:?", cell.replace(" ", "")) or cell == "" for cell in row)


def _buffer_symbol(enum_name: str) -> str:
    name = enum_name
    if name.startswith("e"):
        name = name[1:]
    return f"s_aMcalDmaBuf_{name}"


def parse_dma_channels(md_text: str) -> list[dict[str, str]]:
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
        raise SystemExit("错误：mcal_dma_channels.md 中未找到启用的 DMA 通道行。")

    for idx, row in enumerate(rows, start=1):
        for key in REQUIRED:
            if not row.get(key, "").strip():
                raise SystemExit(f"错误：第 {idx} 行缺少字段「{key}」。")
        buf_type = row["缓冲类型"]
        if buf_type not in {"uint16_t", "uint8_t"}:
            raise SystemExit(f"错误：第 {idx} 行缓冲类型仅支持 uint16_t / uint8_t。")

    return rows


def gen_enum(rows: list[dict[str, str]]) -> str:
    lines = ["typedef enum {"]
    for i, row in enumerate(rows):
        suffix = " = 0," if i == 0 else ","
        lines.append(f"    {row['枚举名']}{suffix}")
    lines.append("    eMcal_DmaMaxNum,")
    lines.append("} eMcal_DmaCh_e;")
    return "\n".join(lines)


def gen_buffers(rows: list[dict[str, str]]) -> str:
    lines: list[str] = []
    for row in rows:
        symbol = _buffer_symbol(row["枚举名"])
        buf_type = row["缓冲类型"]
        buf_len = row["缓冲长度"]
        lines.append(f"static {buf_type} {symbol}[{buf_len}] = {{ 0 }};")
    return "\n".join(lines)


def gen_table(rows: list[dict[str, str]]) -> str:
    lines = ["static xMcal_DmaChCfg_t s_xMcal_DmaConfigTable[eMcal_DmaMaxNum] = {"]
    for row in rows:
        symbol = _buffer_symbol(row["枚举名"])
        circ = "1U" if row["循环"].strip() in {"1", "yes", "true"} else "0U"
        lines.append("    {")
        lines.append(f"        {circ},")
        lines.append(f"        {row['DMA端口']},")
        lines.append(f"        {row['通道']},")
        lines.append(f"        {row['外设地址']},")
        lines.append(f"        {row['外设宽度']},")
        lines.append(f"        {row['内存宽度']},")
        lines.append(f"        {row['传输数量']}U,")
        lines.append(f"        {row['优先级']},")
        lines.append(f"        {row['外设增量']},")
        lines.append(f"        {row['内存增量']},")
        lines.append(f"        {row['方向']},")
        lines.append(f"        {symbol},")
        lines.append(f"        sizeof( {symbol} ),")
        usart = row.get("USART外设", "0").strip() or "0"
        lines.append(f"        {usart},")
        lines.append("    },")
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

    if not C_PATH.is_file():
        print(f"错误：找不到 {C_PATH}，请先创建 mcal_dma.c 模板。", file=sys.stderr)
        return 1

    rows = parse_dma_channels(MD_PATH.read_text(encoding="utf-8"))
    enum_block = gen_enum(rows)
    buffers_block = gen_buffers(rows)
    table_block = gen_table(rows)

    c_text = C_PATH.read_text(encoding="utf-8")
    c_text = replace_block(c_text, BUFFERS_BEGIN, BUFFERS_END, buffers_block)
    c_text = replace_block(c_text, TABLE_BEGIN, TABLE_END, table_block)
    C_PATH.write_text(c_text, encoding="utf-8", newline="\n")

    enum_snippet = f"{ENUM_BEGIN}\n{enum_block}\n{ENUM_END}\n"
    ENUM_SNIPPET_PATH.write_text(enum_snippet, encoding="utf-8", newline="\n")

    names = ", ".join(r["枚举名"] for r in rows)
    print(f"已生成 {len(rows)} 个 DMA 通道：{names}")
    print(f"已更新 {C_PATH.relative_to(ROOT)}")
    print("注意：mcal_dma.h 枚举区块请手动粘贴（Esafenet 会加密 Python 写入的 .h 文件）：")
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
