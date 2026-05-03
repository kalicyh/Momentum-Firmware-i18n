#include "slix_render.h"
#include "nfc/nfc_app_i.h"

void nfc_render_slix_info(const SlixData* data, NfcProtocolFormatType format_type, FuriString* str) {
    nfc_render_iso15693_3_brief(slix_get_base_data(data), str);

    if(format_type != NfcProtocolFormatTypeFull) return;
    const SlixType slix_type = slix_get_type(data);

    furi_string_cat(str, "\n::::::::::::::::::[Passwords]:::::::::::::::::\n");

    static const char* slix_password_names[] = {
        NFC_UI_TEXT("Read", "读取"),
        NFC_UI_TEXT("Write", "写入"),
        NFC_UI_TEXT("Privacy", "隐私"),
        NFC_UI_TEXT("Destroy", "销毁"),
        "EAS/AFI",
    };

    for(uint32_t i = 0; i < SlixPasswordTypeCount; ++i) {
        if(slix_type_supports_password(slix_type, i)) {
            furi_string_cat_printf(
                str, "%s :  %08lX\n", slix_password_names[i], data->passwords[i]);
        }
    }

    furi_string_cat(str, ":::::::::::::::::::[Lock bits]::::::::::::::::::::\n");

    if(slix_type_has_features(slix_type, SLIX_TYPE_FEATURE_EAS)) {
        furi_string_cat_printf(
            str,
            "EAS: %s\n",
            data->system_info.lock_bits.eas ? NFC_UI_TEXT("locked", "已锁定") :
                                              NFC_UI_TEXT("not locked", "未锁定"));
    }

    if(slix_type_has_features(slix_type, SLIX_TYPE_FEATURE_PROTECTION)) {
        furi_string_cat_printf(
            str,
            "PPL: %s\n",
            data->system_info.lock_bits.ppl ? NFC_UI_TEXT("locked", "已锁定") :
                                              NFC_UI_TEXT("not locked", "未锁定"));

        const SlixProtection protection = data->system_info.protection;

        furi_string_cat(str, "::::::::::::[Page protection]::::::::::::\n");
        furi_string_cat_printf(str, "Pointer: H >= %02X\n", protection.pointer);

        const char* rh = (protection.condition & SLIX_PP_CONDITION_RH) ? "" : "un";
        const char* rl = (protection.condition & SLIX_PP_CONDITION_RL) ? "" : "un";

        const char* wh = (protection.condition & SLIX_PP_CONDITION_WH) ? "" : "un";
        const char* wl = (protection.condition & SLIX_PP_CONDITION_WL) ? "" : "un";

        furi_string_cat_printf(str, "R:  H %sprotec. L %sprotec.\n", rh, rl);
        furi_string_cat_printf(str, "W: H %sprotec. L %sprotec.\n", wh, wl);
    }

    if(slix_type_has_features(slix_type, SLIX_TYPE_FEATURE_PRIVACY)) {
        furi_string_cat(str, "::::::::::::::::::::[Privacy]::::::::::::::::::::::\n");
        furi_string_cat_printf(
            str,
            NFC_UI_TEXT("Privacy mode: %s\n", "隐私模式: %s\n"),
            data->privacy ? NFC_UI_TEXT("enabled", "已启用") : NFC_UI_TEXT("disabled", "已禁用"));
    }

    if(slix_type_has_features(slix_type, SLIX_TYPE_FEATURE_SIGNATURE)) {
        furi_string_cat(str, ":::::::::::::::::::[Signature]::::::::::::::::::\n");
        for(uint32_t i = 0; i < 4; ++i) {
            furi_string_cat_printf(str, "%02X ", data->signature[i]);
        }

        furi_string_cat(str, "[ ... ]");

        for(uint32_t i = 0; i < 3; ++i) {
            furi_string_cat_printf(str, " %02X", data->signature[sizeof(SlixSignature) - i - 1]);
        }
    }

    furi_string_cat(str, "\n\e#ISO15693-3 data");
    nfc_render_iso15693_3_extra(slix_get_base_data(data), str);
}
