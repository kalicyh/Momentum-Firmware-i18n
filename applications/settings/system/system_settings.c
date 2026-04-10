#include "system_settings.h"
#include <loader/loader.h>
#include <lib/toolbox/value_index.h>
#include <locale/locale.h>
#include <dialogs/dialogs.h>

enum VarItemListIndex {
    VarItemListIndexHandOrient,
    VarItemListIndexUnits,
    VarItemListIndexTimeFormat,
    VarItemListIndexDateFormat,
    VarItemListIndexLogLevel,
    VarItemListIndexLogDevice,
    VarItemListIndexLogBaudRate,
    VarItemListIndexDebug,
    VarItemListIndexHeapTrace,
    VarItemListIndexSleepMethod,
    VarItemListIndexFileNaming,
};

const char* const log_level_text[] = {
    SYSTEM_SETTINGS_UI_TEXT("Default", "默认"),
    SYSTEM_SETTINGS_UI_TEXT("None", "无"),
    SYSTEM_SETTINGS_UI_TEXT("Error", "错误"),
    SYSTEM_SETTINGS_UI_TEXT("Warning", "警告"),
    SYSTEM_SETTINGS_UI_TEXT("Info", "信息"),
    SYSTEM_SETTINGS_UI_TEXT("Debug", "调试"),
    SYSTEM_SETTINGS_UI_TEXT("Trace", "追踪"),
};

const uint32_t log_level_value[] = {
    FuriLogLevelDefault,
    FuriLogLevelNone,
    FuriLogLevelError,
    FuriLogLevelWarn,
    FuriLogLevelInfo,
    FuriLogLevelDebug,
    FuriLogLevelTrace,
};

static void log_level_changed(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, log_level_text[index]);
    furi_hal_rtc_set_log_level(log_level_value[index]);
}

const char* const log_device_text[] = {
    "USART",
    "LPUART",
    "None",
};

const uint32_t log_device_value[] = {
    FuriHalRtcLogDeviceUsart,
    FuriHalRtcLogDeviceLpuart,
    FuriHalRtcLogDeviceNone};

static void log_device_changed(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, log_device_text[index]);
    furi_hal_rtc_set_log_device(log_device_value[index]);
}

const char* const log_baud_rate_text[] = {
    "9600",
    "38400",
    "57600",
    "115200",
    "230400",
    "460800",
    "921600",
    "1843200",
};

const uint32_t log_baud_rate_value[] = {
    FuriHalRtcLogBaudRate9600,
    FuriHalRtcLogBaudRate38400,
    FuriHalRtcLogBaudRate57600,
    FuriHalRtcLogBaudRate115200,
    FuriHalRtcLogBaudRate230400,
    FuriHalRtcLogBaudRate460800,
    FuriHalRtcLogBaudRate921600,
    FuriHalRtcLogBaudRate1843200,
};

static void log_baud_rate_changed(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, log_baud_rate_text[index]);
    furi_hal_rtc_set_log_baud_rate(log_baud_rate_value[index]);
}

const char* const debug_text[] = {
    SYSTEM_SETTINGS_UI_TEXT("OFF", "关"),
    SYSTEM_SETTINGS_UI_TEXT("ON", "开"),
};

static void debug_changed(VariableItem* item) {
    SystemSettings* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, debug_text[index]);
    if(index) {
        view_dispatcher_send_custom_event(app->view_dispatcher, VarItemListIndexDebug);
    } else {
        furi_hal_rtc_reset_flag(FuriHalRtcFlagDebug);
    }
}

const char* const heap_trace_mode_text[] = {
    SYSTEM_SETTINGS_UI_TEXT("None", "无"),
    SYSTEM_SETTINGS_UI_TEXT("Main", "主线程"),
#ifdef FURI_DEBUG
    SYSTEM_SETTINGS_UI_TEXT("Tree", "树状"),
    SYSTEM_SETTINGS_UI_TEXT("All", "全部"),
#endif
};

const uint32_t heap_trace_mode_value[] = {
    FuriHalRtcHeapTrackModeNone,
    FuriHalRtcHeapTrackModeMain,
#ifdef FURI_DEBUG
    FuriHalRtcHeapTrackModeTree,
    FuriHalRtcHeapTrackModeAll,
#endif
};

static void heap_trace_mode_changed(VariableItem* item) {
    SystemSettings* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, heap_trace_mode_text[index]);
    furi_hal_rtc_set_heap_track_mode(heap_trace_mode_value[index]);
    if(index) {
        view_dispatcher_send_custom_event(app->view_dispatcher, VarItemListIndexHeapTrace);
    }
}

const char* const measurement_units_text[] = {
    SYSTEM_SETTINGS_UI_TEXT("Metric", "公制"),
    SYSTEM_SETTINGS_UI_TEXT("Imperial", "英制"),
};

const uint32_t measurement_units_value[] = {
    LocaleMeasurementUnitsMetric,
    LocaleMeasurementUnitsImperial,
};

static void measurement_units_changed(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, measurement_units_text[index]);
    locale_set_measurement_unit(measurement_units_value[index]);
}

const char* const time_format_text[] = {
    "24h",
    "12h",
};

const uint32_t time_format_value[] = {
    LocaleTimeFormat24h,
    LocaleTimeFormat12h,
};

static void time_format_changed(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, time_format_text[index]);
    locale_set_time_format(time_format_value[index]);
}

const char* const date_format_text[] = {
    "D/M/Y",
    "M/D/Y",
    "Y/M/D",
};

const uint32_t date_format_value[] = {
    LocaleDateFormatDMY,
    LocaleDateFormatMDY,
    LocaleDateFormatYMD,
};

static void date_format_changed(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, date_format_text[index]);
    locale_set_date_format(date_format_value[index]);
}

const char* const hand_mode[] = {
    SYSTEM_SETTINGS_UI_TEXT("Righty", "右手"),
    SYSTEM_SETTINGS_UI_TEXT("Lefty", "左手"),
};

static void hand_orient_changed(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, hand_mode[index]);
    if(index) {
        furi_hal_rtc_set_flag(FuriHalRtcFlagHandOrient);
    } else {
        furi_hal_rtc_reset_flag(FuriHalRtcFlagHandOrient);
    }
}

const char* const sleep_method[] = {
    SYSTEM_SETTINGS_UI_TEXT("Default", "默认"),
    SYSTEM_SETTINGS_UI_TEXT("Legacy", "旧版"),
};

static void sleep_method_changed(VariableItem* item) {
    SystemSettings* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, sleep_method[index]);
    if(index) {
        view_dispatcher_send_custom_event(app->view_dispatcher, VarItemListIndexSleepMethod);
    } else {
        furi_hal_rtc_reset_flag(FuriHalRtcFlagLegacySleep);
    }
}

const char* const filename_scheme[] = {
    SYSTEM_SETTINGS_UI_TEXT("Time", "时间"),
    SYSTEM_SETTINGS_UI_TEXT("Random", "随机"),
};

static void filename_scheme_changed(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, filename_scheme[index]);
    if(index) {
        furi_hal_rtc_set_flag(FuriHalRtcFlagRandomFilename);
    } else {
        furi_hal_rtc_reset_flag(FuriHalRtcFlagRandomFilename);
    }
}

static uint32_t system_settings_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static bool system_settings_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    SystemSettings* app = context;

    VariableItem* item = variable_item_list_get(app->var_item_list, event);
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    DialogMessage* msg = dialog_message_alloc();
    dialog_message_set_buttons(
        msg, SYSTEM_SETTINGS_UI_TEXT("No", "否"), NULL, SYSTEM_SETTINGS_UI_TEXT("Yes", "是"));

    switch(event) {
    case VarItemListIndexDebug:
        dialog_message_set_header(
            msg, SYSTEM_SETTINGS_UI_TEXT("Enable Debug?", "启用调试？"), 64, 4, AlignCenter, AlignTop);
        dialog_message_set_text(
            msg,
            SYSTEM_SETTINGS_UI_TEXT(
                "This consumes 400% more\nbattery life. Don't use unless\nyou know exactly what\nyou're doing.",
                "会额外消耗 400%\n电量。除非你明确\n知道自己在做什么，\n否则不要启用。"),
            64,
            36,
            AlignCenter,
            AlignCenter);
        if(dialog_message_show(dialogs, msg) == DialogMessageButtonRight) {
            furi_hal_rtc_set_flag(FuriHalRtcFlagDebug);
        } else {
            variable_item_set_current_value_text(item, debug_text[0]);
            variable_item_set_current_value_index(item, 0);
        }
        break;
    case VarItemListIndexSleepMethod:
        dialog_message_set_header(
            msg,
            SYSTEM_SETTINGS_UI_TEXT("Disable DeepSleep?", "禁用深度睡眠？"),
            64,
            4,
            AlignCenter,
            AlignTop);
        dialog_message_set_text(
            msg,
            SYSTEM_SETTINGS_UI_TEXT(
                "Disabling will consume 400%\nmore battery life. Only\ndisable if you have a\nspecific reason.",
                "禁用后会额外消耗 400%\n电量。仅在确有\n明确需求时\n再关闭。"),
            64,
            36,
            AlignCenter,
            AlignCenter);
        if(dialog_message_show(dialogs, msg) == DialogMessageButtonRight) {
            furi_hal_rtc_set_flag(FuriHalRtcFlagLegacySleep);
        } else {
            variable_item_set_current_value_text(item, sleep_method[0]);
            variable_item_set_current_value_index(item, 0);
        }
        break;
    case VarItemListIndexHeapTrace:
        dialog_message_set_header(
            msg, SYSTEM_SETTINGS_UI_TEXT("Enable Heap Trace?", "启用堆追踪？"), 64, 4, AlignCenter, AlignTop);
        dialog_message_set_text(
            msg,
            SYSTEM_SETTINGS_UI_TEXT(
                "Will use more RAM and might\ncause Out Of Memory errors.\nDon't enable without a\nspecific reason.",
                "会占用更多 RAM，\n并可能导致内存不足。\n没有明确需求时\n不要启用。"),
            64,
            36,
            AlignCenter,
            AlignCenter);
        if(dialog_message_show(dialogs, msg) != DialogMessageButtonRight) {
            furi_hal_rtc_set_heap_track_mode(heap_trace_mode_value[0]);
            variable_item_set_current_value_text(item, heap_trace_mode_text[0]);
            variable_item_set_current_value_index(item, 0);
        }
        break;
    default:
        break;
    }

    dialog_message_free(msg);
    furi_record_close(RECORD_DIALOGS);
    return true;
}

SystemSettings* system_settings_alloc(void) {
    SystemSettings* app = malloc(sizeof(SystemSettings));

    // Load settings
    app->gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, system_settings_custom_event_callback);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    VariableItem* item;
    uint8_t value_index;
    app->var_item_list = variable_item_list_alloc();

    item = variable_item_list_add(
        app->var_item_list,
        SYSTEM_SETTINGS_UI_TEXT("Hand Orient", "持机方向"),
        COUNT_OF(hand_mode),
        hand_orient_changed,
        app);
    value_index = furi_hal_rtc_is_flag_set(FuriHalRtcFlagHandOrient) ? 1 : 0;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, hand_mode[value_index]);

    item = variable_item_list_add(
        app->var_item_list,
        SYSTEM_SETTINGS_UI_TEXT("Units", "单位"),
        COUNT_OF(measurement_units_text),
        measurement_units_changed,
        app);
    value_index = value_index_uint32(
        locale_get_measurement_unit(), measurement_units_value, COUNT_OF(measurement_units_value));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, measurement_units_text[value_index]);

    item = variable_item_list_add(
        app->var_item_list,
        SYSTEM_SETTINGS_UI_TEXT("Time Format", "时间格式"),
        COUNT_OF(time_format_text),
        time_format_changed,
        app);
    value_index = value_index_uint32(
        locale_get_time_format(), time_format_value, COUNT_OF(time_format_value));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, time_format_text[value_index]);

    item = variable_item_list_add(
        app->var_item_list,
        SYSTEM_SETTINGS_UI_TEXT("Date Format", "日期格式"),
        COUNT_OF(date_format_text),
        date_format_changed,
        app);
    value_index = value_index_uint32(
        locale_get_date_format(), date_format_value, COUNT_OF(date_format_value));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, date_format_text[value_index]);

    item = variable_item_list_add(
        app->var_item_list,
        SYSTEM_SETTINGS_UI_TEXT("Log Level", "日志等级"),
        COUNT_OF(log_level_text),
        log_level_changed,
        app);
    value_index = value_index_uint32(
        furi_hal_rtc_get_log_level(), log_level_value, COUNT_OF(log_level_text));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, log_level_text[value_index]);

    item = variable_item_list_add(
        app->var_item_list,
        SYSTEM_SETTINGS_UI_TEXT("Log Device", "日志设备"),
        COUNT_OF(log_device_text),
        log_device_changed,
        app);
    value_index = value_index_uint32(
        furi_hal_rtc_get_log_device(), log_device_value, COUNT_OF(log_device_text));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, log_device_text[value_index]);

    item = variable_item_list_add(
        app->var_item_list,
        SYSTEM_SETTINGS_UI_TEXT("Log Baud Rate", "日志波特率"),
        COUNT_OF(log_baud_rate_text),
        log_baud_rate_changed,
        app);
    value_index = value_index_uint32(
        furi_hal_rtc_get_log_baud_rate(), log_baud_rate_value, COUNT_OF(log_baud_rate_text));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, log_baud_rate_text[value_index]);

    item = variable_item_list_add(
        app->var_item_list,
        SYSTEM_SETTINGS_UI_TEXT("Debug", "调试"),
        COUNT_OF(debug_text),
        debug_changed,
        app);
    value_index = furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug) ? 1 : 0;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, debug_text[value_index]);

    item = variable_item_list_add(
        app->var_item_list,
        SYSTEM_SETTINGS_UI_TEXT("Heap Trace", "堆追踪"),
        COUNT_OF(heap_trace_mode_text),
        heap_trace_mode_changed,
        app);
    value_index = value_index_uint32(
        furi_hal_rtc_get_heap_track_mode(), heap_trace_mode_value, COUNT_OF(heap_trace_mode_text));
    furi_hal_rtc_set_heap_track_mode(heap_trace_mode_value[value_index]);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, heap_trace_mode_text[value_index]);

    item = variable_item_list_add(
        app->var_item_list,
        SYSTEM_SETTINGS_UI_TEXT("Sleep Method", "睡眠方式"),
        COUNT_OF(sleep_method),
        sleep_method_changed,
        app);
    value_index = furi_hal_rtc_is_flag_set(FuriHalRtcFlagLegacySleep) ? 1 : 0;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, sleep_method[value_index]);

    item = variable_item_list_add(
        app->var_item_list,
        SYSTEM_SETTINGS_UI_TEXT("File Naming", "文件命名"),
        COUNT_OF(filename_scheme),
        filename_scheme_changed,
        app);
    value_index = furi_hal_rtc_is_flag_set(FuriHalRtcFlagRandomFilename) ? 1 : 0;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, filename_scheme[value_index]);

    view_set_previous_callback(
        variable_item_list_get_view(app->var_item_list), system_settings_exit);
    view_dispatcher_add_view(
        app->view_dispatcher,
        SystemSettingsViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    view_dispatcher_switch_to_view(app->view_dispatcher, SystemSettingsViewVarItemList);

    return app;
}

void system_settings_free(SystemSettings* app) {
    furi_assert(app);
    // Variable item list
    view_dispatcher_remove_view(app->view_dispatcher, SystemSettingsViewVarItemList);
    variable_item_list_free(app->var_item_list);
    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    // Records
    furi_record_close(RECORD_GUI);
    free(app);
}

int32_t system_settings_app(void* p) {
    UNUSED(p);
    SystemSettings* app = system_settings_alloc();
    view_dispatcher_run(app->view_dispatcher);
    system_settings_free(app);
    return 0;
}
