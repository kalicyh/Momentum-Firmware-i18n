#include "../infrared_app_i.h"

typedef enum {
    SubmenuIndexUniversalTV,
    SubmenuIndexUniversalTVExtra,
    SubmenuIndexUniversalProjector,
    SubmenuIndexUniversalProjectorExtra,
    SubmenuIndexUniversalAudio,
    SubmenuIndexUniversalAudioExtra,
    SubmenuIndexUniversalSquareAudio,
    SubmenuIndexUniversalAirConditioner,
    SubmenuIndexUniversalLEDs,
    SubmenuIndexUniversalFan,
    SubmenuIndexUniversalFanExtra,
    SubmenuIndexUniversalBluray,
    SubmenuIndexUniversalMonitor,
    SubmenuIndexUniversalDigitalSign,
    SubmenuIndexUniversalFromFile,
} SubmenuIndex;

static void infrared_scene_universal_submenu_callback(void* context, uint32_t index) {
    InfraredApp* infrared = context;
    view_dispatcher_send_custom_event(infrared->view_dispatcher, index);
}

void infrared_scene_universal_on_enter(void* context) {
    InfraredApp* infrared = context;
    Submenu* submenu = infrared->submenu;

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("TVs", "电视"),
        SubmenuIndexUniversalTV,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("TVs+", "电视补充"),
        SubmenuIndexUniversalTVExtra,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("Projectors", "投影仪"),
        SubmenuIndexUniversalProjector,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("Projectors+", "投影补充"),
        SubmenuIndexUniversalProjectorExtra,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("Audio", "音响"),
        SubmenuIndexUniversalAudio,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("Audio+", "音响补充"),
        SubmenuIndexUniversalAudioExtra,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("Square Audio", "广场音响"),
        SubmenuIndexUniversalSquareAudio,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("ACs", "空调"),
        SubmenuIndexUniversalAirConditioner,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("LEDs", "灯带"),
        SubmenuIndexUniversalLEDs,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("Fans", "风扇"),
        SubmenuIndexUniversalFan,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("Fans+", "风扇补充"),
        SubmenuIndexUniversalFanExtra,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("Blu-ray/DVDs", "蓝光/DVD"),
        SubmenuIndexUniversalBluray,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("Monitors", "显示器"),
        SubmenuIndexUniversalMonitor,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("Digital Signs", "数字标牌"),
        SubmenuIndexUniversalDigitalSign,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_add_item(
        submenu,
        INFRARED_UI_TEXT("Load from Library File", "从库文件载入"),
        SubmenuIndexUniversalFromFile,
        infrared_scene_universal_submenu_callback,
        context);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(infrared->scene_manager, InfraredSceneUniversal));

    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewSubmenu);
}

bool infrared_scene_universal_on_event(void* context, SceneManagerEvent event) {
    InfraredApp* infrared = context;
    SceneManager* scene_manager = infrared->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexUniversalTV) {
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalTV);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalTVExtra) {
            furi_string_set(infrared->universal_db_path, EXT_PATH("infrared/assets/tv_extra.ir"));
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalFromFile);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalProjector) {
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalProjector);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalProjectorExtra) {
            furi_string_set(
                infrared->universal_db_path, EXT_PATH("infrared/assets/projectors_extra.ir"));
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalFromFile);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalAudio) {
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalAudio);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalAudioExtra) {
            furi_string_set(
                infrared->universal_db_path, EXT_PATH("infrared/assets/audio_extra.ir"));
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalFromFile);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalSquareAudio) {
            furi_string_set(
                infrared->universal_db_path, EXT_PATH("infrared/assets/square_audio.ir"));
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalFromFile);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalAirConditioner) {
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalAC);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalLEDs) {
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalLEDs);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalFan) {
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalFan);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalFanExtra) {
            furi_string_set(infrared->universal_db_path, EXT_PATH("infrared/assets/fans_extra.ir"));
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalFromFile);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalBluray) {
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalBluray);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalMonitor) {
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalMonitor);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalDigitalSign) {
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalDigitalSign);
            consumed = true;
        } else if(event.event == SubmenuIndexUniversalFromFile) {
            scene_manager_next_scene(scene_manager, InfraredSceneUniversalFromFile);
            consumed = true;
        }
        scene_manager_set_scene_state(scene_manager, InfraredSceneUniversal, event.event);
    }

    return consumed;
}

void infrared_scene_universal_on_exit(void* context) {
    InfraredApp* infrared = context;
    submenu_reset(infrared->submenu);
}
