#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate mcal_flash config from mcal_flash_config.md."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MD_PATH = ROOT / "source" / "mcal" / "config" / "mcal_flash_config.md"
H_PATH = ROOT / "source" / "mcal" / "inc" / "mcal_flash.h"
C_PATH = ROOT / "source" / "mcal" / "src" / "mcal_flash.c"
MACRO_SNIPPET_PATH = ROOT / "tools" / "mcal_flash_macro_snippet.txt"

MACRO_BEGIN = "/* GEN_MCAL_FLASH_MACRO_BEGIN */"
MACRO_END = "/* GEN_MCAL_FLASH_MACRO_END */"
CFG_BEGIN = "/* GEN_MCAL_FLASH_CFG_BEGIN */"
CFG_END = "/* GEN_MCAL_FLASH_CFG_END */"

REQUIRED = ["Flash基址", "页大小", "用户区页数", "长操作喂狗"]


def _split_row(line: str) -> list[str]:
    line = line.strip()
    if not line.startswith("|"):
        return []
    return [p.strip() for p in line.strip("|").split("|")]


def _is_separator(row: list[str]) -> bool:
    return all(re.fullmatch(r":?-+:?", cell.replace(" ", "")) or cell == "" for cell in row)


def _yes_flag(value: str) -> str:
    return "1U" if value.lower() in {"yes", "y", "1", "true", "是"} else "0U"


def _parse_hex(value: str) -> str:
    text = value.strip()
    if text.lower().startswith("0x"):
        return f"{text.upper()}U"
    return f"0x{int(text):X}U"


def parse_flash_config(md_text: str) -> dict[str, str]:
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
            if cells[0] == "Flash基址":
                in_table = True
                headers = cells
            continue

        if _is_separator(cells):
            continue

        item = {headers[i]: cells[i] if i < len(cells) else "" for i in range(len(headers))}
        rows.append(item)

    if not rows:
        raise SystemExit("错误：mcal_flash_config.md 中未找到 Flash 参数表。")

    row = rows[0]
    for key in REQUIRED:
        if not row.get(key, "").strip():
            raise SystemExit(f"错误：Flash 配置缺少字段「{key}」。")
    return row


def gen_macros(row: dict[str, str]) -> str:
    base = _parse_hex(row["Flash基址"])
    page = _parse_hex(row["页大小"])
    pages = f"{row['用户区页数']}U"
    return "\n".join(
        [
            f"#define MCAL_FLASH_START_ADDR    ( {base} )",
            f"#define MCAL_FLASH_PAGE_SIZE     ( {page} )",
            f"#define MCAL_FLASH_USER_PAGES    ( {pages} )",
            "#define MCAL_FLASH_USER_END_ADDR ( MCAL_FLASH_START_ADDR + MCAL_FLASH_PAGE_SIZE * MCAL_FLASH_USER_PAGES - 1U )",
        ]
    )


def gen_cfg(row: dict[str, str]) -> str:
    return "\n".join(
        [
            "static const xMcal_FlashCfg_t s_xMcal_FlashCfg = {",
            f"    {_parse_hex(row['Flash基址'])},",
            f"    {_parse_hex(row['页大小'])},",
            f"    {row['用户区页数']}U,",
            f"    {_yes_flag(row['长操作喂狗'])},",
            "};",
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
        print(f"错误：找不到 {C_PATH}，请先创建 mcal_flash.c 模板。", file=sys.stderr)
        return 1

    row = parse_flash_config(MD_PATH.read_text(encoding="utf-8"))
    c_text = C_PATH.read_text(encoding="utf-8")
    c_text = replace_block(c_text, CFG_BEGIN, CFG_END, gen_cfg(row))
    C_PATH.write_text(c_text, encoding="utf-8", newline="\n")

    macro_snippet = f"{MACRO_BEGIN}\n{gen_macros(row)}\n{MACRO_END}\n"
    MACRO_SNIPPET_PATH.write_text(macro_snippet, encoding="utf-8", newline="\n")

    print(
        f"已生成 Flash 配置：base={row['Flash基址']} page={row['页大小']} pages={row['用户区页数']}"
    )
    print(f"已更新 {C_PATH.relative_to(ROOT)}")
    print("注意：mcal_flash.h 宏区块请手动粘贴（Esafenet 会加密 Python 写入的 .h 文件）：")
    print(f"  {MACRO_SNIPPET_PATH.relative_to(ROOT)}")
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
