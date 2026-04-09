#include "../momentum_app.h"

enum VarItemListIndex {
    VarItemListIndexForeground,
    VarItemListIndexBackground,
};

void momentum_app_scene_misc_vgm_var_item_list_callback(void* context, uint32_t index) {
    MomentumApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static const struct {
    char* name;
    RgbColor color;
} vgm_colors[] = {
    // clang-format off
    {MOMENTUM_UI_TEXT("Default", "默认"), {{0, 0, 0}}},
    {MOMENTUM_UI_TEXT("Rainbow", "彩虹"), {{0, 0, 0}}},
    {MOMENTUM_UI_TEXT("RgbMod", "RGB 模组"), {{0, 0, 0}}},
    {MOMENTUM_UI_TEXT("Black", "黑色"), {{0, 0, 0}}},
    {MOMENTUM_UI_TEXT("Orange", "橙色"), {{255, 130, 0}}},
    {MOMENTUM_UI_TEXT("Red", "红色"), {{255, 0, 0}}},
    {MOMENTUM_UI_TEXT("Maroon", "栗色"), {{128, 0, 0}}},
    {MOMENTUM_UI_TEXT("Yellow", "黄色"), {{255, 255, 0}}},
    {MOMENTUM_UI_TEXT("Olive", "橄榄"), {{128, 128, 0}}},
    {MOMENTUM_UI_TEXT("Lime", "青柠"), {{0, 255, 0}}},
    {MOMENTUM_UI_TEXT("Green", "绿色"), {{0, 128, 0}}},
    {MOMENTUM_UI_TEXT("Aqua", "水绿"), {{0, 255, 127}}},
    {MOMENTUM_UI_TEXT("Cyan", "青色"), {{0, 210, 210}}},
    {MOMENTUM_UI_TEXT("Azure", "天蓝"), {{0, 127, 255}}},
    {MOMENTUM_UI_TEXT("Teal", "蓝绿"), {{0, 128, 128}}},
    {MOMENTUM_UI_TEXT("Blue", "蓝色"), {{0, 0, 255}}},
    {MOMENTUM_UI_TEXT("Navy", "海军蓝"), {{0, 0, 128}}},
    {MOMENTUM_UI_TEXT("Purple", "紫色"), {{128, 0, 128}}},
    {MOMENTUM_UI_TEXT("Fuchsia", "品红"), {{255, 0, 255}}},
    {MOMENTUM_UI_TEXT("Pink", "粉色"), {{173, 31, 173}}},
    {MOMENTUM_UI_TEXT("Brown", "棕色"), {{165, 42, 42}}},
    {MOMENTUM_UI_TEXT("White", "白色"), {{255, 192, 203}}},
    // clang-format on
};

static const size_t vgm_colors_count = COUNT_OF(vgm_colors);

static void momentum_app_scene_misc_vgm_foreground_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, vgm_colors[index].name);
    momentum_settings.rpc_color_fg.rgb = vgm_colors[index].color;

    if(index == 0) {
        momentum_settings.rpc_color_fg.mode = ScreenColorModeDefault;
    } else if(index == 1) {
        momentum_settings.rpc_color_fg.mode = ScreenColorModeRainbow;
    } else if(index == 2) {
        momentum_settings.rpc_color_fg.mode = ScreenColorModeRgbBacklight;
    } else {
        momentum_settings.rpc_color_fg.mode = ScreenColorModeCustom;
    }

    app->save_settings = true;
}

static void momentum_app_scene_misc_vgm_background_changed(VariableItem* item) {
    MomentumApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, vgm_colors[index].name);
    momentum_settings.rpc_color_bg.rgb = vgm_colors[index].color;

    if(index == 0) {
        momentum_settings.rpc_color_bg.mode = ScreenColorModeDefault;
    } else if(index == 1) {
        momentum_settings.rpc_color_bg.mode = ScreenColorModeRainbow;
    } else if(index == 2) {
        momentum_settings.rpc_color_bg.mode = ScreenColorModeRgbBacklight;
    } else {
        momentum_settings.rpc_color_bg.mode = ScreenColorModeCustom;
    }

    app->save_settings = true;
}

void momentum_app_scene_misc_vgm_on_enter(void* context) {
    MomentumApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Foreground", "前景色"),
        vgm_colors_count,
        momentum_app_scene_misc_vgm_foreground_changed,
        app);
    ScreenFrameColor color = momentum_settings.rpc_color_fg;
    bool found = true;
    if(color.mode == ScreenColorModeRainbow) {
        value_index = 1;
    } else if(color.mode == ScreenColorModeRgbBacklight) {
        value_index = 2;
    } else if(color.mode == ScreenColorModeCustom) {
        found = false;
        for(size_t i = 3; i < vgm_colors_count; i++) {
            if(rgbcmp(&color.rgb, &vgm_colors[i].color) != 0) continue;
            value_index = i;
            found = true;
            break;
        }
    } else {
        value_index = 0;
    }

    variable_item_set_current_value_index(item, found ? value_index : 0);
    if(found) {
        variable_item_set_current_value_text(item, vgm_colors[value_index].name);
    } else {
        char str[7];
        snprintf(str, sizeof(str), "%02X%02X%02X", color.rgb.r, color.rgb.g, color.rgb.b);
        variable_item_set_current_value_text(item, str);
    }

    item = variable_item_list_add(
        var_item_list,
        MOMENTUM_UI_TEXT("Background", "背景色"),
        vgm_colors_count,
        momentum_app_scene_misc_vgm_background_changed,
        app);
    color = momentum_settings.rpc_color_bg;
    found = true;
    if(color.mode == ScreenColorModeRainbow) {
        value_index = 1;
    } else if(color.mode == ScreenColorModeRgbBacklight) {
        value_index = 2;
    } else if(color.mode == ScreenColorModeCustom) {
        found = false;
        for(size_t i = 3; i < vgm_colors_count; i++) {
            if(rgbcmp(&color.rgb, &vgm_colors[i].color) != 0) continue;
            value_index = i;
            found = true;
            break;
        }
    } else {
        value_index = 0;
    }

    variable_item_set_current_value_index(item, found ? value_index : 0);
    if(found) {
        variable_item_set_current_value_text(item, vgm_colors[value_index].name);
    } else {
        char str[7];
        snprintf(str, sizeof(str), "%02X%02X%02X", color.rgb.r, color.rgb.g, color.rgb.b);
        variable_item_set_current_value_text(item, str);
    }

    variable_item_list_set_enter_callback(
        var_item_list, momentum_app_scene_misc_vgm_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, MomentumAppSceneMiscVgm));

    view_dispatcher_switch_to_view(app->view_dispatcher, MomentumAppViewVarItemList);
}

bool momentum_app_scene_misc_vgm_on_event(void* context, SceneManagerEvent event) {
    MomentumApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, MomentumAppSceneMiscVgm, event.event);
        consumed = true;

        switch(event.event) {
        case VarItemListIndexForeground:
        case VarItemListIndexBackground:
            scene_manager_set_scene_state(
                app->scene_manager,
                MomentumAppSceneMiscVgmColor,
                event.event - VarItemListIndexForeground);
            scene_manager_next_scene(app->scene_manager, MomentumAppSceneMiscVgmColor);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void momentum_app_scene_misc_vgm_on_exit(void* context) {
    MomentumApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
