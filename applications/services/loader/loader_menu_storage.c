#include "loader_menu_storage_i.h"

#include <core/thread.h>
#include <storage/storage.h>
#include <gui/modules/dialog_ex.h>
#include <gui/view_holder.h>
#include <assets_icons.h>

#include "loader.h"

typedef enum {
    FormatFlagContinue = (1 << 0),
    FormatFlagCancel = (1 << 1),
    FormatFlagAll = (FormatFlagContinue | FormatFlagCancel),
} FormatFlag;

#ifdef MOMENTUM_UI_LANG_ZH_CN
#define LOADER_STORAGE_UI_TEXT(en, zh) (zh)
#else
#define LOADER_STORAGE_UI_TEXT(en, zh) (en)
#endif

static void loader_menu_storage_settings_callback(DialogExResult result, void* context) {
    FuriThread* thread = context;
    furi_thread_flags_set(
        furi_thread_get_id(thread),
        result == DialogExResultRight ? FormatFlagContinue : FormatFlagCancel);
}

static void loader_menu_storage_settings_back(void* context) {
    FuriThread* thread = context;
    furi_thread_flags_set(furi_thread_get_id(thread), FormatFlagCancel);
}

int32_t loader_menu_storage_settings(void* context) {
    Storage* storage = context;
    Loader* loader = furi_record_open(RECORD_LOADER);
#ifdef MOMENTUM_UI_LANG_ZH_CN
    LoaderStatus result = loader_start(loader, "存储", NULL, NULL);
#else
    LoaderStatus result = loader_start(loader, "Storage", NULL, NULL);
#endif
    furi_record_close(RECORD_LOADER);

    if(result != LoaderStatusOk) {
        DialogEx* dialog_ex = dialog_ex_alloc();
        Gui* gui = furi_record_open(RECORD_GUI);
        ViewHolder* view_holder = view_holder_alloc();
        view_holder_attach_to_gui(view_holder, gui);
        view_holder_set_view(view_holder, dialog_ex_get_view(dialog_ex));
        dialog_ex_set_context(dialog_ex, furi_thread_get_current());
        dialog_ex_set_result_callback(dialog_ex, loader_menu_storage_settings_callback);
        view_holder_set_back_callback(
            view_holder, loader_menu_storage_settings_back, furi_thread_get_current());

        dialog_ex_set_header(
            dialog_ex,
            LOADER_STORAGE_UI_TEXT("Update needed", "需要更新"),
            64,
            0,
            AlignCenter,
            AlignTop);
        dialog_ex_set_text(
            dialog_ex,
            LOADER_STORAGE_UI_TEXT(
                "Reinstall firmware\n"
                "to run this app.\n"
                "Can format SD\n"
                "here if needed.",
                "请重装固件\n"
                "以运行此应用。\n"
                "如有需要\n"
                "可在此格式化 SD。"),
            3,
            17,
            AlignLeft,
            AlignTop);
        dialog_ex_set_icon(dialog_ex, 83, 11, &I_WarningDolphinFlip_45x42);
        dialog_ex_set_right_button_text(
            dialog_ex, LOADER_STORAGE_UI_TEXT("Format SD", "格式化 SD"));

        FormatFlag flag = furi_thread_flags_wait(FormatFlagAll, FuriFlagWaitAny, FuriWaitForever);
        if(flag == FormatFlagContinue) {
            char text[96];
            dialog_ex_set_header(
                dialog_ex,
                LOADER_STORAGE_UI_TEXT("Format SD Card?", "格式化 SD 卡?"),
                64,
                0,
                AlignCenter,
                AlignTop);
            dialog_ex_set_icon(dialog_ex, 0, 0, NULL);
            dialog_ex_set_left_button_text(dialog_ex, LOADER_STORAGE_UI_TEXT("Cancel", "取消"));
            dialog_ex_set_right_button_text(dialog_ex, LOADER_STORAGE_UI_TEXT("Format", "格式化"));
            for(uint8_t counter = 5; counter > 0; counter--) {
                snprintf(
                    text,
                    sizeof(text),
                    LOADER_STORAGE_UI_TEXT(
                        "All data will be lost!\n%d presses left",
                        "所有数据将丢失!\n还需按 %d 次"),
                    counter);
                dialog_ex_set_text(dialog_ex, text, 64, 12, AlignCenter, AlignTop);
                flag = furi_thread_flags_wait(FormatFlagAll, FuriFlagWaitAny, FuriWaitForever);
                if(flag != FormatFlagContinue) break;

                if(counter == 1) {
                    dialog_ex_set_header(
                        dialog_ex,
                        LOADER_STORAGE_UI_TEXT("Formatting...", "格式化中..."),
                        70,
                        32,
                        AlignCenter,
                        AlignCenter);
                    dialog_ex_set_text(dialog_ex, NULL, 0, 0, AlignCenter, AlignCenter);
                    dialog_ex_set_icon(dialog_ex, 15, 20, &I_LoadingHourglass_24x24);
                    dialog_ex_set_left_button_text(dialog_ex, NULL);
                    dialog_ex_set_right_button_text(dialog_ex, NULL);

                    FS_Error error = storage_sd_format(storage);
                    if(error != FSE_OK) {
                        dialog_ex_set_header(
                            dialog_ex,
                            LOADER_STORAGE_UI_TEXT("Cannot Format SD Card", "无法格式化 SD 卡"),
                            64,
                            10,
                            AlignCenter,
                            AlignCenter);
                        dialog_ex_set_icon(dialog_ex, 0, 0, NULL);
                        dialog_ex_set_text(
                            dialog_ex,
                            storage_error_get_desc(error),
                            64,
                            32,
                            AlignCenter,
                            AlignCenter);
                    } else {
                        dialog_ex_set_icon(dialog_ex, 48, 6, &I_DolphinDone_80x58);
                        dialog_ex_set_header(
                            dialog_ex,
                            LOADER_STORAGE_UI_TEXT("Formatted", "已格式化"),
                            5,
                            10,
                            AlignLeft,
                            AlignTop);
                    }
                    dialog_ex_set_left_button_text(
                        dialog_ex, LOADER_STORAGE_UI_TEXT("Finish", "完成"));
                    furi_thread_flags_wait(FormatFlagAll, FuriFlagWaitAny, FuriWaitForever);
                }
            }
        }

        view_holder_set_view(view_holder, NULL);
        view_holder_free(view_holder);
        furi_record_close(RECORD_GUI);
        dialog_ex_free(dialog_ex);
    }
    return 0;
}
