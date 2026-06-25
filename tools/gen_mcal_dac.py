#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate mcal_dac config tables from mcal_dac_channels.md."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MD_PATH = ROOT / "source" / "mcal" / "config" / "mcal_dac_channels.md"
H_PATH = ROOT / "source" / "mcal" / "inc" / "mcal_dac.h"
C_PATH = ROOT / "source" / "mcal" / "src" / "mcal_dac.c"
ENUM_SNIPPET_PATH = ROOT / "tools" / "mcal_dac_enum_snippet.txt"

ENUM_BEGIN = "/* GEN_MCAL_DAC_ENUM_BEGIN */"
ENUM_END = "/* GEN_MCAL_DAC_ENUM_END */"
INST_BEGIN = "/* GEN_MCAL_DAC_INST_BEGIN */"
INST_END = "/* GEN_MCAL_DAC_INST_END */"
OUT_CHAN_BEGIN = "/* GEN_MCAL_DAC_OUT_CHAN_BEGIN */"
OUT_CHAN_END = "/* GEN_MCAL_DAC_OUT_CHAN_END */"
OUT_BUF_BEGIN = "/* GEN_MCAL_DAC_OUT_BUF_BEGIN */"
OUT_BUF_END = "/* GEN_MCAL_DAC_OUT_BUF_END */"
INIT_VAL_BEGIN = "/* GEN_MCAL_DAC_INIT_VAL_BEGIN */"
INIT_VAL_END = "/* GEN_MCAL_DAC_INIT_VAL_END */"

REQUIRED = ["枚举名", "输出通道", "输出缓冲", "初始值"]
INST_KEYS = ["DAC端口", "数据对齐"]


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


def parse_dac_instance(md_text: str) -> dict[str, str]:
    _, rows = _parse_table(md_text, "DAC端口")
    if not rows:
        raise SystemExit("错误：mcal_dac_channels.md 中未找到 DAC 实例参数表。")
    inst = rows[0]
    for key in INST_KEYS:
        if not inst.get(key, "").strip():
            raise SystemExit(f"错误：DAC 实例参数缺少字段「{key}」。")
    return inst


def parse_dac_channels(md_text: str) -> list[dict[str, str]]:
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
        raise SystemExit("错误：mcal_dac_channels.md 中未找到启用的 DAC 通道行。")

    for idx, row in enumerate(rows, start=1):
        for key in REQUIRED:
            if not row.get(key, "").strip():
                raise SystemExit(f"错误：第 {idx} 行缺少字段「{key}」。")
    return rows


def _yes_flag(value: str) -> str:
    return "1U" if value.lower() in {"yes", "y", "1", "true", "是"} else "0U"


def gen_enum(rows: list[dict[str, str]]) -> str:
    lines = ["typedef enum {"]
    for i, row in enumerate(rows):
        suffix = " = 0," if i == 0 else ","
        lines.append(f"    {row['枚举名']}{suffix}")
    lines.append("    eMcal_DacMaxNum,")
    lines.append("} eMcal_DacCh_e;")
    return "\n".join(lines)


def gen_inst(inst: dict[str, str]) -> str:
    return "\n".join(
        [
            "static const xMcal_DacInstCfg_t s_xMcal_DacInstCfg = {",
            f"    {inst['DAC端口']},",
            f"    {inst['数据对齐']},",
            "};",
        ]
    )


def gen_out_chan(rows: list[dict[str, str]]) -> str:
    values = ", ".join(f"{row['输出通道']}" for row in rows)
    return f"static const uint8_t s_ucMcalDacOutChan[eMcal_DacMaxNum] = {{ {values} }};"


def gen_out_buf(rows: list[dict[str, str]]) -> str:
    values = ", ".join(_yes_flag(row["输出缓冲"]) for row in rows)
    return f"static const uint8_t s_ucMcalDacOutBufEnable[eMcal_DacMaxNum] = {{ {values} }};"


def gen_init_val(rows: list[dict[str, str]]) -> str:
    values = ", ".join(f"{row['初始值']}U" for row in rows)
    return f"static const uint16_t s_usMcalDacInitValue[eMcal_DacMaxNum] = {{ {values} }};"


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
        print(f"错误：找不到 {C_PATH}，请先创建 mcal_dac.c 模板。", file=sys.stderr)
        return 1

    md_text = MD_PATH.read_text(encoding="utf-8")
    inst = parse_dac_instance(md_text)
    rows = parse_dac_channels(md_text)

    c_text = C_PATH.read_text(encoding="utf-8")
    c_text = replace_block(c_text, INST_BEGIN, INST_END, gen_inst(inst))
    c_text = replace_block(c_text, OUT_CHAN_BEGIN, OUT_CHAN_END, gen_out_chan(rows))
    c_text = replace_block(c_text, OUT_BUF_BEGIN, OUT_BUF_END, gen_out_buf(rows))
    c_text = replace_block(c_text, INIT_VAL_BEGIN, INIT_VAL_END, gen_init_val(rows))
    C_PATH.write_text(c_text, encoding="utf-8", newline="\n")

    enum_snippet = f"{ENUM_BEGIN}\n{gen_enum(rows)}\n{ENUM_END}\n"
    ENUM_SNIPPET_PATH.write_text(enum_snippet, encoding="utf-8", newline="\n")

    names = ", ".join(r["枚举名"] for r in rows)
    print(f"已生成 {len(rows)} 个 DAC 通道：{names}")
    print(f"已更新 {C_PATH.relative_to(ROOT)}")
    print("注意：mcal_dac.h 枚举区块请手动粘贴（Esafenet 会加密 Python 写入的 .h 文件）：")
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
