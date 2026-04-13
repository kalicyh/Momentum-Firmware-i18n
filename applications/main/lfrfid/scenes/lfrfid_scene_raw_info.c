#include "../lfrfid_i.h"

#ifdef MOMENTUM_UI_LANG_ZH_CN
#define LFRFID_INFO_HEADER_Y 3
#define LFRFID_INFO_TEXT_Y   16
#define LFRFID_INFO_BOX_Y    3
#define LFRFID_INFO_BOX_H    61
#else
#define LFRFID_INFO_HEADER_Y 0
#define LFRFID_INFO_TEXT_Y   13
#define LFRFID_INFO_BOX_Y    0
#define LFRFID_INFO_BOX_H    64
#endif

void lfrfid_scene_raw_info_on_enter(void* context) {
    LfRfid* app = context;
    Widget* widget = app->widget;

    if(storage_sd_status(app->storage) != FSE_OK) {
        widget_add_icon_element(widget, 83, 22, &I_WarningDolphinFlip_45x42);
        widget_add_string_element(
            widget,
            64,
            LFRFID_INFO_HEADER_Y,
            AlignCenter,
            AlignTop,
            FontPrimary,
            LFRFID_UI_TEXT("No SD Card!", "没有 SD 卡!"));
        widget_add_string_multiline_element(
            widget,
            0,
            LFRFID_INFO_TEXT_Y,
            AlignLeft,
            AlignTop,
            FontSecondary,
            LFRFID_UI_TEXT("Insert an SD card\nto use this function", "插入 SD 卡\n以使用此功能"));

    } else {
        widget_add_text_box_element(
            widget,
            0,
            LFRFID_INFO_BOX_Y,
            128,
            LFRFID_INFO_BOX_H,
            AlignLeft,
            AlignTop,
            LFRFID_UI_TEXT(
                "\e#RAW RFID Data Reader\e#\n"
                "1. Hold card next to Flipper\n"
                "2. Press OK\n"
                "3. Wait until data is read",
                "\e#RAW RFID 数据读取器\e#\n"
                "1. 将卡片靠近 Flipper\n"
                "2. 按下确定\n"
                "3. 等待读取完成"),
            false);

        widget_add_button_element(
            widget, GuiButtonTypeCenter, LFRFID_UI_TEXT("OK", "确定"), lfrfid_widget_callback, app);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewWidget);
}

bool lfrfid_scene_raw_info_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    SceneManager* scene_manager = app->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
        scene_manager_search_and_switch_to_previous_scene(scene_manager, LfRfidSceneExtraActions);
    } else if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == GuiButtonTypeCenter) {
            scene_manager_next_scene(scene_manager, LfRfidSceneRawRead);
        } else if(event.event == GuiButtonTypeLeft) {
            scene_manager_search_and_switch_to_previous_scene(
                scene_manager, LfRfidSceneExtraActions);
        }
    }

    return consumed;
}

void lfrfid_scene_raw_info_on_exit(void* context) {
    LfRfid* app = context;
    widget_reset(app->widget);
}
