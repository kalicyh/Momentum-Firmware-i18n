# App Name Localization Exceptions

This repository supports `name_zh` in `application.fam`, but a small number of apps are intentionally kept in English even in `zh_CN` builds.

## Why these apps are not localized

The current launcher flow still has several places that treat an app's display name as its runtime identifier.

- Internal apps can be started by `name` or `appid`.
- Settings and menu-external apps are still looked up by display `name`.
- Some code paths also use hardcoded English strings such as `"UpdaterApp"` or `"Storage"` when starting apps or handling special cases.

Because of that, changing `name_zh` for the apps below can break:

- updater startup
- settings entry launching
- desktop shortcut actions
- archive file associations

If these code paths are later refactored to use `appid` consistently, the exceptions below can be removed.

## Current exceptions

The following apps intentionally keep `name_zh` equal to their English `name`:

- `updater_app`
- `desktop_settings`
- `power_settings`
- `storage_settings`
- `js_app`
- `passport`

## Related call sites

### `updater_app` / `UpdaterApp`

These places start the updater by hardcoded English name:

- `applications/system/updater/cli/updater_cli.c`
- `applications/main/archive/scenes/archive_scene_browser.c`

If `name_zh` is changed to Chinese without updating these paths, firmware update flow can fail to enter the updater app.

### `desktop_settings` / `Desktop`

This place starts the desktop settings app by hardcoded English name:

- `applications/services/desktop/scenes/desktop_scene_lock_menu.c`

If `name_zh` becomes Chinese, PIN setup from the lock menu can fail to open the settings app.

### `power_settings` / `Power`

These places start the power settings app by hardcoded English name:

- `applications/services/desktop/desktop.c`
- `applications/services/desktop/desktop_keybinds.c`

If `name_zh` becomes Chinese, shutdown fallback and the desktop "Device Info" shortcut can fail.

### `storage_settings` / `Storage`

These places depend on the English display name:

- `applications/services/loader/loader_menu_storage.c`
- `applications/services/loader/loader_menu.c`
- `applications/services/desktop/desktop_keybinds.c`

This is both a launcher dependency and a special-case string comparison. If `name_zh` becomes Chinese, SD formatting fallback and the desktop "Wipe Device" shortcut can fail.

### `js_app` / `JS Runner`

These places use the English display name when JS runner is built into firmware instead of launched as a `.fap`:

- `applications/services/loader/loader_applications.c`
- `applications/main/archive/scenes/archive_scene_browser.c`

If `name_zh` becomes Chinese, `.js` files may fail to open through the built-in JS runner path.

### `passport` / `Passport`

These places keep desktop shortcut values as English strings:

- `applications/services/desktop/desktop_keybinds.c`
- `applications/settings/desktop_settings/desktop_settings_app.c`

If `name_zh` becomes Chinese, the default desktop shortcut value `"Passport"` may no longer match the actual app name used by the loader.

## Recommendation for future cleanup

The correct long-term fix is:

1. Make all launcher paths use `appid` instead of display `name`.
2. Extend settings and menu-external app lookup to support `appid`, not just `name`.
3. Replace hardcoded comparisons like `"Storage"` with stable IDs.

After that, these six apps can safely use Chinese `name_zh` values again.
