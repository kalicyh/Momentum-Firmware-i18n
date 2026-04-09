#include "../momentum_app.h"

enum VarItemListIndex {
    VarItemListIndexSubghzFreqs,
    VarItemListIndexSubghzBypass,
    VarItemListIndexSubghzExtend,
    VarItemListIndexGpioPins,
    VarItemListIndexFileNamingPrefix,
};

void momentum_app_scene_protocols_var_item_list_callback(void* context, uint32_t index) {
    MomentumApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void momentum_app_scene_protocols_subghz_bypass_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    view_dispatcher_send_custom_event(app->view_dispatcher, VarItemListIndexSubghzBypass);
}

static void momentum_app_scene_protocols_subghz_extend_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    view_dispatcher_send_custom_event(app->view_dispatcher, VarItemListIndexSubghzExtend);
}

static void momentum_app_scene_protocols_file_naming_prefix_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(
        item, value ? MOMENTUM_UI_TEXT("After", "后置") : MOMENTUM_UI_TEXT("Before", "前置"));
    momentum_settings.file_naming_prefix_after = value;
    app->save_settings = true;
}

void momentum_app_scene_protocols_on_enter(void* context) {
    MomentumApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item =
        variable_item_list_add(var_item_list, MOMENTUM_UI_TEXT("SubGHz Freqs", "SubGHz 频率"), 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("SubGHz Bypass Region Lock", "SubGHz 绕过区域锁"),
        2,
        momentum_app_scene_protocols_subghz_bypass_changed,
        app);
    variable_item_set_current_value_index(item, app->subghz_bypass);
    variable_item_set_current_value_text(
        item, app->subghz_bypass ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("SubGHz Extend Freq Bands", "SubGHz 扩展频段"),
        2,
        momentum_app_scene_protocols_subghz_extend_changed,
        app);
    variable_item_set_current_value_index(item, app->subghz_extend);
    variable_item_set_current_value_text(
        item, app->subghz_extend ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
    variable_item_set_locked(
        item,
        !app->subghz_bypass,
        MOMENTUM_UI_TEXT("Must bypass\nregion lock\nfirst!", "必须先\n绕过区域锁!"));

    item = variable_item_list_add(var_item_list, MOMENTUM_UI_TEXT("GPIO Pins", "GPIO 引脚"), 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("File Naming Prefix", "文件命名前缀"),
        2,
        momentum_app_scene_protocols_file_naming_prefix_changed,
        app);
    variable_item_set_current_value_index(item, momentum_settings.file_naming_prefix_after);
    variable_item_set_current_value_text(
        item,
        momentum_settings.file_naming_prefix_after ? MOMENTUM_UI_TEXT("After", "后置") :
                                                     MOMENTUM_UI_TEXT("Before", "前置"));

    variable_item_list_set_enter_callback(
        var_item_list, momentum_app_scene_protocols_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, MomentumAppSceneProtocols));

    view_dispatcher_switch_to_view(app->view_dispatcher, MomentumAppViewVarItemList);
}

bool momentum_app_scene_protocols_on_event(void* context, SceneManagerEvent event) {
    MomentumApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, MomentumAppSceneProtocols, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexSubghzFreqs:
            scene_manager_set_scene_state(app->scene_manager, MomentumAppSceneProtocolsFreqs, 0);
            scene_manager_next_scene(app->scene_manager, MomentumAppSceneProtocolsFreqs);
            break;
        case VarItemListIndexSubghzBypass:
        case VarItemListIndexSubghzExtend: {
            bool* setting = (event.event == VarItemListIndexSubghzBypass) ? &app->subghz_bypass :
                                                                            &app->subghz_extend;
            VariableItem* item = variable_item_list_get(app->var_item_list, event.event);
            bool value = variable_item_get_current_value_index(item);
            if(value == *setting) value = !value; // Invoked via click
            bool change = !value; // Change without confirm if going from ON to OFF
            if(value) {
                DialogMessage* msg = dialog_message_alloc();
                dialog_message_set_header(
                    msg, MOMENTUM_UI_TEXT("Are you sure?", "确定吗?"), 64, 4, AlignCenter, AlignTop);
                dialog_message_set_buttons(
                    msg, MOMENTUM_UI_TEXT("No", "否"), NULL, MOMENTUM_UI_TEXT("Yes", "是"));
                dialog_message_set_text(
                    msg,
                    (event.event == VarItemListIndexSubghzBypass) ?
                        MOMENTUM_UI_TEXT(
                            "Unlocks TX to 300-350,\n"
                            "387-467, 779-928 MHz\n"
                            "Use responsibly, check\n"
                            "local laws",
                            "解锁 300-350,\n"
                            "387-467, 779-928 MHz\n"
                            "请谨慎使用并遵守\n"
                            "当地法律") :
                        MOMENTUM_UI_TEXT(
                            "Extends TX to 281-361,\n"
                            "378-481, 749-962 MHz\n"
                            "Use at own risk, may\n"
                            "damage Flipper",
                            "扩展到 281-361,\n"
                            "378-481, 749-962 MHz\n"
                            "风险自负, 可能会\n"
                            "损坏 Flipper"),
                    64,
                    36,
                    AlignCenter,
                    AlignCenter);
                if(dialog_message_show(app->dialogs, msg) == DialogMessageButtonRight) {
                    change = true;
                }
                dialog_message_free(msg);
            }
            if(change) {
                *setting = value;
                app->save_subghz = true;
                app->require_reboot = true;
                if(event.event == VarItemListIndexSubghzBypass) {
                    variable_item_set_locked(
                        variable_item_list_get(app->var_item_list, VarItemListIndexSubghzExtend),
                        !value,
                        NULL);
                }
            } else {
                value = !value;
            }
            variable_item_set_current_value_index(item, value);
            variable_item_set_current_value_text(
                item, value ? MOMENTUM_UI_TEXT("ON", "开") : MOMENTUM_UI_TEXT("OFF", "关"));
            break;
        }
        case VarItemListIndexGpioPins:
            scene_manager_set_scene_state(app->scene_manager, MomentumAppSceneProtocolsGpio, 0);
            scene_manager_next_scene(app->scene_manager, MomentumAppSceneProtocolsGpio);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void momentum_app_scene_protocols_on_exit(void* context) {
    MomentumApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
