#!/usr/bin/env python3

import os
import re
from pathlib import Path


PROFILE_ROWS = [
    ("原版", "real", "original"),
    ("复刻版", "clone", "clone"),
    ("Clipper", "clipper", "clipper"),
]


def build_download_table(version_tag: str, repository: str):
    lines = []
    for display_name, profile, asset_name in PROFILE_ROWS:
        base = f"momentum-fw-cn-{version_tag}-{asset_name}"
        lines.append(
            "| {display} | `MOMENTUM_DEVICE={profile}` | "
            "[ZIP](https://github.com/{repo}/releases/download/{tag}/{base}.zip) | "
            "[TGZ](https://github.com/{repo}/releases/download/{tag}/{base}.tgz) | "
            "[DFU](https://github.com/{repo}/releases/download/{tag}/{base}.dfu) |".format(
                display=display_name,
                profile=profile,
                repo=repository,
                tag=version_tag,
                base=base,
            )
        )
    return "\n".join(lines)


def read_changelog():
    changelog_path = Path("CHANGELOG.md")
    if not changelog_path.exists():
        return ""

    changelog = changelog_path.read_text(encoding="utf-8").strip()
    if not changelog:
        return ""

    match = re.search(r"^## .+?(?=^## |\Z)", changelog, flags=re.MULTILINE | re.DOTALL)
    if not match:
        return ""

    latest_entry = match.group(0).strip()
    return f"\n\n---\n\n{latest_entry}\n"


if __name__ == "__main__":
    version_tag = os.environ["RELEASE_TAG"]
    repository = os.environ["GITHUB_REPOSITORY"]

    template_path = Path(".github/workflow_data/release.template.md")
    output_path = Path(".github/workflow_data/release.md")
    template = template_path.read_text(encoding="utf-8")
    notes = template.format(
        VERSION_TAG=version_tag,
        REPOSITORY=repository,
        DOWNLOAD_TABLE=build_download_table(version_tag, repository),
    )
    notes += read_changelog()
    output_path.write_text(notes, encoding="utf-8")
