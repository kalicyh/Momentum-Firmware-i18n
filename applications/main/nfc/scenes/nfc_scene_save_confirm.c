#include "../nfc_app_i.h"

void nfc_scene_save_confirm_dialog_callback(DialogExResult result, void* context) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
}

void nfc_scene_save_confirm_on_enter(void* context) {
    NfcApp* nfc = context;
    DialogEx* dialog_ex = nfc->dialog_ex;

    dialog_ex_set_left_button_text(dialog_ex, NFC_UI_TEXT("Skip", "跳过"));
    dialog_ex_set_right_button_text(dialog_ex, NFC_UI_TEXT("Save", "保存"));
    dialog_ex_set_header(
        dialog_ex, NFC_UI_TEXT("Save the Key?", "保存密钥?"), 64, 0, AlignCenter, AlignTop);
    dialog_ex_set_text(
        dialog_ex,
        NFC_UI_TEXT("All unsaved data will be lost", "所有未保存的数据都会丢失"),
        64,
        12,
        AlignCenter,
        AlignTop);
    dialog_ex_set_context(dialog_ex, nfc);
    dialog_ex_set_result_callback(dialog_ex, nfc_scene_save_confirm_dialog_callback);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewDialogEx);
}

bool nfc_scene_save_confirm_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == DialogExResultRight) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneSaveName);
            consumed = true;
        } else if(event.event == DialogExResultLeft) {
            NfcSceneSaveConfirmState scene_state =
                scene_manager_get_scene_state(nfc->scene_manager, NfcSceneSaveConfirm);

            NfcScene scene = scene_state == NfcSceneSaveConfirmStateCrackNonces ?
                                 NfcSceneMfClassicMfkeyComplete :
                                 NfcSceneMfClassicDetectReader;

            scene_manager_next_scene(nfc->scene_manager, scene);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_save_confirm_on_exit(void* context) {
    NfcApp* nfc = context;

    // Clean view
    dialog_ex_reset(nfc->dialog_ex);
}
