#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate mcal_dog config from mcal_dog_config.md."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MD_PATH = ROOT / "source" / "mcal" / "config" / "mcal_dog_config.md"
C_PATH = ROOT / "source" / "mcal" / "src" / "mcal_dog.c"

CFG_BEGIN = "/* GEN_MCAL_DOG_CFG_BEGIN */"
CFG_END = "/* GEN_MCAL_DOG_CFG_END */"

REQUIRED = ["看门狗类型", "重装载值", "预分频", "初始化喂狗"]


def _split_row(line: str) -> list[str]:
    line = line.strip()
    if not line.startswith("|"):
        return []
    return [p.strip() for p in line.strip("|").split("|")]


def _is_separator(row: list[str]) -> bool:
    return all(re.fullmatch(r":?-+:?", cell.replace(" ", "")) or cell == "" for cell in row)


def _yes_flag(value: str) -> str:
    return "1U" if value.lower() in {"yes", "y", "1", "true", "是"} else "0U"


def parse_dog_config(md_text: str) -> dict[str, str]:
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
            if cells[0] == "启用" and "看门狗类型" in cells:
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
        raise SystemExit("错误：mcal_dog_config.md 中未找到启用的看门狗配置行。")

    row = rows[0]
    if len(rows) > 1:
        print("警告：仅使用第一行看门狗配置。")

    for key in REQUIRED:
        if not row.get(key, "").strip():
            raise SystemExit(f"错误：看门狗配置缺少字段「{key}」。")

    if row["看门狗类型"] != "FWDGT":
        raise SystemExit(f"错误：不支持的看门狗类型「{row['看门狗类型']}」，当前仅支持 FWDGT。")

    return row


def gen_cfg(row: dict[str, str]) -> str:
    return "\n".join(
        [
            "static const xMcal_DogCfg_t s_xMcal_DogCfg = {",
            f"    {row['重装载值']}U,",
            f"    {row['预分频']},",
            f"    {_yes_flag(row['初始化喂狗'])},",
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
        print(f"错误：找不到 {C_PATH}，请先创建 mcal_dog.c 模板。", file=sys.stderr)
        return 1

    row = parse_dog_config(MD_PATH.read_text(encoding="utf-8"))
    c_text = replace_block(C_PATH.read_text(encoding="utf-8"), CFG_BEGIN, CFG_END, gen_cfg(row))
    C_PATH.write_text(c_text, encoding="utf-8", newline="\n")

    print(f"已生成看门狗配置：{row['看门狗类型']} reload={row['重装载值']} prescaler={row['预分频']}")
    print(f"已更新 {C_PATH.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
