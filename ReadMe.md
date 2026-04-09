<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset=".github/assets/logo_dark.png">
    <source media="(prefers-color-scheme: light)" srcset=".github/assets/logo_light.png">
    <img
        alt="Momentum 固件"
        src=".github/assets/logo_dark.png">
  </picture>
</p>

<h2 align="center">
  <a href="#安装方法">安装</a> · <a href="#变更列表">功能</a> · <a href="https://discord.gg/momentum">Discord</a> · <a href="#支持">支持</a>
</h2>

<img src=".github/assets/qq.png" align="right" height="260vh"/>

本分支基于 [Momentum-Firmware](https://github.com/Next-Flip/Momentum-Firmware) 维护，

保留上游主体功能，并继续整理适合当前仓库的中文方案与常用定制。

- 提供中文固件编译支持
- 使用独立总中文字库
- 添加更多 Tesla SubGhz 数据
- 添加更多 Sextoy SubGhz 数据
- 添加复刻设备状态开关

当前中文方案已经不是旧版 `CN` / `CN_built-in` 的实现方式。

现在使用的是：

- 编译时从中文词条源生成总字库
- 资源输出到 `zh_fonts/primary_zh.u8f`
- 运行时由 `canvas` 私有懒加载中文字体

详细说明请看：

- [汉化方法](./汉化方法.md)

QQ 交流群：`716547787`

<br clear="right"/>

该自定义固件基于 [官方固件](https://github.com/flipperdevices/flipperzero-firmware) 开发，并包含 [Unleashed](https://github.com/DarkFlippers/unleashed-firmware) 的大量实用功能。它也是 Xtreme 固件路线的延续版本之一。

<br>
<h2 align="center">操作模式</h2>

本分支的目标是在保留 Momentum 主体体验的前提下，继续维护一套可用、清晰、可同步上游的中文固件方案，同时保留常用增强功能。
<br><br>
- <h4>功能丰富：集成稳定且实用的功能与应用，并保留常见增强能力。</h4>

- <h4>稳定性：尽量减少中文显示方案对主固件结构和体积的影响，保持可编译、可升级。</h4>

- <h4>可维护性：中文字库生成、显示路径、文档说明都尽量收敛成单一方案，避免继续维护多套旧逻辑。</h4>

<br><br>
以下仅列出一部分明显功能和改动，完整内容仍建议结合上游 Momentum 更新记录一起查看。

<br>
<h2 align="center">Momentum 设置</h2>

我们保留了 Momentum 自带的设置应用，用于配置固件的大部分核心行为：

<img src=".github/assets/settings.png" align="left" height="160vh"/>
<img align="left" height="180vh" width="10" src="https://upload.wikimedia.org/wikipedia/commons/3/3d/1_120_transparent.png">

- <ins><b>界面：</b></ins> 调整桌面动画、主菜单、锁屏、文件浏览器等界面行为。
- <ins><b>协议：</b></ins> 配置 SubGhz 设置，管理自定义频率，扩展 SubGhz 频率范围，并配置外部模块所用 GPIO 引脚。
- <ins><b>杂项：</b></ins> 修改设备名称、XP 等级、屏幕选项，并配置 [RGB 背光](https://github.com/Z3BRO/Flipper-Zero-RGB-Backlight) 等功能。

<br clear="left"/>

<br>
<h2 align="center">动画 / 资源包</h2>

Momentum 仍然保留完整的资源包系统，用于切换动画、图标和主题资源。

<img src=".github/assets/packs-folder.png" align="left" width="200px"/>
你可以自行制作资源包，也可以从社区或网站获取现成资源包。关于资源包格式和制作方式，可参考上游文档中的 [Asset Packs 说明](https://github.com/Next-Flip/Momentum-Firmware/blob/dev/documentation/file_formats/AssetPacks.md)。

<br clear="left"/>

<br>

<img src=".github/assets/packs-select.png" align="left" width="200px"/>
资源包上传到 Flipper 的 `SD/asset_packs` 目录后，可以在 `Momentum Settings > Interface > Graphics` 中进行选择和切换。

<br clear="left"/>

<br>

<img src=".github/assets/packs-done.png" align="left" width="200px"/>
需要注意的是，当前中文字体方案已经不再依赖资源包。资源包只负责动画、图标和主题表现，不再承担中文主字库功能。

<br clear="left"/>

<br>
<h2 align="center">Bad 键盘</h2>

<img src=".github/assets/badkb.png" align="left" width="250px"/>
BadUSB 本身已经很强，但 Bad-KB 提供了更多 USB 和蓝牙参数配置能力。

在蓝牙模式下，可以伪装显示名称和 MAC 地址，用于更灵活的测试场景。

在 USB 模式下，也可以伪造制造商、产品名，以及 VID/PID 等信息。

<br clear="left"/>

<br>
<h2 align="center" id="变更列表">变更列表</h2>

变更内容很多，下面只列出从终端用户角度较明显的一部分，不是完整列表。

```txt
[新增]

- Momentum 设置应用
- 资源包系统
- 更多界面自定义
- Bad-Keyboard
- BLE Spam
- FindMy Flipper
- NFC Maker
- Wardriver
- SD 卡文件搜索
- 更多 NFC 解析器和协议
- Subdriving（Sub-GHz GPS 坐标）
- 更方便的伪装功能（名称、MAC、序列号）
- 视频游戏模块颜色配置
- 增强 RGB 背光模式
- 设备内文件管理
- Infrared GPIO 设置记忆
- 更多 Tesla SubGhz 数据
- 更多 Sextoy SubGhz 数据
- 中文固件编译支持
```

```txt
[更新]

- 增强 WiFi 支持
- 扩展键盘与符号输入
- 文件浏览器排序与更多文件类型支持
- 优化等级系统
- 桌面按键绑定系统
- 存储后端增强
- 扩展 Sub-GHz 应用
- 改进错误提示
```

```txt
[移除]

- 未使用的 Dummy Mode
- 已损坏或多余的应用
```

<br>
<h2 align="center" id="安装方法">安装方法</h2>

推荐继续使用 Momentum 上游常见的几种安装方式，按自己习惯选择即可：

<br>

> <details><summary><code>网页更新器（Chrome）</code></summary><ul>
>   <li>确保 qFlipper 已关闭</li>
>   <li>打开 <a href="https://momentum-fw.dev/update">Web Updater</a></li>
>   <li>点击 <code>Connect</code> 并从列表中选择你的 Flipper</li>
>   <li>点击 <code>Install</code> 并等待安装完成</li>
> </ul></details>

> <details><summary><code>Flipper Lab / 手机 App</code></summary><ul>
>   <li>(桌面端) 确保 qFlipper 已关闭</li>
>   <li>(移动端) 确保已安装并配对 <a href="https://docs.flipper.net/mobile-app">Flipper Mobile App</a></li>
>   <li>打开 <a href="https://github.com/Next-Flip/Momentum-Firmware/releases/latest">最新发布页面</a></li>
>   <li>点击 <code>☁️ Flipper Lab/App</code> 链接</li>
>   <li>(桌面端) 点击 <code>Connect</code> 并选择设备</li>
>   <li>(桌面端) 点击 <code>Install</code> 并等待完成</li>
>   <li>(移动端) 接受提示并在 App 中继续安装</li>
> </ul></details>

> <details><summary><code>qFlipper 安装包 (.tgz)</code></summary><ul>
>   <li>从 <a href="https://github.com/Next-Flip/Momentum-Firmware/releases/latest">最新发布页面</a> 下载 <code>.tgz</code> 包</li>
>   <li>确保 <code>WebUpdater</code> 和 <code>lab.flipper.net</code> 已关闭</li>
>   <li>打开 <a href="https://flipperzero.one/update">qFlipper</a> 并连接设备</li>
>   <li>点击 <code>Install from file</code></li>
>   <li>选择下载好的 <code>.tgz</code> 并等待完成</li>
> </ul></details>

> <details><summary><code>压缩包 (.zip)</code></summary><ul>
>   <li>从 <a href="https://github.com/Next-Flip/Momentum-Firmware/releases/latest">最新发布页面</a> 下载 <code>.zip</code> 包</li>
>   <li>解压后得到新的固件目录</li>
>   <li>打开 <a href="https://flipperzero.one/update">qFlipper</a>，进入 <code>SD/update</code> 并把固件目录放进去</li>
>   <li>在 Flipper 上进入文件菜单，找到更新目录</li>
>   <li>进入目录后运行名为 <code>Update</code> 的文件</li>
> </ul></details>

<br>
<h2 align="center">自行构建</h2>

```bash
下载仓库:
$ git clone --recursive --jobs 8 https://github.com/Next-Flip/Momentum-Firmware.git
$ cd Momentum-Firmware/

直接刷写到 Flipper（需要 USB 连接，qFlipper 已关闭）:
$ ./fbt flash_usb_full

编译英文固件:
$ ./fbt COMPACT=1 DEBUG=0

编译中文固件:
$ MOMENTUM_UI_LANG=zh_CN ./fbt COMPACT=1 DEBUG=0

编译英文更新包:
$ ./fbt updater_package

编译中文更新包:
$ MOMENTUM_UI_LANG=zh_CN ./fbt updater_package

构建并启动单个应用:
$ ./fbt launch APPSRC=your_appid
```

<h2 align="center">Stargazers 随时间变化</h2>

[![Stargazers over time](https://starchart.cc/Next-Flip/Momentum-Firmware.svg?variant=adaptive)](https://starchart.cc/Next-Flip/Momentum-Firmware)

<h2 align="center" id="支持">支持</h2>

如果你喜欢这套固件，欢迎传播、反馈和补充改进意见。

> **[Ko-fi](https://ko-fi.com/willyjl)**：单次或周期性支持

> **[PayPal](https://paypal.me/willyjl1)**：单次支持

> **BTC**：`1EnCi1HF8Jw6m2dWSUwHLbCRbVBCQSyDKm`

感谢支持。
