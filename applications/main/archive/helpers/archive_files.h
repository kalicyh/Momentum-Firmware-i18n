#pragma once

#include <furi.h>
#include <storage/storage.h>
#include "toolbox/path.h"
#include <momentum/momentum.h>

#define FAP_MANIFEST_MAX_ICON_SIZE 32

typedef enum {
    ArchiveFileTypeIButton,
    ArchiveFileTypeNFC,
    ArchiveFileTypeSubGhz,
    ArchiveFileTypeLFRFID,
    ArchiveFileTypeInfrared,
    ArchiveFileTypeSubghzPlaylist,
    ArchiveFileTypeSubghzRemote,
    ArchiveFileTypeInfraredRemote,
    ArchiveFileTypeBadUsb,
    ArchiveFileTypeWAV,
    ArchiveFileTypeMag,
    ArchiveFileTypeCrossRemote,
    ArchiveFileTypePicopass,
    ArchiveFileTypeU2f,
    ArchiveFileTypeUpdateManifest,
    ArchiveFileTypeApplication,
    ArchiveFileTypeJS,
    ArchiveFileTypeSearch,
    ArchiveFileTypeDiskImage,
    ArchiveFileTypeFolder,
    ArchiveFileTypeSetting,
    ArchiveFileTypeProtoPirate,
    ArchiveFileTypeUnknown,
    ArchiveFileTypeAppOrJs,
    ArchiveFileTypeLoading,
} ArchiveFileTypeEnum;

typedef struct {
    FuriString* path;
    ArchiveFileTypeEnum type;
    uint8_t* custom_icon_data;
    FuriString* custom_name;
    bool fav;
    bool is_app;
} ArchiveFile_t;

void ArchiveFile_t_init(ArchiveFile_t* obj);
void ArchiveFile_t_init_set(ArchiveFile_t* obj, const ArchiveFile_t* src);
void ArchiveFile_t_set(ArchiveFile_t* obj, const ArchiveFile_t* src);
void ArchiveFile_t_clear(ArchiveFile_t* obj);
int ArchiveFile_t_cmp(const ArchiveFile_t* a, const ArchiveFile_t* b);

#define M_OPL_ArchiveFile_t()                 \
    (INIT(API_2(ArchiveFile_t_init)),         \
     SET(API_6(ArchiveFile_t_set)),           \
     INIT_SET(API_6(ArchiveFile_t_init_set)), \
     CLEAR(API_2(ArchiveFile_t_clear)),       \
     CMP(API_6(ArchiveFile_t_cmp)),           \
     SWAP(M_SWAP_DEFAULT),                    \
     EQUAL(API_6(M_EQUAL_DEFAULT)))

typedef struct {
    size_t size;
    size_t alloc;
    ArchiveFile_t* ptr;
} files_array_data_t;

typedef files_array_data_t files_array_t[1];

void files_array_init(files_array_t array);
void files_array_reset(files_array_t array);
void files_array_clear(files_array_t array);
size_t files_array_size(const files_array_t array);
ArchiveFile_t* files_array_get(const files_array_t array, size_t index);
void files_array_sort(files_array_t array);
void files_array_remove_v(files_array_t array, size_t start, size_t end);
void files_array_pop_at(ArchiveFile_t* out, files_array_t array, size_t index);
void files_array_push_at(files_array_t array, size_t index, const ArchiveFile_t item);
void files_array_swap_at(files_array_t array, size_t index1, size_t index2);
void files_array_push_back(files_array_t array, const ArchiveFile_t item);

void archive_set_file_type(ArchiveFile_t* file, const char* path, bool is_folder, bool is_app);
bool archive_get_items(void* context, const char* path);
void archive_file_append(const char* path, const char* format, ...)
    _ATTRIBUTE((__format__(__printf__, 2, 3)));
void archive_delete_file(void* context, const char* format, ...)
    _ATTRIBUTE((__format__(__printf__, 2, 3)));
FS_Error archive_copy_rename_file_or_dir(
    void* context,
    const char* src_path,
    FuriString* dst_path,
    bool copy,
    bool find_name);
