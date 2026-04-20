#include "type_4_tag_render.h"

#include "nfc/nfc_app_i.h"

#include "../iso14443_4a/iso14443_4a_render.h"

void nfc_render_type_4_tag_info(
    const Type4TagData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    nfc_render_iso14443_4a_brief(type_4_tag_get_base_data(data), str);

    furi_string_cat_printf(
        str,
        "\n:::::::::::::::[%s]:::::::::::::::\n",
        NFC_UI_TEXT("Stored NDEF", "已存储 NDEF"));
    furi_string_cat_printf(
        str,
        "%s: %lu",
        NFC_UI_TEXT("Current NDEF Size", "当前 NDEF 大小"),
        simple_array_get_count(data->ndef_data));

    if(data->is_tag_specific) {
        furi_string_cat_printf(
            str, "\n::::::::::::::::::[%s]::::::::::::::::::\n", NFC_UI_TEXT("Tag Specs", "标签规格"));
        furi_string_cat_printf(
            str,
            "%s: %s\n",
            NFC_UI_TEXT("Card", "卡片"),
            furi_string_empty(data->platform_name) ? NFC_UI_TEXT("unknown", "未知") :
                                                     furi_string_get_cstr(data->platform_name));
        furi_string_cat_printf(
            str,
            "%s: %u.%u\n",
            NFC_UI_TEXT("T4T Mapping Version", "T4T 映射版本"),
            data->t4t_version.major,
            data->t4t_version.minor);
        furi_string_cat_printf(str, "%s: %04X\n", NFC_UI_TEXT("NDEF File ID", "NDEF 文件 ID"), data->ndef_file_id);
        furi_string_cat_printf(
            str, "%s: %u\n", NFC_UI_TEXT("Max NDEF Size", "最大 NDEF 大小"), data->ndef_max_len);
        furi_string_cat_printf(
            str,
            "%s: R:%u W:%u\n",
            NFC_UI_TEXT("APDU Sizes", "APDU 大小"),
            data->chunk_max_read,
            data->chunk_max_write);
        furi_string_cat_printf(
            str,
            "%s: %02X%s\n",
            NFC_UI_TEXT("Read Lock", "读取锁"),
            data->ndef_read_lock,
            data->ndef_read_lock == 0 ? NFC_UI_TEXT(" (unlocked)", " (未锁定)") : "");
        furi_string_cat_printf(
            str,
            "%s: %02X%s",
            NFC_UI_TEXT("Write Lock", "写入锁"),
            data->ndef_write_lock,
            data->ndef_write_lock == 0 ? NFC_UI_TEXT(" (unlocked)", " (未锁定)") : "");
    }

    if(format_type != NfcProtocolFormatTypeFull) return;

    furi_string_cat(str, "\n\e#ISO14443-4 data");
    nfc_render_iso14443_4a_extra(type_4_tag_get_base_data(data), str);
}

void nfc_render_type_4_tag_dump(const Type4TagData* data, FuriString* str) {
    size_t ndef_len = simple_array_get_count(data->ndef_data);
    if(ndef_len == 0) {
        furi_string_cat_str(str, NFC_UI_TEXT("No NDEF data to show", "没有可显示的 NDEF 数据"));
        return;
    }
    const uint8_t* ndef_data = simple_array_cget_data(data->ndef_data);
    furi_string_cat_printf(str, "\e*");
    for(size_t i = 0; i < ndef_len; i += TYPE_4_TAG_RENDER_BYTES_PER_LINE) {
        const uint8_t* line_data = &ndef_data[i];
        for(size_t j = 0; j < TYPE_4_TAG_RENDER_BYTES_PER_LINE; j += 2) {
            furi_string_cat_printf(str, " %02X%02X", line_data[j], line_data[j + 1]);
        }
    }
}
