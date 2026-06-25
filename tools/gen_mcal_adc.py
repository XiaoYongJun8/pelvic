#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate mcal_adc channel tables from mcal_adc_channels.md."""

from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MD_PATH = ROOT / "source" / "mcal" / "config" / "mcal_adc_channels.md"
DMA_MD_PATH = ROOT / "source" / "mcal" / "config" / "mcal_dma_channels.md"
H_PATH = ROOT / "source" / "mcal" / "inc" / "mcal_adc.h"
C_PATH = ROOT / "source" / "mcal" / "src" / "mcal_adc.c"
ENUM_SNIPPET_PATH = ROOT / "tools" / "mcal_adc_enum_snippet.txt"
GEN_MCAL_DMA = ROOT / "tools" / "gen_mcal_dma.py"

ENUM_BEGIN = "/* GEN_MCAL_ADC_ENUM_BEGIN */"
ENUM_END = "/* GEN_MCAL_ADC_ENUM_END */"
INST_BEGIN = "/* GEN_MCAL_ADC_INST_BEGIN */"
INST_END = "/* GEN_MCAL_ADC_INST_END */"
CHAN_BEGIN = "/* GEN_MCAL_ADC_CHAN_BEGIN */"
CHAN_END = "/* GEN_MCAL_ADC_CHAN_END */"
SAMPLE_BEGIN = "/* GEN_MCAL_ADC_SAMPLE_BEGIN */"
SAMPLE_END = "/* GEN_MCAL_ADC_SAMPLE_END */"

REQUIRED = ["枚举名", "硬件通道", "采样时间"]
INST_KEYS = ["ADC端口", "工作模式", "分辨率", "扫描模式", "连续模式", "数据对齐"]


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


def parse_adc_instance(md_text: str) -> dict[str, str]:
    _, rows = _parse_table(md_text, "ADC端口")
    if not rows:
        raise SystemExit("错误：mcal_adc_channels.md 中未找到 ADC 实例参数表。")
    inst = rows[0]
    for key in INST_KEYS:
        if not inst.get(key, "").strip():
            raise SystemExit(f"错误：ADC 实例参数缺少字段「{key}」。")
    return inst


def parse_adc_channels(md_text: str) -> list[dict[str, str]]:
    _, rows = _parse_table(md_text, "启用")
    channels: list[dict[str, str]] = []
    for item in rows:
        if "枚举名" not in item:
            continue
        enable = item.get("启用", "").lower()
        if enable in {"yes", "y", "1", "true", "是"}:
            channels.append(item)

    if not channels:
        raise SystemExit("错误：mcal_adc_channels.md 中未找到启用的 ADC 通道行。")

    for idx, row in enumerate(channels, start=1):
        for key in REQUIRED:
            if not row.get(key, "").strip():
                raise SystemExit(f"错误：第 {idx} 行缺少字段「{key}」。")
    return channels


def gen_enum(rows: list[dict[str, str]]) -> str:
    lines = ["typedef enum {"]
    for i, row in enumerate(rows):
        suffix = " = 0," if i == 0 else ","
        lines.append(f"    {row['枚举名']}{suffix}")
    lines.append("    eMcal_AdcMaxNum,")
    lines.append("} eMcal_AdcCh_e;")
    return "\n".join(lines)


def gen_inst(inst: dict[str, str]) -> str:
    return "\n".join(
        [
            "static const xMcal_AdcInstCfg_t s_xMcal_AdcInstCfg = {",
            f"    {inst['ADC端口']},",
            f"    {inst['工作模式']},",
            f"    {inst['分辨率']},",
            f"    {inst['扫描模式']},",
            f"    {inst['连续模式']},",
            f"    {inst['数据对齐']},",
            "};",
        ]
    )


def gen_chan_buffer(rows: list[dict[str, str]]) -> str:
    values = ", ".join(row["硬件通道"] for row in rows)
    return f"static const uint8_t s_ucMcalAdcChanBuffer[eMcal_AdcMaxNum] = {{ {values} }};"


def gen_sample_buffer(rows: list[dict[str, str]]) -> str:
    values = ", ".join(row["采样时间"] for row in rows)
    return f"static const uint32_t s_ulMcalAdcSampleTime[eMcal_AdcMaxNum] = {{ {values} }};"


def replace_block(text: str, begin: str, end: str, content: str) -> str:
    pattern = re.compile(re.escape(begin) + r".*?" + re.escape(end), re.DOTALL)
    replacement = f"{begin}\n{content}\n{end}"
    if not pattern.search(text):
        raise SystemExit(f"错误：未找到标记 {begin} ... {end}")
    return pattern.sub(replacement, text, count=1)


def sync_dma_adc_buffer(channel_count: int) -> None:
    if not DMA_MD_PATH.is_file():
        print(f"警告：未找到 {DMA_MD_PATH}，跳过 DMA 缓冲同步。", file=sys.stderr)
        return

    text = DMA_MD_PATH.read_text(encoding="utf-8")
    lines = text.splitlines()
    out: list[str] = []
    in_table = False
    headers: list[str] = []

    for line in lines:
        cells = _split_row(line)
        if cells and cells[0] == "启用" and "枚举名" in cells:
            in_table = True
            headers = cells
            out.append(line)
            continue

        if in_table and cells and _is_separator(cells):
            out.append(line)
            continue

        if in_table and cells and "枚举名" in headers:
            item = {headers[i]: cells[i] if i < len(cells) else "" for i in range(len(headers))}
            if item.get("枚举名") == "eMcal_DmaAdc0":
                if "传输数量" in headers:
                    item["传输数量"] = str(channel_count)
                if "缓冲长度" in headers:
                    item["缓冲长度"] = str(channel_count)
                new_cells = [item.get(h, "") for h in headers]
                out.append("| " + " | ".join(new_cells) + " |")
                continue

        if in_table and not cells and out and _split_row(out[-1]):
            in_table = False

        out.append(line)

    DMA_MD_PATH.write_text("\n".join(out) + "\n", encoding="utf-8", newline="\n")


def main() -> int:
    if not MD_PATH.is_file():
        print(f"错误：找不到配置文件 {MD_PATH}", file=sys.stderr)
        return 1

    if not C_PATH.is_file():
        print(f"错误：找不到 {C_PATH}，请先创建 mcal_adc.c 模板。", file=sys.stderr)
        return 1

    md_text = MD_PATH.read_text(encoding="utf-8")
    inst = parse_adc_instance(md_text)
    rows = parse_adc_channels(md_text)

    c_text = C_PATH.read_text(encoding="utf-8")
    c_text = replace_block(c_text, INST_BEGIN, INST_END, gen_inst(inst))
    c_text = replace_block(c_text, CHAN_BEGIN, CHAN_END, gen_chan_buffer(rows))
    c_text = replace_block(c_text, SAMPLE_BEGIN, SAMPLE_END, gen_sample_buffer(rows))
    C_PATH.write_text(c_text, encoding="utf-8", newline="\n")

    enum_snippet = f"{ENUM_BEGIN}\n{gen_enum(rows)}\n{ENUM_END}\n"
    ENUM_SNIPPET_PATH.write_text(enum_snippet, encoding="utf-8", newline="\n")

    sync_dma_adc_buffer(len(rows))
    if GEN_MCAL_DMA.is_file():
        subprocess.run([sys.executable, str(GEN_MCAL_DMA)], check=False)

    names = ", ".join(r["枚举名"] for r in rows)
    print(f"已生成 {len(rows)} 个 ADC 通道：{names}")
    print(f"已更新 {C_PATH.relative_to(ROOT)}")
    print(f"已同步 eMcal_DmaAdc0 缓冲长度为 {len(rows)}")
    print("注意：mcal_adc.h 枚举区块请手动粘贴（Esafenet 会加密 Python 写入的 .h 文件）：")
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
