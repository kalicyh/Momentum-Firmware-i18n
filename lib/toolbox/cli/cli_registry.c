#include "cli_registry.h"
#include "cli_registry_i.h"
#include <toolbox/pipe.h>
#include <storage/storage.h>
#include <string.h>

#define TAG "CliRegistry"

typedef struct {
    FuriString* name;
    CliRegistryCommand command;
} CliRegistryEntry;

struct CliRegistry {
    CliRegistryEntry* commands;
    size_t count;
    size_t alloc;
    FuriMutex* mutex;
};

static void cli_registry_reserve(CliRegistry* registry, size_t min_alloc) {
    furi_assert(registry);

    if(registry->alloc >= min_alloc) {
        return;
    }

    size_t new_alloc = registry->alloc ? registry->alloc : 8;
    while(new_alloc < min_alloc) {
        new_alloc *= 2;
    }

    CliRegistryEntry* commands = realloc(registry->commands, sizeof(CliRegistryEntry) * new_alloc);
    furi_check(commands != NULL);

    registry->commands = commands;
    registry->alloc = new_alloc;
}

static ssize_t cli_registry_find_index(CliRegistry* registry, FuriString* name) {
    furi_assert(registry);
    furi_assert(name);

    for(size_t i = 0; i < registry->count; i++) {
        if(furi_string_cmp(registry->commands[i].name, name) == 0) {
            return (ssize_t)i;
        }
    }

    return -1;
}

CliRegistry* cli_registry_alloc(void) {
    CliRegistry* registry = malloc(sizeof(CliRegistry));
    registry->commands = NULL;
    registry->count = 0;
    registry->alloc = 0;
    registry->mutex = furi_mutex_alloc(FuriMutexTypeRecursive);
    return registry;
}

void cli_registry_free(CliRegistry* registry) {
    furi_check(furi_mutex_acquire(registry->mutex, FuriWaitForever) == FuriStatusOk);
    furi_mutex_free(registry->mutex);

    for(size_t i = 0; i < registry->count; i++) {
        furi_string_free(registry->commands[i].name);
    }

    free(registry->commands);
    free(registry);
}

void cli_registry_add_command(
    CliRegistry* registry,
    const char* name,
    CliCommandFlag flags,
    CliCommandExecuteCallback callback,
    void* context) {
    cli_registry_add_command_ex(
        registry, name, flags, callback, context, CLI_BUILTIN_COMMAND_STACK_SIZE);
}

void cli_registry_add_command_ex(
    CliRegistry* registry,
    const char* name,
    CliCommandFlag flags,
    CliCommandExecuteCallback callback,
    void* context,
    size_t stack_size) {
    furi_check(registry);
    furi_check(name);
    furi_check(callback);

    // the shell always attaches the pipe to the stdio, thus both flags can't be used at once
    if(flags & CliCommandFlagUseShellThread) furi_check(!(flags & CliCommandFlagDontAttachStdio));

    FuriString* name_str;
    name_str = furi_string_alloc_set(name);
    // command cannot contain spaces
    furi_check(furi_string_search_char(name_str, ' ') == FURI_STRING_FAILURE);

    CliRegistryCommand command = {
        .context = context,
        .execute_callback = callback,
        .flags = flags,
        .stack_depth = stack_size,
    };

    furi_check(furi_mutex_acquire(registry->mutex, FuriWaitForever) == FuriStatusOk);
    ssize_t index = cli_registry_find_index(registry, name_str);
    if(index >= 0) {
        registry->commands[index].command = command;
    } else {
        cli_registry_reserve(registry, registry->count + 1);
        registry->commands[registry->count].name = furi_string_alloc_set(name_str);
        registry->commands[registry->count].command = command;
        registry->count++;
    }
    furi_check(furi_mutex_release(registry->mutex) == FuriStatusOk);

    furi_string_free(name_str);
}

void cli_registry_delete_command(CliRegistry* registry, const char* name) {
    furi_check(registry);
    FuriString* name_str;
    name_str = furi_string_alloc_set(name);
    furi_string_trim(name_str);

    size_t name_replace;
    do {
        name_replace = furi_string_replace(name_str, " ", "_");
    } while(name_replace != FURI_STRING_FAILURE);

    furi_check(furi_mutex_acquire(registry->mutex, FuriWaitForever) == FuriStatusOk);
    ssize_t index = cli_registry_find_index(registry, name_str);
    if(index >= 0) {
        furi_string_free(registry->commands[index].name);
        if((size_t)index + 1 < registry->count) {
            memmove(
                &registry->commands[index],
                &registry->commands[index + 1],
                (registry->count - (size_t)index - 1) * sizeof(registry->commands[0]));
        }
        registry->count--;
    }
    furi_check(furi_mutex_release(registry->mutex) == FuriStatusOk);

    furi_string_free(name_str);
}

bool cli_registry_get_command(
    CliRegistry* registry,
    FuriString* command,
    CliRegistryCommand* result) {
    furi_assert(registry);
    furi_check(furi_mutex_acquire(registry->mutex, FuriWaitForever) == FuriStatusOk);
    ssize_t index = cli_registry_find_index(registry, command);
    if(index >= 0) {
        *result = registry->commands[index].command;
    }

    furi_check(furi_mutex_release(registry->mutex) == FuriStatusOk);

    return index >= 0;
}

void cli_registry_remove_external_commands(CliRegistry* registry) {
    furi_check(registry);
    furi_check(furi_mutex_acquire(registry->mutex, FuriWaitForever) == FuriStatusOk);

    size_t write_index = 0;
    for(size_t read_index = 0; read_index < registry->count; read_index++) {
        CliRegistryEntry* entry = &registry->commands[read_index];
        if(entry->command.flags & CliCommandFlagExternal) {
            furi_string_free(entry->name);
            continue;
        }

        if(write_index != read_index) {
            registry->commands[write_index] = *entry;
        }
        write_index++;
    }
    registry->count = write_index;

    furi_check(furi_mutex_release(registry->mutex) == FuriStatusOk);
}

void cli_registry_reload_external_commands(
    CliRegistry* registry,
    const CliCommandExternalConfig* config) {
    furi_check(registry);
    furi_check(furi_mutex_acquire(registry->mutex, FuriWaitForever) == FuriStatusOk);
    FURI_LOG_D(TAG, "Reloading ext commands");

    cli_registry_remove_external_commands(registry);

    // iterate over files in plugin directory
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* plugin_dir = storage_file_alloc(storage);

    if(storage_dir_open(plugin_dir, config->search_directory)) {
        char plugin_filename[64];
        FuriString* plugin_name = furi_string_alloc();

        while(storage_dir_read(plugin_dir, NULL, plugin_filename, sizeof(plugin_filename))) {
            FURI_LOG_T(TAG, "Plugin: %s", plugin_filename);
            furi_string_set_str(plugin_name, plugin_filename);

            if(!furi_string_end_with_str(plugin_name, ".fal")) continue;
            furi_string_replace_all_str(plugin_name, ".fal", "");
            if(!furi_string_start_with_str(plugin_name, config->fal_prefix)) continue;
            furi_string_replace_at(plugin_name, 0, strlen(config->fal_prefix), "");

            CliRegistryCommand command = {
                .context = NULL,
                .execute_callback = NULL,
                .flags = CliCommandFlagExternal,
            };
            ssize_t index = cli_registry_find_index(registry, plugin_name);
            if(index >= 0) {
                registry->commands[index].command = command;
            } else {
                cli_registry_reserve(registry, registry->count + 1);
                registry->commands[registry->count].name = furi_string_alloc_set(plugin_name);
                registry->commands[registry->count].command = command;
                registry->count++;
            }
        }

        furi_string_free(plugin_name);
    }

    storage_dir_close(plugin_dir);
    storage_file_free(plugin_dir);
    furi_record_close(RECORD_STORAGE);

    FURI_LOG_D(TAG, "Done reloading ext commands");
    furi_check(furi_mutex_release(registry->mutex) == FuriStatusOk);
}

void cli_registry_lock(CliRegistry* registry) {
    furi_assert(registry);
    furi_check(furi_mutex_acquire(registry->mutex, FuriWaitForever) == FuriStatusOk);
}

void cli_registry_unlock(CliRegistry* registry) {
    furi_assert(registry);
    furi_mutex_release(registry->mutex);
}

size_t cli_registry_get_count(CliRegistry* registry) {
    furi_assert(registry);
    return registry->count;
}

FuriString* cli_registry_get_name_at(CliRegistry* registry, size_t index) {
    furi_assert(registry);
    furi_assert(index < registry->count);
    return registry->commands[index].name;
}

const CliRegistryCommand* cli_registry_get_command_at(CliRegistry* registry, size_t index) {
    furi_assert(registry);
    furi_assert(index < registry->count);
    return &registry->commands[index].command;
}
