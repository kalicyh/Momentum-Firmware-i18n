## 当前中文字体方案

当前 GUI 层的中文显示方案已经改成：

- 编译时生成一份总中文字库 `primary_zh.c`
- 字体数据链接进固件 `.rodata`
- 检测到中文字符串时临时切换字体绘制

不再使用：

- `asset_packs` 挂中文主字库
- 运行时从 `/ext/zh_fonts/primary_zh.u8f` 读取字体
- 运行时 `malloc` 常驻中文字库
- `kalicyh_font.h/.c`
- `canvas_set_custom_u8g2_font(canvas, kalicyh);`

## 字体符号

`canvas.c` 当前直接引用编译进固件的符号：

```c
extern const uint8_t primary_zh[];
```

构建仍会生成 `primary_zh.u8f` 资源文件用于兼容和检查，但 GUI 运行时不依赖它。

## 构建链

中文构建时：

1. 从 `localization/zh_CN/strings.json` 提取全部非 ASCII 字符
2. 扫描源码字符串字面量、`application.fam` 里的 `name_zh`，以及海豚动画文本
3. 调用 `scripts/momentum_zh_font_gen.py`
4. 使用仓库内的 `tools/u8g2_cn_tools/bdfconv`
5. 生成 `primary_zh.c` 和兼容用的 `primary_zh.u8f`
6. 将 `primary_zh.c` 编入 `fwassets` 静态库

只有：

```bash
MOMENTUM_UI_LANG=zh_CN ./fbt ...
```

时才会启用这条路径。

## Canvas 接入方式

当前逻辑在 `applications/services/gui/canvas.c`：

- 检查字符串是否包含非 ASCII
- 若包含，则使用 `.rodata` 中的 `primary_zh`
- 临时 `u8g2_SetFont(&canvas->fb, zh_font)`
- 绘制完成后恢复原字体

所以当前方案是：

- 英文仍走原生字体
- 中文走独立总字库，但不占用运行时 heap
