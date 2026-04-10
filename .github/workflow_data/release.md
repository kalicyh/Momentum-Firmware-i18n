## 下载

### 固件版本

| 版本 | 说明 | 更新包 ZIP | 更新包 TGZ | 完整 DFU |
|---|---|---|---|---|
{DOWNLOAD_TABLE}

### 字体文件

| 文件 | 说明 | 下载 |
|---|---|---|
| `momentum-fw-cn-{VERSION_TAG}-zh-fonts.zip` | 全部 `tools/u8g2_cn_tools/bdf` 对应生成的 `.u8f` 文件 | [下载](https://github.com/{REPOSITORY}/releases/download/{VERSION_TAG}/momentum-fw-cn-{VERSION_TAG}-zh-fonts.zip) |

可以手动将想使用的字体文件改名为 `primary_zh.u8f` 后放入 `/ext/zh_fonts/`。

## 说明

- `原版`：`MOMENTUM_DEVICE=real`
- `复刻版`：`MOMENTUM_DEVICE=clone`
- `Clipper`：`MOMENTUM_DEVICE=clipper`
- 所有 Release 固件均为中文构建：`MOMENTUM_UI_LANG=zh_CN`
