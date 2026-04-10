#!/usr/bin/env python3

import os
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


if __name__ == "__main__":
    version_tag = os.environ["RELEASE_TAG"]
    repository = os.environ["GITHUB_REPOSITORY"]

    changelog = Path("CHANGELOG.md").read_text(encoding="utf-8")
    template_path = Path(".github/workflow_data/release.md")
    template = template_path.read_text(encoding="utf-8")
    notes = template.format(
        VERSION_TAG=version_tag,
        REPOSITORY=repository,
        DOWNLOAD_TABLE=build_download_table(version_tag, repository),
        CHANGELOG=changelog,
    )
    template_path.write_text(notes, encoding="utf-8")
