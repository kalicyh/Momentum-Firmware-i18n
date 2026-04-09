#include "../lfrfid_i.h"

void lfrfid_scene_exit_confirm_on_enter(void* context) {
    LfRfid* app = context;
    Widget* widget = app->widget;

    widget_add_button_element(
        widget, GuiButtonTypeLeft, LFRFID_UI_TEXT("Exit", "退出"), lfrfid_widget_callback, app);
    widget_add_button_element(
        widget, GuiButtonTypeRight, LFRFID_UI_TEXT("Stay", "停留"), lfrfid_widget_callback, app);
    widget_add_string_element(
        widget,
        64,
        0,
        AlignCenter,
        AlignTop,
        FontPrimary,
        LFRFID_UI_TEXT("Exit to RFID Menu?", "退出到 RFID 菜单?"));
    widget_add_string_element(
        widget,
        64,
        13,
        AlignCenter,
        AlignTop,
        FontSecondary,
        LFRFID_UI_TEXT("All unsaved data will be lost", "所有未保存数据都会丢失"));

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewWidget);
}

bool lfrfid_scene_exit_confirm_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    SceneManager* scene_manager = app->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        consumed = true; // Ignore Back button presses
    } else if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_search_and_switch_to_previous_scene(scene_manager, LfRfidSceneStart);
        } else if(event.event == GuiButtonTypeRight) {
            scene_manager_previous_scene(scene_manager);
        }
    }

    return consumed;
}

void lfrfid_scene_exit_confirm_on_exit(void* context) {
    LfRfid* app = context;
    widget_reset(app->widget);
}
