#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

void nfc_scene_mf_ultralight_unlock_warn_dialog_callback(DialogExResult result, void* context) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
}

void nfc_scene_mf_ultralight_unlock_warn_on_enter(void* context) {
    NfcApp* nfc = context;
    DialogEx* dialog_ex = nfc->dialog_ex;

    dialog_ex_set_context(dialog_ex, nfc);
    dialog_ex_set_result_callback(dialog_ex, nfc_scene_mf_ultralight_unlock_warn_dialog_callback);

    MfUltralightAuthType type = nfc->mf_ul_auth->type;
    if((type == MfUltralightAuthTypeReader) || (type == MfUltralightAuthTypeManual)) {
        // Build dialog text
        FuriString* password_str =
            furi_string_alloc_set_str(NFC_UI_TEXT("Try to unlock the card with\npassword: ", "尝试使用以下密码\n解锁卡片: "));
        for(size_t i = 0; i < sizeof(nfc->mf_ul_auth->password.data); i++) {
            furi_string_cat_printf(password_str, "%02X ", nfc->mf_ul_auth->password.data[i]);
        }
        furi_string_cat_str(password_str, NFC_UI_TEXT("\nWarning: incorrect password\nwill block the card!", "\n警告: 密码错误\n会锁死卡片!"));
        nfc_text_store_set(nfc, furi_string_get_cstr(password_str));
        furi_string_free(password_str);

        const char* message = (type == MfUltralightAuthTypeReader) ?
                                  NFC_UI_TEXT("Password Captured!", "密码已捕获!") :
                                  NFC_UI_TEXT("Risky Action!", "危险操作!");
        dialog_ex_set_header(dialog_ex, message, 64, 0, AlignCenter, AlignTop);
        dialog_ex_set_text(dialog_ex, nfc->text_store, 64, 10, AlignCenter, AlignTop);
        dialog_ex_set_left_button_text(dialog_ex, NFC_UI_TEXT("Cancel", "取消"));
        dialog_ex_set_right_button_text(dialog_ex, NFC_UI_TEXT("Continue", "继续"));

        if(type == MfUltralightAuthTypeReader) {
            notification_message(nfc->notifications, &sequence_set_green_255);
        }
    } else {
        dialog_ex_set_header(dialog_ex, NFC_UI_TEXT("Risky action!", "危险操作!"), 64, 4, AlignCenter, AlignTop);
        dialog_ex_set_text(
            dialog_ex,
            NFC_UI_TEXT("Wrong password\ncan block your\ncard.", "错误密码\n可能锁死\n卡片。"),
            4,
            18,
            AlignLeft,
            AlignTop);
        dialog_ex_set_icon(dialog_ex, 83, 22, &I_WarningDolphinFlip_45x42);
        dialog_ex_set_center_button_text(dialog_ex, NFC_UI_TEXT("OK", "确定"));
    }

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewDialogEx);
}

bool nfc_scene_mf_ultralight_unlock_warn_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;

    bool consumed = false;

    MfUltralightAuthType type = nfc->mf_ul_auth->type;
    if((type == MfUltralightAuthTypeReader) || (type == MfUltralightAuthTypeManual)) {
        if(event.type == SceneManagerEventTypeCustom) {
            if(event.event == DialogExResultRight) {
                const NfcProtocol mfu_protocol[] = {NfcProtocolMfUltralight};
                nfc_detected_protocols_set(
                    nfc->detected_protocols, mfu_protocol, COUNT_OF(mfu_protocol));
                scene_manager_next_scene(nfc->scene_manager, NfcSceneRead);
                dolphin_deed(DolphinDeedNfcRead);
                consumed = true;
            } else if(event.event == DialogExResultLeft) {
                if(type == MfUltralightAuthTypeReader) {
                    consumed = scene_manager_search_and_switch_to_previous_scene(
                        nfc->scene_manager, NfcSceneMfUltralightUnlockMenu);
                } else {
                    consumed = scene_manager_previous_scene(nfc->scene_manager);
                }
            }
        } else if(event.type == SceneManagerEventTypeBack) {
            // Cannot press back
            consumed = true;
        }
    } else {
        if(event.type == SceneManagerEventTypeCustom) {
            if(event.event == DialogExResultCenter) {
                const NfcProtocol mfu_protocol[] = {NfcProtocolMfUltralight};
                nfc_detected_protocols_set(
                    nfc->detected_protocols, mfu_protocol, COUNT_OF(mfu_protocol));
                scene_manager_next_scene(nfc->scene_manager, NfcSceneRead);
                dolphin_deed(DolphinDeedNfcRead);
                consumed = true;
            }
        }
    }

    return consumed;
}

void nfc_scene_mf_ultralight_unlock_warn_on_exit(void* context) {
    NfcApp* nfc = context;

    dialog_ex_reset(nfc->dialog_ex);
    nfc_text_store_clear(nfc);

    notification_message_block(nfc->notifications, &sequence_reset_green);
}
