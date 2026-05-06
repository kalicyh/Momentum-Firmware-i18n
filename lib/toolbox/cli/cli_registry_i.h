/**
 * @file cli_registry_i.h
 * Internal API for getting commands registered with the CLI
 */

#pragma once

#include <furi.h>
#include "cli_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLI_BUILTIN_COMMAND_STACK_SIZE (4 * 1024U)

typedef struct {
    void* context; //<! Context passed to callbacks
    CliCommandExecuteCallback execute_callback; //<! Callback for command execution
    CliCommandFlag flags;
    size_t stack_depth;
} CliRegistryCommand;

bool cli_registry_get_command(
    CliRegistry* registry,
    FuriString* command,
    CliRegistryCommand* result);

void cli_registry_lock(CliRegistry* registry);

void cli_registry_unlock(CliRegistry* registry);

size_t cli_registry_get_count(CliRegistry* registry);
FuriString* cli_registry_get_name_at(CliRegistry* registry, size_t index);
const CliRegistryCommand* cli_registry_get_command_at(CliRegistry* registry, size_t index);

#ifdef __cplusplus
}
#endif
