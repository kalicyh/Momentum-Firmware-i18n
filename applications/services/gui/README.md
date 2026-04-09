## 当前中文字体方案

当前 GUI 层的中文显示方案已经改成：

- 编译时生成一份总中文字库 `primary_zh.u8f`
- 运行时由 `canvas.c` 私有懒加载
- 检测到中文字符串时临时切换字体绘制

不再使用：

- `asset_packs` 挂中文主字库
- `kalicyh_font.h/.c`
- `canvas_set_custom_u8g2_font(canvas, kalicyh);`

## 资源路径

`canvas.c` 当前读取的路径是：

```c
EXT_PATH("zh_fonts/primary_zh.u8f")
```

设备上对应：

```text
/ext/zh_fonts/primary_zh.u8f
```

## 构建链

中文构建时：

1. 从 `localization/zh_CN/strings.json` 提取全部非 ASCII 字符
2. 调用 `scripts/momentum_zh_font_gen.py`
3. 使用仓库内的 `tools/u8g2_cn_tools/bdfconv`
4. 生成 `primary_zh.u8f`

只有：

```bash
MOMENTUM_UI_LANG=zh_CN ./fbt ...
```

时才会启用这条路径。

## Canvas 接入方式

当前逻辑在 `applications/services/gui/canvas.c`：

- 检查字符串是否包含非 ASCII
- 若包含，则加载 `primary_zh.u8f`
- 临时 `u8g2_SetFont(&canvas->fb, zh_font)`
- 绘制完成后恢复原字体

所以当前方案是：

- 英文仍走原生字体
- 中文走独立总字库
