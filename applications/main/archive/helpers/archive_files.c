#include "archive_files.h"
#include "archive_apps.h"
#include "archive_browser.h"
#include <applications/external/subghz_playlist/playlist_file.h>
#include <applications/external/subghz_remote/subghz_remote_app_i.h>
#include <applications/external/ir_remote/infrared_remote.h>

#define TAG "Archive"

#define ASSETS_DIR "assets"

void ArchiveFile_t_init(ArchiveFile_t* obj) {
    obj->path = furi_string_alloc();
    obj->type = ArchiveFileTypeUnknown;
    obj->custom_icon_data = NULL;
    obj->custom_name = furi_string_alloc();
    obj->fav = false;
    obj->is_app = false;
}

void ArchiveFile_t_init_set(ArchiveFile_t* obj, const ArchiveFile_t* src) {
    obj->path = furi_string_alloc_set(src->path);
    obj->type = src->type;
    if(src->custom_icon_data) {
        obj->custom_icon_data = malloc(FAP_MANIFEST_MAX_ICON_SIZE);
        memcpy(obj->custom_icon_data, src->custom_icon_data, FAP_MANIFEST_MAX_ICON_SIZE);
    } else {
        obj->custom_icon_data = NULL;
    }
    obj->custom_name = furi_string_alloc_set(src->custom_name);
    obj->fav = src->fav;
    obj->is_app = src->is_app;
}

void ArchiveFile_t_set(ArchiveFile_t* obj, const ArchiveFile_t* src) {
    furi_string_set(obj->path, src->path);
    obj->type = src->type;
    if(src->custom_icon_data) {
        obj->custom_icon_data = malloc(FAP_MANIFEST_MAX_ICON_SIZE);
        memcpy(obj->custom_icon_data, src->custom_icon_data, FAP_MANIFEST_MAX_ICON_SIZE);
    } else {
        obj->custom_icon_data = NULL;
    }
    furi_string_set(obj->custom_name, src->custom_name);
    obj->fav = src->fav;
    obj->is_app = src->is_app;
}

void ArchiveFile_t_clear(ArchiveFile_t* obj) {
    furi_string_free(obj->path);
    if(obj->custom_icon_data) {
        free(obj->custom_icon_data);
        obj->custom_icon_data = NULL;
    }
    furi_string_free(obj->custom_name);
}

int ArchiveFile_t_cmp(const ArchiveFile_t* a, const ArchiveFile_t* b) {
    if(momentum_settings.sort_dirs_first) {
        if(a->type == ArchiveFileTypeFolder && b->type != ArchiveFileTypeFolder) {
            return -1;
        }
        if(a->type != ArchiveFileTypeFolder && b->type == ArchiveFileTypeFolder) {
            return 1;
        }
    }

    return furi_string_cmpi(
        furi_string_empty(a->custom_name) ? a->path : a->custom_name,
        furi_string_empty(b->custom_name) ? b->path : b->custom_name);
}

static void files_array_reserve(files_array_t array, size_t min_alloc) {
    furi_assert(array);

    if(array->alloc >= min_alloc) {
        return;
    }

    size_t new_alloc = array->alloc ? array->alloc : 4;
    while(new_alloc < min_alloc) {
        new_alloc = new_alloc < 16 ? new_alloc * 2 : new_alloc + new_alloc / 2;
    }

    ArchiveFile_t* new_ptr = realloc(array->ptr, sizeof(ArchiveFile_t) * new_alloc);
    furi_check(new_ptr != NULL);

    array->ptr = new_ptr;
    array->alloc = new_alloc;
}

void files_array_init(files_array_t array) {
    furi_assert(array);
    array->size = 0;
    array->alloc = 0;
    array->ptr = NULL;
}

void files_array_reset(files_array_t array) {
    furi_assert(array);

    for(size_t i = 0; i < array->size; i++) {
        ArchiveFile_t_clear(&array->ptr[i]);
    }

    array->size = 0;
}

void files_array_clear(files_array_t array) {
    furi_assert(array);

    files_array_reset(array);
    free(array->ptr);
    array->ptr = NULL;
    array->alloc = 0;
}

size_t files_array_size(const files_array_t array) {
    furi_assert(array);
    return array->size;
}

ArchiveFile_t* files_array_get(const files_array_t array, size_t index) {
    furi_assert(array);
    furi_assert(index < array->size);
    return &array->ptr[index];
}

static int files_array_qsort_cmp(const void* lhs, const void* rhs) {
    return ArchiveFile_t_cmp(lhs, rhs);
}

void files_array_sort(files_array_t array) {
    furi_assert(array);

    if(array->size > 1) {
        qsort(array->ptr, array->size, sizeof(array->ptr[0]), files_array_qsort_cmp);
    }
}

void files_array_remove_v(files_array_t array, size_t start, size_t end) {
    furi_assert(array);
    furi_assert(start <= end);
    furi_assert(end <= array->size);

    if(start == end) {
        return;
    }

    for(size_t i = start; i < end; i++) {
        ArchiveFile_t_clear(&array->ptr[i]);
    }

    size_t tail_count = array->size - end;
    if(tail_count > 0) {
        memmove(&array->ptr[start], &array->ptr[end], tail_count * sizeof(array->ptr[0]));
    }

    array->size -= end - start;
}

void files_array_pop_at(ArchiveFile_t* out, files_array_t array, size_t index) {
    furi_assert(out);
    furi_assert(array);
    furi_assert(index < array->size);

    ArchiveFile_t_set(out, &array->ptr[index]);
    files_array_remove_v(array, index, index + 1);
}

void files_array_push_at(files_array_t array, size_t index, const ArchiveFile_t item) {
    furi_assert(array);
    furi_assert(index <= array->size);

    files_array_reserve(array, array->size + 1);

    if(index < array->size) {
        memmove(
            &array->ptr[index + 1],
            &array->ptr[index],
            (array->size - index) * sizeof(array->ptr[0]));
    }

    ArchiveFile_t_init_set(&array->ptr[index], &item);
    array->size++;
}

void files_array_swap_at(files_array_t array, size_t index1, size_t index2) {
    furi_assert(array);
    furi_assert(index1 < array->size);
    furi_assert(index2 < array->size);

    if(index1 == index2) {
        return;
    }

    ArchiveFile_t tmp = array->ptr[index1];
    array->ptr[index1] = array->ptr[index2];
    array->ptr[index2] = tmp;
}

void files_array_push_back(files_array_t array, const ArchiveFile_t item) {
    files_array_push_at(array, array->size, item);
}

void archive_set_file_type(ArchiveFile_t* file, const char* path, bool is_folder, bool is_app) {
    furi_assert(file);

    file->is_app = is_app;
    if(is_app) {
        file->type = archive_get_app_filetype(archive_get_app_type(path));
    } else if(is_folder) {
        file->type = ArchiveFileTypeFolder;
    } else {
        for(size_t i = 0; i < COUNT_OF(known_ext); i++) {
            if((known_ext[i][0] == '?') || (known_ext[i][0] == '*')) continue;
            if(furi_string_end_withi(file->path, known_ext[i])) {
                // Check for .txt containing folder
                if(strcmp(known_ext[i], ".txt") == 0) {
                    const char* txt_path = NULL;
                    switch(i) {
                    case ArchiveFileTypeSubghzPlaylist:
                        txt_path = PLAYLIST_FOLDER;
                        break;
                    case ArchiveFileTypeSubghzRemote:
                        txt_path = SUBREM_APP_FOLDER;
                        break;
                    case ArchiveFileTypeInfraredRemote:
                        txt_path = IR_REMOTE_PATH;
                        break;
                    case ArchiveFileTypeBadUsb:
                        txt_path = archive_get_default_path(ArchiveTabBadUsb);
                        break;
                    }
                    if(txt_path != NULL) {
                        size_t len = strlen(txt_path);
                        if(furi_string_size(file->path) < len) continue;
                        // Compare but ignore /ext or /any, continue if different (memcmp() != 0)
                        if(memcmp(furi_string_get_cstr(file->path) + 4, txt_path + 4, len - 4)) {
                            continue;
                        }
                        file->type = i;
                        return;
                    }
                } else {
                    file->type = i;
                    return;
                }
            }
        }

        file->type = ArchiveFileTypeUnknown;
    }
}

bool archive_get_items(void* context, const char* path) {
    furi_assert(context);

    bool res = false;
    ArchiveBrowserView* browser = context;

    if(archive_get_tab(browser) == ArchiveTabFavorites) {
        res = archive_favorites_read(browser);
    } else if(strncmp(path, "/app:", 5) == 0) {
        res = archive_app_read_dir(browser, path);
    }
    return res;
}

void archive_file_append(const char* path, const char* format, ...) {
    furi_assert(path);

    FuriString* string;
    va_list args;
    va_start(args, format);
    string = furi_string_alloc_vprintf(format, args);
    va_end(args);

    Storage* fs_api = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(fs_api);

    bool res = storage_file_open(file, path, FSAM_WRITE, FSOM_OPEN_APPEND);

    if(res) {
        storage_file_write(file, furi_string_get_cstr(string), furi_string_size(string));
    }

    furi_string_free(string);
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

void archive_delete_file(void* context, const char* format, ...) {
    furi_assert(context);

    FuriString* filename;
    va_list args;
    va_start(args, format);
    filename = furi_string_alloc_vprintf(format, args);
    va_end(args);

    ArchiveBrowserView* browser = context;
    Storage* fs_api = furi_record_open(RECORD_STORAGE);

    FileInfo fileinfo;
    storage_common_stat(fs_api, furi_string_get_cstr(filename), &fileinfo);

    bool res = false;

    if(file_info_is_dir(&fileinfo)) {
        res = storage_simply_remove_recursive(fs_api, furi_string_get_cstr(filename));
    } else {
        res = (storage_common_remove(fs_api, furi_string_get_cstr(filename)) == FSE_OK);
    }

    furi_record_close(RECORD_STORAGE);

    if(archive_is_favorite("%s", furi_string_get_cstr(filename))) {
        archive_favorites_delete("%s", furi_string_get_cstr(filename));
    }

    if(res) {
        archive_file_array_rm_selected(browser);
    }

    furi_string_free(filename);
}

FS_Error archive_copy_rename_file_or_dir(
    void* context,
    const char* src_path,
    FuriString* dst_path,
    bool copy,
    bool find_name) {
    furi_assert(context);
    FURI_LOG_I(
        TAG, "%s from %s to %s", copy ? "Copy" : "Move", src_path, furi_string_get_cstr(dst_path));

    Storage* fs_api = furi_record_open(RECORD_STORAGE);

    FileInfo fileinfo;
    storage_common_stat(fs_api, src_path, &fileinfo);

    FS_Error error = FSE_OK;

    if(!path_contains_only_ascii(furi_string_get_cstr(dst_path))) {
        error = FSE_INVALID_NAME;
    } else if(!copy && !strcmp(src_path, furi_string_get_cstr(dst_path))) {
        error = FSE_EXIST;
    } else {
        if(find_name && storage_common_exists(fs_api, furi_string_get_cstr(dst_path))) {
            FuriString* dir_path = furi_string_alloc();
            FuriString* filename = furi_string_alloc();
            FuriString* file_ext = furi_string_alloc();

            path_extract_dirname(furi_string_get_cstr(dst_path), dir_path);
            path_extract_filename(dst_path, filename, true);
            path_extract_ext_str(dst_path, file_ext);

            storage_get_next_filename(
                fs_api,
                furi_string_get_cstr(dir_path),
                furi_string_get_cstr(filename),
                furi_string_get_cstr(file_ext),
                dst_path,
                255);
            furi_string_cat_printf(
                dir_path, "/%s%s", furi_string_get_cstr(dst_path), furi_string_get_cstr(file_ext));
            furi_string_set(dst_path, dir_path);

            furi_string_free(dir_path);
            furi_string_free(filename);
            furi_string_free(file_ext);
        }

        if(copy) {
            error = storage_common_copy(fs_api, src_path, furi_string_get_cstr(dst_path));
        } else {
            error = storage_common_rename_safe(fs_api, src_path, furi_string_get_cstr(dst_path));
        }
    }
    furi_record_close(RECORD_STORAGE);

    if(!copy && archive_is_favorite("%s", src_path)) {
        archive_favorites_rename(src_path, furi_string_get_cstr(dst_path));
    }

    if(error == FSE_OK) {
        FURI_LOG_I(
            TAG,
            "%s from %s to %s is DONE",
            copy ? "Copy" : "Move",
            src_path,
            furi_string_get_cstr(dst_path));
    } else {
        FURI_LOG_E(
            TAG,
            "%s from %s to %s failed: %s, Code: %d",
            copy ? "Copy" : "Move",
            src_path,
            furi_string_get_cstr(dst_path),
            filesystem_api_error_get_desc(error),
            error);
    }

    return error;
}
