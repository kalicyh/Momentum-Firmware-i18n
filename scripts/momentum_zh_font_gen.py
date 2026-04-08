#!/usr/bin/env python3

import argparse
import json
import os
import subprocess
from pathlib import Path


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--strings", required=True)
    parser.add_argument("--tools-dir", required=True)
    parser.add_argument("--bdf", required=True)
    parser.add_argument("--work-dir", required=True)
    parser.add_argument("--out-dir", required=True)
    parser.add_argument("--stamp", required=True)
    parser.add_argument("--enabled", required=True, choices=("0", "1"))
    return parser.parse_args()


def collect_chars(strings_path: Path):
    data = json.loads(strings_path.read_text(encoding="utf-8"))
    chars = set()
    for row in data:
        for ch in row["text"]:
            if ord(ch) > 127:
                chars.add(ch)
    return chars


def write_map(chars, path: Path):
    lines = ["32-128,"]
    for codepoint in sorted(ord(ch) for ch in chars):
        lines.append(f"${codepoint:04X},")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def c_to_u8f(c_path: Path, u8f_path: Path):
    code = c_path.read_bytes().split(b' U8G2_FONT_SECTION("')[1].split(b'") =')[1].strip()
    font = b""
    for line in code.splitlines():
        if line.count(b'"') == 2:
            font += (
                line[line.find(b'"') + 1 : line.rfind(b'"')]
                .decode("unicode_escape")
                .encode("latin_1")
            )
    font += b"\0"
    u8f_path.parent.mkdir(parents=True, exist_ok=True)
    u8f_path.write_bytes(font)


def main():
    args = parse_args()
    work_dir = Path(args.work_dir)
    out_dir = Path(args.out_dir)
    stamp = Path(args.stamp)
    work_dir.mkdir(parents=True, exist_ok=True)
    out_dir.mkdir(parents=True, exist_ok=True)
    for stale_name in ("primary_zh.c", "primary_zh.map"):
        stale_path = out_dir / stale_name
        if stale_path.exists():
            stale_path.unlink()

    if args.enabled != "1":
        (out_dir / "primary_zh.u8f").write_bytes(b"")
        stamp.parent.mkdir(parents=True, exist_ok=True)
        stamp.write_text("disabled\n", encoding="utf-8", newline="\n")
        return

    strings = Path(args.strings)
    tools_dir = Path(args.tools_dir)
    bdfconv = tools_dir / "bdfconv"
    bdf = Path(args.bdf)

    if not bdfconv.is_file():
        raise FileNotFoundError(f"bdfconv not found: {bdfconv}")
    if not bdf.is_file():
        raise FileNotFoundError(f"BDF not found: {bdf}")
    if not os.access(bdfconv, os.X_OK):
        bdfconv.chmod(bdfconv.stat().st_mode | 0o111)

    chars = collect_chars(strings)
    if chars:
        map_file = work_dir / "primary_zh.map"
        c_file = work_dir / "primary_zh.c"
        u8f_file = out_dir / "primary_zh.u8f"
        write_map(chars, map_file)
        subprocess.run(
            [
                str(bdfconv),
                str(bdf),
                "-b",
                "0",
                "-f",
                "1",
                "-M",
                str(map_file),
                "-n",
                "momentum_primary_zh",
                "-o",
                str(c_file),
            ],
            check=True,
            capture_output=True,
            text=True,
        )
        c_to_u8f(c_file, u8f_file)

    stamp.parent.mkdir(parents=True, exist_ok=True)
    stamp.write_text("ok\n", encoding="utf-8", newline="\n")


if __name__ == "__main__":
    main()
