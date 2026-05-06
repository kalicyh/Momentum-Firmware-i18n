#include "infrared_brute_force.h"

#include <stdlib.h>
#include <m-dict.h>
#include <m-array.h>
#include <flipper_format/flipper_format.h>

#include "infrared_signal.h"

#define TAG "InfraredBruteforce"

#define INFRARED_FILE_HEADER     "IR signals file"
#define INFRARED_LIBRARY_HEADER  "IR library file"
#define INFRARED_LIBRARY_VERSION (1)

ARRAY_DEF(SignalPositionArray, size_t, M_DEFAULT_OPLIST); //-V658

typedef struct {
    size_t index;
    SignalPositionArray_t signals;
} InfraredBruteForceRecord;

static inline void ir_bf_record_init(InfraredBruteForceRecord* record) {
    record->index = 0;
    SignalPositionArray_init(record->signals);
}
#define IR_BF_RECORD_INIT(r) (ir_bf_record_init(&(r)))

static inline void
    ir_bf_record_init_set(InfraredBruteForceRecord* dest, const InfraredBruteForceRecord* src) {
    dest->index = src->index;
    SignalPositionArray_init_set(dest->signals, src->signals);
}
#define IR_BF_RECORD_INIT_SET(d, s) (ir_bf_record_init_set(&(d), &(s)))

static inline void
    ir_bf_record_set(InfraredBruteForceRecord* dest, const InfraredBruteForceRecord* src) {
    dest->index = src->index;
    SignalPositionArray_set(dest->signals, src->signals);
}
#define IR_BF_RECORD_SET(d, s) (ir_bf_record_set(&(d), &(s)))

static inline void ir_bf_record_clear(InfraredBruteForceRecord* record) {
    SignalPositionArray_clear(record->signals);
}
#define IR_BF_RECORD_CLEAR(r) (ir_bf_record_clear(&(r)))

#define IR_BF_RECORD_OPLIST           \
    (INIT(IR_BF_RECORD_INIT),         \
     INIT_SET(IR_BF_RECORD_INIT_SET), \
     SET(IR_BF_RECORD_SET),           \
     CLEAR(IR_BF_RECORD_CLEAR))

DICT_DEF2(
    InfraredBruteForceRecordDict,
    FuriString*,
    FURI_STRING_OPLIST,
    InfraredBruteForceRecord,
    IR_BF_RECORD_OPLIST);

struct InfraredBruteForce {
    FlipperFormat* ff;
    const char* db_filename;
    FuriString* current_record_name;
    InfraredBruteForceRecord current_record;
    InfraredSignal* current_signal;
    InfraredBruteForceRecordDict_t records;
    bool is_started;
};

static bool infrared_brute_force_has_record_name(
    const InfraredBruteForce* brute_force,
    const char* name) {
    InfraredBruteForceRecordDict_it_t it;
    for(InfraredBruteForceRecordDict_it(it, brute_force->records);
        !InfraredBruteForceRecordDict_end_p(it);
        InfraredBruteForceRecordDict_next(it)) {
        const InfraredBruteForceRecordDict_itref_t* pair = InfraredBruteForceRecordDict_cref(it);
        if(strcmp(furi_string_get_cstr(pair->key), name) == 0) {
            return true;
        }
    }

    return false;
}

static const char* infrared_brute_force_first_matching_record_name(
    const InfraredBruteForce* brute_force,
    const char* const* candidates,
    size_t candidate_count) {
    for(size_t i = 0; i < candidate_count; ++i) {
        if(infrared_brute_force_has_record_name(brute_force, candidates[i])) {
            return candidates[i];
        }
    }

    return NULL;
}

static const char* infrared_brute_force_resolve_button_name(
    const InfraredBruteForce* brute_force,
    const char* name) {
    static const char* power_candidates[] = {"Power", "POWER"};
    static const char* play_candidates[] = {"Play", "PLAY"};
    const char* const* candidates = NULL;
    size_t candidate_count = 0;

    if(strcmp(name, "电源") == 0) {
        candidates = power_candidates;
        candidate_count = COUNT_OF(power_candidates);
    } else if(strcmp(name, "静音") == 0) {
        static const char* mute_candidates[] = {"Mute"};
        candidates = mute_candidates;
        candidate_count = COUNT_OF(mute_candidates);
    } else if(strcmp(name, "音量加") == 0) {
        static const char* vol_up_candidates[] = {"Vol_up"};
        candidates = vol_up_candidates;
        candidate_count = COUNT_OF(vol_up_candidates);
    } else if(strcmp(name, "音量减") == 0) {
        static const char* vol_dn_candidates[] = {"Vol_dn"};
        candidates = vol_dn_candidates;
        candidate_count = COUNT_OF(vol_dn_candidates);
    } else if(strcmp(name, "频道加") == 0) {
        static const char* ch_next_candidates[] = {"Ch_next"};
        candidates = ch_next_candidates;
        candidate_count = COUNT_OF(ch_next_candidates);
    } else if(strcmp(name, "频道减") == 0) {
        static const char* ch_prev_candidates[] = {"Ch_prev"};
        candidates = ch_prev_candidates;
        candidate_count = COUNT_OF(ch_prev_candidates);
    } else if(strcmp(name, "播放") == 0) {
        candidates = play_candidates;
        candidate_count = COUNT_OF(play_candidates);
    } else if(strcmp(name, "暂停") == 0) {
        static const char* pause_candidates[] = {"Pause"};
        candidates = pause_candidates;
        candidate_count = COUNT_OF(pause_candidates);
    } else if(strcmp(name, "上一曲") == 0) {
        static const char* prev_candidates[] = {"Prev"};
        candidates = prev_candidates;
        candidate_count = COUNT_OF(prev_candidates);
    } else if(strcmp(name, "下一曲") == 0) {
        static const char* next_candidates[] = {"Next"};
        candidates = next_candidates;
        candidate_count = COUNT_OF(next_candidates);
    } else if(strcmp(name, "模式") == 0) {
        static const char* mode_candidates[] = {"Mode"};
        candidates = mode_candidates;
        candidate_count = COUNT_OF(mode_candidates);
    } else if(strcmp(name, "风速加") == 0) {
        static const char* speed_up_candidates[] = {"Speed_up"};
        candidates = speed_up_candidates;
        candidate_count = COUNT_OF(speed_up_candidates);
    } else if(strcmp(name, "风速减") == 0) {
        static const char* speed_dn_candidates[] = {"Speed_dn"};
        candidates = speed_dn_candidates;
        candidate_count = COUNT_OF(speed_dn_candidates);
    } else if(strcmp(name, "摇头") == 0) {
        static const char* rotate_candidates[] = {"Rotate"};
        candidates = rotate_candidates;
        candidate_count = COUNT_OF(rotate_candidates);
    } else if(strcmp(name, "定时") == 0) {
        static const char* timer_candidates[] = {"Timer"};
        candidates = timer_candidates;
        candidate_count = COUNT_OF(timer_candidates);
    } else if(strcmp(name, "关闭") == 0) {
        static const char* off_candidates[] = {"Off"};
        candidates = off_candidates;
        candidate_count = COUNT_OF(off_candidates);
    } else if(strcmp(name, "除湿") == 0) {
        static const char* dh_candidates[] = {"Dh"};
        candidates = dh_candidates;
        candidate_count = COUNT_OF(dh_candidates);
    } else if(strcmp(name, "强力制冷") == 0) {
        static const char* cool_hi_candidates[] = {"Cool_hi"};
        candidates = cool_hi_candidates;
        candidate_count = COUNT_OF(cool_hi_candidates);
    } else if(strcmp(name, "节能制冷") == 0) {
        static const char* cool_lo_candidates[] = {"Cool_lo"};
        candidates = cool_lo_candidates;
        candidate_count = COUNT_OF(cool_lo_candidates);
    } else if(strcmp(name, "强力制热") == 0) {
        static const char* heat_hi_candidates[] = {"Heat_hi"};
        candidates = heat_hi_candidates;
        candidate_count = COUNT_OF(heat_hi_candidates);
    } else if(strcmp(name, "节能制热") == 0) {
        static const char* heat_lo_candidates[] = {"Heat_lo"};
        candidates = heat_lo_candidates;
        candidate_count = COUNT_OF(heat_lo_candidates);
    } else if(strcmp(name, "信号源") == 0) {
        static const char* source_candidates[] = {"SOURCE"};
        candidates = source_candidates;
        candidate_count = COUNT_OF(source_candidates);
    } else if(strcmp(name, "菜单") == 0) {
        static const char* menu_candidates[] = {"MENU"};
        candidates = menu_candidates;
        candidate_count = COUNT_OF(menu_candidates);
    } else if(strcmp(name, "退出") == 0) {
        static const char* exit_candidates[] = {"EXIT"};
        candidates = exit_candidates;
        candidate_count = COUNT_OF(exit_candidates);
    } else if(strcmp(name, "停止") == 0) {
        static const char* stop_candidates[] = {"STOP"};
        candidates = stop_candidates;
        candidate_count = COUNT_OF(stop_candidates);
    } else if(strcmp(name, "电源开") == 0) {
        static const char* power_on_candidates[] = {"Power_on"};
        candidates = power_on_candidates;
        candidate_count = COUNT_OF(power_on_candidates);
    } else if(strcmp(name, "电源关") == 0) {
        static const char* power_off_candidates[] = {"Power_off"};
        candidates = power_off_candidates;
        candidate_count = COUNT_OF(power_off_candidates);
    } else if(strcmp(name, "亮度加") == 0) {
        static const char* brightness_up_candidates[] = {"Brightness_up"};
        candidates = brightness_up_candidates;
        candidate_count = COUNT_OF(brightness_up_candidates);
    } else if(strcmp(name, "亮度减") == 0) {
        static const char* brightness_dn_candidates[] = {"Brightness_dn"};
        candidates = brightness_dn_candidates;
        candidate_count = COUNT_OF(brightness_dn_candidates);
    } else if(strcmp(name, "红色") == 0) {
        static const char* red_candidates[] = {"Red"};
        candidates = red_candidates;
        candidate_count = COUNT_OF(red_candidates);
    } else if(strcmp(name, "绿色") == 0) {
        static const char* green_candidates[] = {"Green"};
        candidates = green_candidates;
        candidate_count = COUNT_OF(green_candidates);
    } else if(strcmp(name, "蓝色") == 0) {
        static const char* blue_candidates[] = {"Blue"};
        candidates = blue_candidates;
        candidate_count = COUNT_OF(blue_candidates);
    } else if(strcmp(name, "白色") == 0) {
        static const char* white_candidates[] = {"White"};
        candidates = white_candidates;
        candidate_count = COUNT_OF(white_candidates);
    } else if(strcmp(name, "弹出") == 0) {
        static const char* eject_candidates[] = {"Eject"};
        candidates = eject_candidates;
        candidate_count = COUNT_OF(eject_candidates);
    } else if(strcmp(name, "快退") == 0) {
        static const char* fast_ba_candidates[] = {"Fast_ba"};
        candidates = fast_ba_candidates;
        candidate_count = COUNT_OF(fast_ba_candidates);
    } else if(strcmp(name, "快进") == 0) {
        static const char* fast_fo_candidates[] = {"Fast_fo"};
        candidates = fast_fo_candidates;
        candidate_count = COUNT_OF(fast_fo_candidates);
    } else if(strcmp(name, "确定") == 0) {
        static const char* ok_candidates[] = {"Ok"};
        candidates = ok_candidates;
        candidate_count = COUNT_OF(ok_candidates);
    } else if(strcmp(name, "字幕") == 0) {
        static const char* subtitle_candidates[] = {"Subtitle"};
        candidates = subtitle_candidates;
        candidate_count = COUNT_OF(subtitle_candidates);
    }

    if(candidates) {
        const char* resolved_name = infrared_brute_force_first_matching_record_name(
            brute_force, candidates, candidate_count);
        return resolved_name ? resolved_name : name;
    }

    return name;
}

InfraredBruteForce* infrared_brute_force_alloc(void) {
    InfraredBruteForce* brute_force = malloc(sizeof(InfraredBruteForce));
    brute_force->ff = NULL;
    brute_force->db_filename = NULL;
    brute_force->current_signal = NULL;
    brute_force->is_started = false;
    brute_force->current_record_name = furi_string_alloc();
    InfraredBruteForceRecordDict_init(brute_force->records);
    return brute_force;
}

void infrared_brute_force_free(InfraredBruteForce* brute_force) {
    furi_check(brute_force);
    furi_assert(!brute_force->is_started);
    InfraredBruteForceRecordDict_clear(brute_force->records);
    furi_string_free(brute_force->current_record_name);
    free(brute_force);
}

void infrared_brute_force_set_db_filename(InfraredBruteForce* brute_force, const char* db_filename) {
    furi_check(brute_force);
    furi_assert(!brute_force->is_started);
    brute_force->db_filename = db_filename;
}

InfraredErrorCode infrared_brute_force_calculate_messages_ex(
    InfraredBruteForce* brute_force,
    bool auto_detect_buttons,
    bool ignore_unknown_buttons) {
    furi_check(brute_force);
    furi_assert(!brute_force->is_started);
    furi_assert(brute_force->db_filename);
    InfraredErrorCode error = InfraredErrorCodeNone;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);
    FuriString* signal_name = furi_string_alloc();
    FuriString* lookup_name = furi_string_alloc();
    InfraredSignal* signal = infrared_signal_alloc();

    do {
        if(!flipper_format_buffered_file_open_existing(ff, brute_force->db_filename)) {
            error = InfraredErrorCodeFileOperationFailed;
            break;
        }

        uint32_t version;
        // Temporarily use signal_name to get header info
        if(!flipper_format_read_header(ff, signal_name, &version)) {
            error = InfraredErrorCodeFileOperationFailed;
            break;
        }

        if(furi_string_equal(signal_name, INFRARED_FILE_HEADER)) {
            FURI_LOG_E(TAG, "Remote file can't be loaded in this context");
            error = InfraredErrorCodeWrongFileType;
            break;
        }

        if(!furi_string_equal(signal_name, INFRARED_LIBRARY_HEADER)) {
            error = InfraredErrorCodeWrongFileType;
            FURI_LOG_E(TAG, "Filetype unknown");
            break;
        }

        if(version != INFRARED_LIBRARY_VERSION) {
            error = InfraredErrorCodeWrongFileVersion;
            FURI_LOG_E(TAG, "Wrong file version");
            break;
        }

        bool signal_valid = false;
        uint32_t auto_detect_button_index = 0;
        while(infrared_signal_read_name(ff, signal_name) == InfraredErrorCodeNone) {
            size_t signal_start = flipper_format_tell(ff);
            error = infrared_signal_read_body(signal, ff);
            signal_valid = (!INFRARED_ERROR_PRESENT(error)) && infrared_signal_is_valid(signal);
            if(!signal_valid) break;

            if(auto_detect_buttons) {
                furi_string_set(lookup_name, signal_name);
            } else {
                const char* resolved_name = infrared_brute_force_resolve_button_name(
                    brute_force, furi_string_get_cstr(signal_name));
                furi_string_set_str(lookup_name, resolved_name);
            }

            InfraredBruteForceRecord* record =
                InfraredBruteForceRecordDict_get(brute_force->records, lookup_name);
            if(!record) {
                if(auto_detect_buttons) {
                    infrared_brute_force_add_record(
                        brute_force,
                        auto_detect_button_index++,
                        furi_string_get_cstr(signal_name));
                    record = InfraredBruteForceRecordDict_get(brute_force->records, signal_name);
                } else if(ignore_unknown_buttons) {
                    continue;
                } else {
                    FURI_LOG_E(TAG, "Unknown signal name: %s", furi_string_get_cstr(signal_name));
                    furi_crash("Unknown signal name");
                }
            }
            SignalPositionArray_push_back(record->signals, signal_start);
        }
        if(!signal_valid) break;
    } while(false);

    infrared_signal_free(signal);
    furi_string_free(lookup_name);
    furi_string_free(signal_name);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return error;
}

InfraredErrorCode infrared_brute_force_calculate_messages(InfraredBruteForce* brute_force) {
    return infrared_brute_force_calculate_messages_ex(brute_force, false, false);
}

bool infrared_brute_force_start(
    InfraredBruteForce* brute_force,
    uint32_t index,
    uint32_t* record_count) {
    furi_check(brute_force);
    furi_assert(!brute_force->is_started);
    bool success = false;
    *record_count = 0;

    InfraredBruteForceRecordDict_it_t it;
    for(InfraredBruteForceRecordDict_it(it, brute_force->records);
        !InfraredBruteForceRecordDict_end_p(it);
        InfraredBruteForceRecordDict_next(it)) {
        const InfraredBruteForceRecordDict_itref_t* record = InfraredBruteForceRecordDict_cref(it);
        if(record->value.index == index) {
            *record_count = SignalPositionArray_size(record->value.signals);
            if(*record_count) {
                furi_string_set(brute_force->current_record_name, record->key);
                brute_force->current_record = record->value;
            }
            break;
        }
    }

    if(*record_count) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        brute_force->ff = flipper_format_buffered_file_alloc(storage);
        brute_force->current_signal = infrared_signal_alloc();
        brute_force->is_started = true;
        success =
            flipper_format_buffered_file_open_existing(brute_force->ff, brute_force->db_filename);
        if(!success) infrared_brute_force_stop(brute_force);
    }
    return success;
}

bool infrared_brute_force_is_started(const InfraredBruteForce* brute_force) {
    furi_check(brute_force);
    return brute_force->is_started;
}

void infrared_brute_force_stop(InfraredBruteForce* brute_force) {
    furi_check(brute_force);
    furi_assert(brute_force->is_started);
    furi_string_reset(brute_force->current_record_name);
    infrared_signal_free(brute_force->current_signal);
    flipper_format_free(brute_force->ff);
    brute_force->current_signal = NULL;
    brute_force->ff = NULL;
    brute_force->is_started = false;
    furi_record_close(RECORD_STORAGE);
}

bool infrared_brute_force_send(InfraredBruteForce* brute_force, uint32_t signal_index) {
    furi_check(brute_force);
    furi_assert(brute_force->is_started);

    if(signal_index >= SignalPositionArray_size(brute_force->current_record.signals)) return false;

    size_t signal_start =
        *SignalPositionArray_cget(brute_force->current_record.signals, signal_index);
    if(!flipper_format_seek(brute_force->ff, signal_start, FlipperFormatOffsetFromStart))
        return false;

    if(INFRARED_ERROR_PRESENT(
           infrared_signal_read_body(brute_force->current_signal, brute_force->ff)))
        return false;

    infrared_signal_transmit(brute_force->current_signal);
    return true;
}

void infrared_brute_force_add_record(
    InfraredBruteForce* brute_force,
    uint32_t index,
    const char* name) {
    InfraredBruteForceRecord value;
    ir_bf_record_init(&value);
    value.index = index;
    FuriString* key;
    key = furi_string_alloc_set(name);
    InfraredBruteForceRecordDict_set_at(brute_force->records, key, value);
    furi_string_free(key);
}

void infrared_brute_force_reset(InfraredBruteForce* brute_force) {
    furi_assert(!brute_force->is_started);
    InfraredBruteForceRecordDict_reset(brute_force->records);
}

size_t infrared_brute_force_get_button_count(const InfraredBruteForce* brute_force) {
    size_t size = InfraredBruteForceRecordDict_size(brute_force->records);
    return size;
}

const char*
    infrared_brute_force_get_button_name(const InfraredBruteForce* brute_force, size_t index) {
    if(index >= infrared_brute_force_get_button_count(brute_force)) {
        return NULL;
    }

    InfraredBruteForceRecordDict_it_t it;
    for(InfraredBruteForceRecordDict_it(it, brute_force->records);
        !InfraredBruteForceRecordDict_end_p(it);
        InfraredBruteForceRecordDict_next(it)) {
        // Dict elements are unordered, they may be shuffled while adding elements, so the
        // index used in add_record() may differ when iterating here, so we have to check
        // the stored index not "position" index
        const InfraredBruteForceRecordDict_itref_t* pair = InfraredBruteForceRecordDict_cref(it);
        if(pair->value.index == index) {
            const char* button_name = furi_string_get_cstr(pair->key);
            return button_name;
        }
    }

    return NULL; //just as fallback
}
