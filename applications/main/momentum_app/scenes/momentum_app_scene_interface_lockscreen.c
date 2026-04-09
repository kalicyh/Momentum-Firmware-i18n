#include "../momentum_app.h"

enum VarItemListIndex {
    VarItemListIndexLockOnBoot,
    VarItemListIndexFormatOn10BadPins,
    VarItemListIndexPinUnlockFromApp,
    VarItemListIndexShowTime,
    VarItemListIndexShowSeconds,
    VarItemListIndexShowDate,
    VarItemListIndexShowStatusbar,
    VarItemListIndexUnlockPrompt,
};

void momentum_app_scene_interface_lockscreen_var_item_list_callback(void* context, uint32_t index) {
    MomentumApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void momentum_app_scene_interface_lockscreen_lock_on_boot_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    momentum_settings.lock_on_boot = value;
    app->save_settings = true;
}

static void momentum_app_scene_interface_lockscreen_bad_pins_format_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    momentum_settings.bad_pins_format = value;
    app->save_settings = true;
}

static void
    momentum_app_scene_interface_lockscreen_allow_locked_rpc_usb_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    momentum_settings.allow_locked_rpc_usb = value;
    app->save_settings = true;
}

static void
    momentum_app_scene_interface_lockscreen_allow_locked_rpc_ble_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    momentum_settings.allow_locked_rpc_ble = value;
    app->save_settings = true;
}

static void
    momentum_app_scene_interface_lockscreen_lockscreen_poweroff_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    momentum_settings.lockscreen_poweroff = value;
    app->save_settings = true;
}

static void momentum_app_scene_interface_lockscreen_lockscreen_time_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    momentum_settings.lockscreen_time = value;
    app->save_settings = true;
}

static void
    momentum_app_scene_interface_lockscreen_lockscreen_seconds_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    momentum_settings.lockscreen_seconds = value;
    app->save_settings = true;
}

static void momentum_app_scene_interface_lockscreen_lockscreen_date_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    momentum_settings.lockscreen_date = value;
    app->save_settings = true;
}

static void
    momentum_app_scene_interface_lockscreen_lockscreen_statusbar_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    momentum_settings.lockscreen_statusbar = value;
    app->save_settings = true;
}

static void momentum_app_scene_interface_lockscreen_lockscreen_prompt_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    momentum_settings.lockscreen_prompt = value;
    app->save_settings = true;
}

static void
    momentum_app_scene_interface_lockscreen_lockscreen_transparent_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    momentum_settings.lockscreen_transparent = value;
    app->save_settings = true;
}

static void
    momentum_app_scene_interface_lockscreen_lockscreen_skip_animation_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    momentum_settings.lockscreen_skip_animation = value;
    app->save_settings = true;
}

void momentum_app_scene_interface_lockscreen_on_enter(void* context) {
    MomentumApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Lock on Boot if PIN set", "开机自动锁定 (已设 PIN)"),
        2,
        momentum_app_scene_interface_lockscreen_lock_on_boot_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.lock_on_boot);
    variable_item_set_current_value_text(item, momentum_settings.lock_on_boot ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Format on 10 Bad PINs", "10 次错误 PIN 后格式化"),
        2,
        momentum_app_scene_interface_lockscreen_bad_pins_format_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.bad_pins_format);
    variable_item_set_current_value_text(item, momentum_settings.bad_pins_format ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Allow USB RPC While Locked", "锁定时允许 USB RPC"),
        2,
        momentum_app_scene_interface_lockscreen_allow_locked_rpc_usb_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.allow_locked_rpc_usb);
    variable_item_set_current_value_text(
        item, momentum_settings.allow_locked_rpc_usb ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Allow BLE RPC While Locked", "锁定时允许 BLE RPC"),
        2,
        momentum_app_scene_interface_lockscreen_allow_locked_rpc_ble_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.allow_locked_rpc_ble);
    variable_item_set_current_value_text(
        item, momentum_settings.allow_locked_rpc_ble ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Allow Poweroff", "允许关机"),
        2,
        momentum_app_scene_interface_lockscreen_lockscreen_poweroff_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.lockscreen_poweroff);
    variable_item_set_current_value_text(
        item, momentum_settings.lockscreen_poweroff ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Show Time", "显示时间"),
        2,
        momentum_app_scene_interface_lockscreen_lockscreen_time_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.lockscreen_time);
    variable_item_set_current_value_text(item, momentum_settings.lockscreen_time ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Show Seconds", "显示秒数"),
        2,
        momentum_app_scene_interface_lockscreen_lockscreen_seconds_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.lockscreen_seconds);
    variable_item_set_current_value_text(
        item, momentum_settings.lockscreen_seconds ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Show Date", "显示日期"),
        2,
        momentum_app_scene_interface_lockscreen_lockscreen_date_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.lockscreen_date);
    variable_item_set_current_value_text(item, momentum_settings.lockscreen_date ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Show Statusbar", "显示状态栏"),
        2,
        momentum_app_scene_interface_lockscreen_lockscreen_statusbar_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.lockscreen_statusbar);
    variable_item_set_current_value_text(
        item, momentum_settings.lockscreen_statusbar ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Unlock Prompt", "解锁提示"),
        2,
        momentum_app_scene_interface_lockscreen_lockscreen_prompt_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.lockscreen_prompt);
    variable_item_set_current_value_text(item, momentum_settings.lockscreen_prompt ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Transparent (see animation)", "透明 (显示动画)"),
        2,
        momentum_app_scene_interface_lockscreen_lockscreen_transparent_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.lockscreen_transparent);
    variable_item_set_current_value_text(
        item, momentum_settings.lockscreen_transparent ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Skip Sliding Animation", "跳过滑动动画"),
        2,
        momentum_app_scene_interface_lockscreen_lockscreen_skip_animation_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.lockscreen_skip_animation);
    variable_item_set_current_value_text(
        item, momentum_settings.lockscreen_skip_animation ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    variable_item_list_set_enter_callback(
        var_item_list, momentum_app_scene_interface_lockscreen_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, MomentumAppSceneInterfaceLockscreen));

    view_dispatcher_switch_to_view(app->view_dispatcher, MomentumAppViewVarItemList);
}

bool momentum_app_scene_interface_lockscreen_on_event(void* context, SceneManagerEvent event) {
    MomentumApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, MomentumAppSceneInterfaceLockscreen, event.event);
        consumed = true;
        switch(event.event) {
        default:
            break;
        }
    }

    return consumed;
}

void momentum_app_scene_interface_lockscreen_on_exit(void* context) {
    MomentumApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
