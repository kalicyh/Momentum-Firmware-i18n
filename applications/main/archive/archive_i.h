#pragma once

#include "archive.h"
#include <stdint.h>
#include <furi.h>
#include <gui/gui_i.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>
#include <gui/modules/widget_elements/widget_element_i.h>
#include <gui/view_stack.h>
#include <dialogs/dialogs.h>
#include <gui/modules/loading.h>
#include <toolbox/dir_walk.h>
#include <loader/loader.h>

#include "views/archive_browser_view.h"
#include "scenes/archive_scene.h"

typedef enum {
    ArchiveViewBrowser,
    ArchiveViewTextInput,
    ArchiveViewWidget,
    ArchiveViewTotal,
    ArchiveViewStack,
} ArchiveViewEnum;

struct ArchiveApp {
    Gui* gui;
    Loader* loader;
    ViewDispatcher* view_dispatcher;
    ViewStack* view_stack;
    SceneManager* scene_manager;
    ArchiveBrowserView* browser;
    TextInput* text_input;
    Widget* widget;
    DialogsApp* dialogs;
    Loading* loading;
    FuriPubSubSubscription* loader_stop_subscription;

    FuriString* fav_move_str;
    char text_store[MAX_NAME_LEN];
    FuriString* file_extension;

    WidgetElement* size_element;
    WidgetElement* count_element;
    FuriThread* info_thread;
    FuriThread* search_thread;
};

void archive_show_loading_popup(ArchiveApp* context, bool show);

#ifdef MOMENTUM_UI_LANG_ZH_CN
#define ARCHIVE_UI_TEXT(en, zh) (zh)
#else
#define ARCHIVE_UI_TEXT(en, zh) (en)
#endif

#define ARCHIVE_SEARCH_PATH_START \
    ARCHIVE_UI_TEXT("/app:search/Search for files", "/app:search/搜索文件")
#define ARCHIVE_SEARCH_PATH_CANCEL \
    ARCHIVE_UI_TEXT("/app:search/Cancel search", "/app:search/取消搜索")
#define ARCHIVE_SEARCH_PATH_NO_RESULTS \
    ARCHIVE_UI_TEXT("/app:search/No results found!", "/app:search/未找到结果!")
#define ARCHIVE_SEARCH_PATH_ERROR \
    ARCHIVE_UI_TEXT("/app:search/Error while searching!", "/app:search/搜索出错!")
