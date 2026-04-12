#!/usr/bin/env python3

import argparse
import json
import os
import re
import subprocess
from pathlib import Path

STRING_LITERAL_RE = re.compile(r'"((?:\\.|[^"\\])*)"', re.DOTALL)

SOURCE_SCAN_ROOTS = (
    Path("applications/main"),
    Path("applications/services"),
    Path("applications/settings"),
    Path("lib"),
)
ANIMATION_TEXT_ROOT = Path("assets/dolphin")
EXTERNAL_APPS_DEFAULT = Path("localization/zh_CN/external_apps.txt")


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--strings", required=True)
    parser.add_argument("--tools-dir", required=True)
    parser.add_argument("--bdf")
    parser.add_argument("--bdf-dir")
    parser.add_argument("--work-dir", required=True)
    parser.add_argument("--out-dir", required=True)
    parser.add_argument("--stamp", required=True)
    parser.add_argument("--enabled", required=True, choices=("0", "1"))
    parser.add_argument("--external-apps")
    return parser.parse_args()


def collect_chars_from_strings(strings_path: Path):
    data = json.loads(strings_path.read_text(encoding="utf-8"))
    chars = set()
    for row in data:
        for ch in row["text"]:
            if ord(ch) > 127:
                chars.add(ch)
    return chars


def iter_source_files(repo_root: Path):
    for scan_root in SOURCE_SCAN_ROOTS:
        root = repo_root / scan_root
        if not root.is_dir():
            continue
        for pattern in ("*.c", "*.h"):
            yield from root.rglob(pattern)


def decode_c_string_literal(literal: str):
    escape_map = {
        "n": "\n",
        "r": "\r",
        "t": "\t",
        "\\": "\\",
        '"': '"',
        "'": "'",
        "0": "\0",
    }
    result = []
    i = 0
    while i < len(literal):
        ch = literal[i]
        if ch == "\\" and i + 1 < len(literal):
            nxt = literal[i + 1]
            result.append(escape_map.get(nxt, nxt))
            i += 2
            continue
        result.append(ch)
        i += 1
    return "".join(result)


def collect_chars_from_source_literals(repo_root: Path):
    chars = set()
    for source_file in iter_source_files(repo_root):
        contents = source_file.read_text(encoding="utf-8", errors="ignore")
        for match in STRING_LITERAL_RE.finditer(contents):
            literal = decode_c_string_literal(match.group(1))
            for ch in literal:
                if ord(ch) > 127:
                    chars.add(ch)
    return chars


def collect_chars_from_animation_text(repo_root: Path):
    chars = set()
    root = repo_root / ANIMATION_TEXT_ROOT
    if not root.is_dir():
        return chars

    for meta_file in root.rglob("meta.txt"):
        contents = meta_file.read_text(encoding="utf-8", errors="ignore")
        for line in contents.splitlines():
            if not line.startswith("Text:"):
                continue
            text = line[5:].strip().replace("\\n", "\n")
            for ch in text:
                if ord(ch) > 127:
                    chars.add(ch)
    return chars


def collect_chars_from_external_apps(repo_root: Path, external_apps_path: Path | None):
    chars = set()
    if not external_apps_path:
        return chars
    if not external_apps_path.is_file():
        return chars

    external_root = repo_root / "applications" / "external"
    if not external_root.is_dir():
        return chars

    for line in external_apps_path.read_text(encoding="utf-8", errors="ignore").splitlines():
        app_name = line.strip()
        if not app_name or app_name.startswith("#"):
            continue
        app_root = external_root / app_name
        if not app_root.is_dir():
            continue
        for pattern in ("*.c", "*.h"):
            for source_file in app_root.rglob(pattern):
                contents = source_file.read_text(encoding="utf-8", errors="ignore")
                for match in STRING_LITERAL_RE.finditer(contents):
                    literal = decode_c_string_literal(match.group(1))
                    for ch in literal:
                        if ord(ch) > 127:
                            chars.add(ch)
    return chars


def write_map(chars, path: Path):
    lines = ["32-128,"]
    for codepoint in sorted(ord(ch) for ch in chars):
        lines.append(f"${codepoint:04X},")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def write_chars_report(chars, path: Path):
    sorted_chars = sorted(chars, key=ord)
    lines = [
        f"count={len(sorted_chars)}",
        "".join(sorted_chars),
        "",
    ]
    lines.extend(f"U+{ord(ch):04X} {ch}" for ch in sorted_chars)
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


def sanitize_symbol_name(name: str):
    return re.sub(r"[^A-Za-z0-9_]", "_", name)


def generate_font(
    bdfconv: Path, bdf: Path, map_file: Path, work_dir: Path, out_dir: Path, out_name: str
):
    c_file = work_dir / f"{out_name}.c"
    u8f_file = out_dir / f"{out_name}.u8f"
    symbol_name = sanitize_symbol_name(out_name)
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
            symbol_name,
            "-o",
            str(c_file),
        ],
        check=True,
        capture_output=True,
        text=True,
    )
    c_to_u8f(c_file, u8f_file)


def main():
    args = parse_args()
    work_dir = Path(args.work_dir)
    out_dir = Path(args.out_dir)
    stamp = Path(args.stamp)
    repo_root = Path(__file__).resolve().parent.parent
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
    bdf = Path(args.bdf) if args.bdf else None
    bdf_dir = Path(args.bdf_dir) if args.bdf_dir else None
    external_apps = (
        Path(args.external_apps)
        if args.external_apps
        else repo_root / EXTERNAL_APPS_DEFAULT
    )

    if bool(bdf) == bool(bdf_dir):
        raise ValueError("Specify exactly one of --bdf or --bdf-dir")

    if not bdfconv.is_file():
        raise FileNotFoundError(f"bdfconv not found: {bdfconv}")
    if not os.access(bdfconv, os.X_OK):
        bdfconv.chmod(bdfconv.stat().st_mode | 0o111)

    if bdf and not bdf.is_file():
        raise FileNotFoundError(f"BDF not found: {bdf}")
    if bdf_dir and not bdf_dir.is_dir():
        raise FileNotFoundError(f"BDF dir not found: {bdf_dir}")

    chars = collect_chars_from_strings(strings)
    chars.update(collect_chars_from_source_literals(repo_root))
    chars.update(collect_chars_from_animation_text(repo_root))
    chars.update(collect_chars_from_external_apps(repo_root, external_apps))
    if chars:
        map_file = work_dir / "primary_zh.map"
        chars_file = work_dir / "primary_zh_chars.txt"
        write_chars_report(chars, chars_file)
        write_map(chars, map_file)
        if bdf:
            generate_font(bdfconv, bdf, map_file, work_dir, out_dir, "primary_zh")
        else:
            for font_bdf in sorted(bdf_dir.glob("*.bdf")):
                generate_font(bdfconv, font_bdf, map_file, work_dir, out_dir, font_bdf.stem)

    stamp.parent.mkdir(parents=True, exist_ok=True)
    stamp.write_text("ok\n", encoding="utf-8", newline="\n")


if __name__ == "__main__":
    main()
