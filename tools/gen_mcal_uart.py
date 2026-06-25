#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate mcal_uart config table from mcal_uart_ports.md."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MD_PATH = ROOT / "source" / "mcal" / "config" / "mcal_uart_ports.md"
H_PATH = ROOT / "source" / "mcal" / "inc" / "mcal_uart.h"
C_PATH = ROOT / "source" / "mcal" / "src" / "mcal_uart.c"
ENUM_SNIPPET_PATH = ROOT / "tools" / "mcal_uart_enum_snippet.txt"

ENUM_BEGIN = "/* GEN_MCAL_UART_ENUM_BEGIN */"
ENUM_END = "/* GEN_MCAL_UART_ENUM_END */"
RXBUF_BEGIN = "/* GEN_MCAL_UART_RXBUF_BEGIN */"
RXBUF_END = "/* GEN_MCAL_UART_RXBUF_END */"
TABLE_BEGIN = "/* GEN_MCAL_UART_TABLE_BEGIN */"
TABLE_END = "/* GEN_MCAL_UART_TABLE_END */"
IRQ_BEGIN = "/* GEN_MCAL_UART_IRQ_BEGIN */"
IRQ_END = "/* GEN_MCAL_UART_IRQ_END */"

REQUIRED = [
    "枚举名",
    "USART",
    "波特率",
    "数据位",
    "停止位",
    "校验",
    "DMA发送",
    "DMA通道",
    "TX端口",
    "TX引脚",
    "RX端口",
    "RX引脚",
    "复用",
    "RX缓冲长度",
]


def _split_row(line: str) -> list[str]:
    line = line.strip()
    if not line.startswith("|"):
        return []
    return [p.strip() for p in line.strip("|").split("|")]


def _is_separator(row: list[str]) -> bool:
    return all(re.fullmatch(r":?-+:?", cell.replace(" ", "")) or cell == "" for cell in row)


def _buf_symbol(enum_name: str) -> str:
    name = enum_name[1:] if enum_name.startswith("e") else enum_name
    return f"s_ucMcalUartRxBuf_{name}"


def _rx_state_symbol(enum_name: str) -> str:
    name = enum_name[1:] if enum_name.startswith("e") else enum_name
    return f"s_xMcalUartRxState_{name}"


def parse_uart_ports(md_text: str) -> list[dict[str, str]]:
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
        raise SystemExit("错误：mcal_uart_ports.md 中未找到启用的 UART 端口行。")

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
    lines.append("    eMcal_UartMaxNum,")
    lines.append("} eMcal_UartCom_e;")
    return "\n".join(lines)


def gen_rx_buffers(rows: list[dict[str, str]]) -> str:
    lines: list[str] = []
    for row in rows:
        symbol = _buf_symbol(row["枚举名"])
        state = _rx_state_symbol(row["枚举名"])
        size = row["RX缓冲长度"]
        lines.append(f"static xMcal_UartRxState_t {state} = {{ 0U, 0U }};")
        lines.append(f"static uint8_t {symbol}[{size}];")
    return "\n".join(lines)


def _dma_flag(row: dict[str, str]) -> str:
    return "1U" if row["DMA发送"].lower() in {"yes", "y", "1", "true", "是"} else "0U"


def gen_table(rows: list[dict[str, str]]) -> str:
    lines = ["static xMcal_UartCfg_t s_xMcal_UartConfigTable[eMcal_UartMaxNum] = {"]
    for row in rows:
        buf = _buf_symbol(row["枚举名"])
        state = _rx_state_symbol(row["枚举名"])
        lines.append("    {")
        lines.append(f"        {row['USART']},")
        lines.append(f"        {row['波特率']}U,")
        lines.append(f"        {row['数据位']},")
        lines.append(f"        {row['停止位']},")
        lines.append(f"        {row['校验']},")
        lines.append(f"        {_dma_flag(row)},")
        lines.append(f"        {row['DMA通道']},")
        lines.append(f"        {row['TX端口']},")
        lines.append(f"        {row['TX引脚']},")
        lines.append(f"        {row['RX端口']},")
        lines.append(f"        {row['RX引脚']},")
        lines.append(f"        {row['复用']},")
        lines.append(f"        {buf},")
        lines.append(f"        {row['RX缓冲长度']}U,")
        lines.append(f"        &{state},")
        lines.append("    },")
    lines.append("};")
    return "\n".join(lines)


def _irq_handler_name(usart: str) -> str:
    mapping = {
        "USART0": "USART0_IRQHandler",
        "USART1": "USART1_IRQHandler",
        "USART2": "USART2_IRQHandler",
    }
    if usart not in mapping:
        raise SystemExit(f"错误：不支持的 USART「{usart}」。")
    return mapping[usart]


def gen_irq_handlers(rows: list[dict[str, str]]) -> str:
    lines: list[str] = []
    used: set[str] = set()
    for row in rows:
        handler = _irq_handler_name(row["USART"])
        if handler in used:
            continue
        used.add(handler)
        lines.append("/*******************************************************************************")
        lines.append(f"Name            : {handler}")
        lines.append("Parameters(in)  : None")
        lines.append("Parameters(out) : None")
        lines.append("Return value    : None")
        lines.append("Description     : USART receive interrupt service routine")
        lines.append("Call By         : NVIC")
        lines.append("|******************************************************************************/")
        lines.append(f"void {handler}( void )")
        lines.append("{")
        lines.append(f"    Mcal_UartIrqHandler( {row['USART']} );")
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
        print(f"错误：找不到 {C_PATH}，请先创建 mcal_uart.c 模板。", file=sys.stderr)
        return 1

    rows = parse_uart_ports(MD_PATH.read_text(encoding="utf-8"))
    c_text = C_PATH.read_text(encoding="utf-8")
    c_text = replace_block(c_text, RXBUF_BEGIN, RXBUF_END, gen_rx_buffers(rows))
    c_text = replace_block(c_text, TABLE_BEGIN, TABLE_END, gen_table(rows))
    if IRQ_BEGIN in c_text and IRQ_END in c_text:
        c_text = replace_block(c_text, IRQ_BEGIN, IRQ_END, gen_irq_handlers(rows))
    C_PATH.write_text(c_text, encoding="utf-8", newline="\n")

    enum_snippet = f"{ENUM_BEGIN}\n{gen_enum(rows)}\n{ENUM_END}\n"
    ENUM_SNIPPET_PATH.write_text(enum_snippet, encoding="utf-8", newline="\n")

    names = ", ".join(r["枚举名"] for r in rows)
    print(f"已生成 {len(rows)} 个 UART 端口：{names}")
    print(f"已更新 {C_PATH.relative_to(ROOT)}")
    print("注意：mcal_uart.h 枚举区块请手动粘贴（Esafenet 会加密 Python 写入的 .h 文件）：")
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
