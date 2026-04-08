# CN-Clipper 中文显示方案

## 目标

中文方案改成：

1. 编译时从中文词条源收集全部会用到的中文字符
2. 生成一份总中文字库文件
3. 运行时通过 `zh_font_cache` 按页按需加载和释放字形

默认固件仍然是英文：

- 不生成中文字库
- 不携带中文资源
- 继续使用原始英文字体

中文固件：

- 编译时生成总中文字库
- 输出到独立目录 `/ext/zh_fonts`
- 运行时只缓存当前页面实际需要的汉字

## 编译时流程

中文构建时：

1. 读取 `localization/zh_CN/strings.json`
2. 提取全部非 ASCII 字符
3. 去重排序
4. 生成 `primary_zh.map`
5. 调用仓库内 vendored `tools/u8g2_cn_tools/bdfconv`
6. 生成中间文件：
   - `build/<target>/generated/zh_fonts/primary_zh.map`
   - `build/<target>/generated/zh_fonts/primary_zh.c`
7. 导出最终资源：
   - `build/<target>/resources/zh_fonts/primary_zh.u8f`

说明：

- 中文字库不放在 `asset_packs`
- 字体工具和 BDF 一起放进当前仓库
- 不依赖用户本机另一个项目

## 运行时流程

新增 `zh_font_cache` 模块：

- 打开 `/ext/zh_fonts/primary_zh.u8f`
- 从总字库中读取指定字符的 glyph
- 维护固定数量的 glyph cache
- 页面切换时清空或复用缓存

规划接口：

- `zh_font_cache_init()`
- `zh_font_cache_free()`
- `zh_font_cache_reset_page()`
- `zh_font_cache_prepare_utf8(const char* text)`
- `zh_font_cache_has_glyph(uint32_t codepoint)`

## GUI 接入原则

- ASCII 继续走默认字体
- 中文绘制前把当前页面文本送进 cache
- 只缓存当前页实际出现的汉字
- 超出缓存容量时执行淘汰

## 当前阶段

第一阶段先落：

- 中文词条源
- 总中文字库生成脚本
- repo 内 vendored `bdfconv` 和 BDF
- `zh_font_cache` 模块骨架

第二阶段再做：

- `canvas` 接入 `zh_font_cache`
- 页面级文本预热
- 用 `system_settings` / `momentum_app` 做试点
