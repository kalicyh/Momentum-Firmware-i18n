#include "../nfc_app_i.h"

enum SubmenuIndex {
    SubmenuIndexReadCardType,
    SubmenuIndexMfClassicKeys,
    SubmenuIndexMfUltralightCKeys,
    SubmenuIndexMfUltralightUnlock,
    SubmenuIndexSlixUnlock,
};

void nfc_scene_extra_actions_submenu_callback(void* context, uint32_t index) {
    NfcApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void nfc_scene_extra_actions_on_enter(void* context) {
    NfcApp* instance = context;
    Submenu* submenu = instance->submenu;

    submenu_add_item(
        submenu,
        NFC_UI_TEXT("Read Specific Card Type", "读取指定卡片类型"),
        SubmenuIndexReadCardType,
        nfc_scene_extra_actions_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        NFC_UI_TEXT("MIFARE Classic Keys", "MIFARE Classic 密钥"),
        SubmenuIndexMfClassicKeys,
        nfc_scene_extra_actions_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        NFC_UI_TEXT("MIFARE Ultralight C Keys", "MIFARE Ultralight C 密钥"),
        SubmenuIndexMfUltralightCKeys,
        nfc_scene_extra_actions_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        NFC_UI_TEXT("Unlock NTAG/Ultralight", "解锁 NTAG/Ultralight"),
        SubmenuIndexMfUltralightUnlock,
        nfc_scene_extra_actions_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        NFC_UI_TEXT("Unlock SLIX-L", "解锁 SLIX-L"),
        SubmenuIndexSlixUnlock,
        nfc_scene_extra_actions_submenu_callback,
        instance);
    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(instance->scene_manager, NfcSceneExtraActions));
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_extra_actions_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexMfClassicKeys) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneMfClassicKeys);
            consumed = true;
        } else if(event.event == SubmenuIndexMfUltralightCKeys) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneMfUltralightCKeys);
            consumed = true;
        } else if(event.event == SubmenuIndexMfUltralightUnlock) {
            mf_ultralight_auth_reset(instance->mf_ul_auth);
            scene_manager_next_scene(instance->scene_manager, NfcSceneMfUltralightUnlockMenu);
            consumed = true;
        } else if(event.event == SubmenuIndexReadCardType) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneSelectProtocol);
            consumed = true;
        } else if(event.event == SubmenuIndexSlixUnlock) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneSlixUnlockMenu);
            consumed = true;
        }
        scene_manager_set_scene_state(instance->scene_manager, NfcSceneExtraActions, event.event);
    }

    return consumed;
}

void nfc_scene_extra_actions_on_exit(void* context) {
    NfcApp* instance = context;

    submenu_reset(instance->submenu);
}
