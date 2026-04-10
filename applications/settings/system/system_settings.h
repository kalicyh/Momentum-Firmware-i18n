#pragma once

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    VariableItemList* var_item_list;
} SystemSettings;

typedef enum {
    SystemSettingsViewVarItemList,
} SystemSettingsView;

#ifdef MOMENTUM_UI_LANG_ZH_CN
#define SYSTEM_SETTINGS_UI_TEXT(en, zh) (zh)
#else
#define SYSTEM_SETTINGS_UI_TEXT(en, zh) (en)
#endif
