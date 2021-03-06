
/****************************************************************************
 * Copyright (c) 2011 Yahoo! Inc. All rights reserved. Licensed under the
 * Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of the License
 * at http://www.apache.org/licenses/LICENSE-2.0 Unless required by applicable
 * law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the specific language
 * governing permissions and limitations under the License.
 * See accompanying LICENSE file. 
 ************************************************************************** */


#include "upk_include.h"
#include "upk_json.h"

/**
  @file
  @brief configuration defaults, and implementation
  @addtogroup config_impl
  @{
  */

/* FIXME: figure out how to get token line and char number and add to meta structure for error reporting */

/* ******************************************************************************************************************
   ****************************************************************************************************************** */

typedef enum {
    SVCID_APPENDER = 0,
    ACTION_APPENDER,
} appender_type_t;

/* upkeeper/upk_config.c */
static void             upk_svcconf_svcid_array_appender(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static inline void      upk_svcconf_setup_array_handlers(upk_json_handlers_t * handlers);
static void             upk_svcconf_array_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static void             upk_svcconf_customaction_array_appender(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static inline void      upk_svcconf_setup_object_handlers(upk_json_handlers_t * handlers);
static void             upk_svcconf_nested_object_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static inline void      upk_svcconf_setup_handlers(upk_json_handlers_t * handlers);
static void             upk_ctrlconf_object_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static void             upk_ctrlconf_string_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static void             upk_ctrlconf_double_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static void             upk_ctrlconf_toplvl_obj(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static void             upk_svcconf_object_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static void             upk_svcconf_toplvl_obj(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static void             upk_ctrlconf_pack(upk_controller_config_t * cfg, const char *json_string);
static void             upk_svcconf_bool_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static void             upk_svcconf_int_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static void             upk_svcconf_string_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static void             upk_conf_error_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v);
static char            *upk_config_loadfile(const char *filename);

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
/* FIXME: allow specifying default via configure */
char                    upk_ctrl_configuration_file[UPK_MAX_PATH_LEN] = stringify(CONF_SYSCONFDIR) "/upkeeper.conf";

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
/* *INDENT-OFF* */
const char upk_default_configuration_str[] = 
    "{\n" 
	"    // StateDir\n" 
	"    // Path to variable state-dir for controller and buddies\n" 
    "    \"StateDir\": \"" stringify(CONF_LOCALSTATEDIR) "/upkeeper\",\n" 
    "\n" 
    "    // SvcConfigPath\n" 
    "    // Path to location of service configuration files\n" 
    "    \"SvcConfigPath\": \"" stringify(CONF_SYSCONFDIR) "/upkeeper.d\",\n" 
    "\n" 
    "    // SvcRunPath\n" 
    "    // Path to location to setup and run buddies\n" 
    "    \"SvcRunPath\": \"" stringify(CONF_LOCALSTATEDIR) "/upkeeper/buddies\",\n" 
    "\n" 
    "    // Path to the buddy executable\n"
    "    \"UpkBuddyPath\": \"" stringify(CONF_LIBEXECDIR) "/upk_buddy\",\n"
    "\n"
/* TODO: 
    "    // Specified in seconds, the longest duration to keep audit records\n"
    "    // for, or a value < 0 if they should be kept forever\n"
    "    \"AuditMaxAge\": 31536000, // One year\n"
    "\n"
*/
/* TODO: 
    "    // Maximum number of audit records to keep. In addition to MaxAge,\n"
    "    // this provides a way to ensure audit records to not exhaust disk\n"
    "    // space and/or grow excessively, use a negative value to indicate\n"
    "    // that countroller should keep all records (until they surpass\n"
    "    // AuditMaxAge
    "    \"AuditMaxRecords\": -1
    "\n"
*/
    "    // The verbosity for spawned buddy. An integer value between 0 and 7\n"
    "    \"BuddyVerbosity\": 1,\n"
    "\n"
    "    // How frequently buddy sockets should be polled for events\n"
    "    // in seconds and fractions of a second\n"
    "    \"BuddyPollingInterval\": 0.5,\n"
    "\n"
    "    // ServiceDefaults:\n" 
    "    \"ServiceDefaults\": {\n"
    "        // An array of strings (['foo','bar']) describing what this service provides. These are then used in ordering service\n"
    "        // startup via prerequisites. Service name, package, and UUID are implicitely added to the list of Provides.\n"
    "        \"Provides\": [],\n"
    "\n"
    "        // A valid UUID for the service, will be automatically generated if not provided.\n"
    "        \"UUID\": Null,\n"
    "\n"
    "        // A brief description of the service\n"
    "        \"ShortDescription\": Null,\n"
    "\n"
    "        // A longer and more complete description of the service\n"
    "        \"LongDescription\": Null,\n"
    "\n"
    "        // An array of strings (['foo','uuid#<...>']) of what must already be running (as named in 'Provides')\n"
    "        // before this service should start. The syntax <package-name>:<service-name> may be used to specify a service from\n"
    "        // a specific package. The syntax 'uuid#<uuid-string>' can be used to specify an explicit UUID.\n"
    "        \"Prerequisites\": [],\n"
    "\n"
    "        // Numeric start priority, used in lieu of, or in conjunction with Provides/Prerequisites to determine start order\n"
    "        \"StartPriority\": 0,\n"
    "\n"
    "        // Shutdown timeout before resorting to SIGKILL, Values < 0 will never SIGKILL\n" 
    "        \"KillTimeout\": 60,\n"
    "\n"
    "        // Maximum number of times a process may fail in-a-row before its state is changed to down\n"
    "        // a negative value indicates to restart forever (and is the default)\n"
    "        \"MaxConsecutiveFailures\": -1,\n"
    "\n"
    "        // user-defined max number of restarts within restart window\n"
    "        \"UserMaxRestarts\": Null,\n"
    "\n"
    "        // User-defined restart window, in seconds\n"
    "        \"UserRestartWindow\": Null,\n" 
    "\n"
    "        // duration, in seconds, to wait between respawn attempts\n"
    "        \"UserRateLimit\": Null,\n" 
    "\n"
    "        // a flag to enable/disable adding a randomized jitter to the user_ratelimit\n"
    "        \"RandomizeRateLimit\": false,\n" 
    "\n"
    "        // if controller and/or buddy is run euid root; which uid to run the service as\n"
    "        \"SetUID\": 0,\n" 
    "\n"
    "        // if controller and/or buddy is run euid root; which gid to run the service as\n"
    "        \"SetGID\": 0,\n" 
    "\n"
    "        // size of the ringbuffer to maintain in the buddy\n"
    "        \"RingbufferSize\": 256,\n" 
    "\n"
    "        // number of times to retry connections to the controler when emergent actions\n"
    "        // occur in the buddy; (-1 for indefinate)\n"
    "        \"ReconnectRetries\": 10,\n" 
    "\n"
    "        // command to exec for start, see 'StartScript'\n"
    "        \"ExecStart\": Null,\n" 
    "\n"
    "        // script to start the monitored process; The default is 'exec %(EXEC_START)'\n"
    "        \"StartScript\": \"#!/bin/sh\\nexec %(EXEC_START)\\n\",\n" 
    "\n"
    "        // command to exec for stop. Default: 'kill', see 'StopScript'\n"
    "        \"ExecStop\": \"kill\",\n" 
    "\n"
    "        // Script to stop the monitored process. The default is 'exec %(EXEC_STOP) $1'; where $1 passed\n"
    "        // to it will be the pid of monitored process (and also the pgrp, and sid, unless the monitored process changed them\n"
    "        \"StopScript\": \"#!/bin/sh\\nexec %(EXEC_STOP) $1\\n\",\n" 
    "\n"
    "        // command to exec for reload. Default: 'kill -HUP', see 'ReloadScript'\n"
    "        \"ExecReload\": \"kill -HUP\",\n" 
    "\n"
    "        // Script to reload the monitored process. The default is 'exec %(EXEC_RELOAD) $1'; where $1 passed\n"
    "        // to it will be the pid of monitored process (and also the pgrp, and sid, unless the monitored process changed them\n"
    "        \"ReloadScript\": \"#!/bin/sh\\nexec %(EXEC_RELOAD) $1\\n\",\n" 
    "\n"
    "        // A collection of key/value (JSON Object: {\"foo\":\"bar\"}) pairs where the key is the name of the action, and\n"
    "        // the value is the contents of a script script to run for that action\n"
    "        \"CustomActions\": {},\n" 
    "\n"
    "        // optional script to pipe stdout to. For example: 'exec logger -p local0.notice'\n"
    "        \"PipeStdoutScript\": Null,\n" 
    "\n"
    "        // optional script to pipe stderr to. For example: 'exec logger -p local0.warn'\n"
    "        \"PipeStderrScript\": Null,\n" 
    "\n"
    "        // optional place to direct stdout.\n"
    "        // Note that if you pipe stdout elsewhere, this might never be written to, unless the thing you pipe to prints\n"
    "        // to stdout itself\n"
    "        \"RedirectStdout\": Null,\n" 
    "\n"
    "        // optional place to direct stderr.\n"
    "        // Note that if you pipe stderr elsewhere, this might never be written to, unless the thing you pipe to prints\n"
    "        // to stderr itself\n"
    "        \"RedirectStderr\": Null,\n" 
    "\n"
    "        // state the service should be set to initially. this is used only when a service is first configured.\n"
    "        \"InitialState\": \"stopped\",\n" 
    "\n"
    "        // May be used by a package to instruct the controler to remove a configured service\n"
    "        // if the file defining that service ever disappears. Possibly useful in packaging\n"
    "        // to cleanup the controller on package removal. The default behavior is to ignore\n"
    "        // file removal, and require explicit manual removal of configured services\n"
    "        \"UnconfigureOnFileRemoval\": false,\n" 
    "\n"
    "        // If the controller starts/restarts, and buddy has a service state set to 'stopped',\n"
    "        // but controller's data-store believes the service should be running, prefer\n"
    "        // buddy's world view, and update the data-store to reflect the stopped state.\n"
    "        // The default is to trust the data-store; which would cause the service to be started\n"
    "        \"PreferBuddyStateForStopped\": false,\n" 
    "\n"
    "        // if the controller starts/restarts, and buddy has a service state set to 'running',\n"
    "        // but controller's data-store believes the service should be stopped, prefer\n"
    "        // buddy's world view, and update the data-store to reflect the running state.\n"
    "        // The default is to trust the buddy, which would leave the service running\n"
    "        \"PreferBuddyStateForRunning\": true,\n"
    "    },\n"
    "}\n"
    ;
/* *INDENT-ON* */

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
/* FIXME: this could lead to thread safety issues; need to do this more cleanly */
upk_controller_config_t upk_default_configuration;
upk_controller_config_t upk_file_configuration;
upk_controller_config_t upk_db_configuration;
upk_controller_config_t upk_runtime_configuration;

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_svc_desc_free(upk_svc_desc_t * svc)
{
    if(svc->Provides)
        UPKLIST_FREE(svc->Provides);
    if(svc->LongDescription)
        free(svc->LongDescription);
    if(svc->Prerequisites)
        UPKLIST_FREE(svc->Prerequisites);
    if(svc->StartScript)
        free(svc->StartScript);
    if(svc->StopScript)
        free(svc->StopScript);
    if(svc->ReloadScript)
        free(svc->ReloadScript);
    if(svc->CustomActions) {
        UPKLIST_FOREACH(svc->CustomActions) {
            if(svc->CustomActions->thisp->script) {
                free(svc->CustomActions->thisp->script);
                svc->CustomActions->thisp->script = NULL;
            } 
            UPKLIST_UNLINK(svc->CustomActions);
        }
        free(svc->CustomActions);
    }
    if(svc->PipeStdoutScript)
        free(svc->PipeStdoutScript);
    if(svc->PipeStderrScript)
        free(svc->PipeStderrScript);
    if(svc->LongDescription)
        free(svc->LongDescription);
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_svclist_free(upk_svc_desc_meta_t * svclist)
{
    if(svclist) {
        UPKLIST_FOREACH(svclist) {
            upk_svc_desc_free(svclist->thisp);
        }
        UPKLIST_FREE(svclist);
    }
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_ctrlconf_free(upk_controller_config_t * cfg)
{
    upk_svclist_free(cfg->svclist);
    upk_svc_desc_free(&cfg->ServiceDefaults);
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_ctrl_free_config(void)
{
    upk_ctrlconf_free(&upk_default_configuration);
    upk_ctrlconf_free(&upk_file_configuration);
    upk_ctrlconf_free(&upk_runtime_configuration);
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_svcconf_svcid_array_appender(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_svcid_meta_t       *d = data;
    char                   *p = 0;

    UPKLIST_APPEND(d);

    if((p = index(v.val.str.c_str, ':'))) {
        if((p - v.val.str.c_str) < UPK_MAX_STRING_LEN) {
            strncpy(d->thisp->pkg, v.val.str.c_str, (p - v.val.str.c_str));
            strncpy(d->thisp->name, p + 1, UPK_MAX_STRING_LEN - 1);
        }
    } else if(strncasecmp(v.val.str.c_str, "uuid#", 5) == 0) {
        strncpy(d->thisp->pkg, v.val.str.c_str + 5, UPK_MAX_STRING_LEN - 1);
    } else {
        strncpy(d->thisp->name, v.val.str.c_str, UPK_MAX_STRING_LEN - 1);
    }
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static inline void
upk_svcconf_setup_array_handlers(upk_json_handlers_t * handlers)
{
    upk_json_handler_t     *restrict json_null = &handlers->json_null;
    upk_json_handler_t     *restrict json_bool = &handlers->json_bool;
    upk_json_handler_t     *restrict json_double = &handlers->json_double;
    upk_json_handler_t     *restrict json_int = &handlers->json_int;
    upk_json_handler_t     *restrict json_string = &handlers->json_string;
    upk_json_handler_t     *restrict json_object = &handlers->json_object;
    upk_json_handler_t     *restrict json_array = &handlers->json_array;
    upk_json_handler_t     *restrict after_json_obj_pop = &handlers->after_json_obj_pop;
    upk_json_handler_t     *restrict after_json_array_pop = &handlers->after_json_array_pop;

    *json_null = NULL;
    *json_bool = upk_conf_error_handler;
    *json_double = upk_conf_error_handler;
    *json_int = upk_conf_error_handler;
    *json_string = upk_svcconf_svcid_array_appender;
    *json_array = upk_conf_error_handler;
    *json_object = upk_conf_error_handler;
    *after_json_obj_pop = NULL;
    *after_json_array_pop = NULL;
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_svcconf_array_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_svc_desc_t         *d = data;
    upk_json_stack_node_t   service_array_node = { 0 };

    if(strcasecmp(key, "Provides") == 0) {
        if(!d->Provides)
            d->Provides = calloc(1, sizeof(*d->Provides));
        service_array_node.data = d->Provides;
        upk_svcconf_setup_array_handlers(&service_array_node.handlers);
        upk_json_stack_push(meta, &service_array_node);
    } else if(strcasecmp(key, "Prerequisites") == 0) {
        if(!d->Prerequisites)
            d->Prerequisites = calloc(1, sizeof(*d->Prerequisites));
        service_array_node.data = d->Prerequisites;
        upk_svcconf_setup_array_handlers(&service_array_node.handlers);
        upk_json_stack_push(meta, &service_array_node);
    }
    return;
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_svcconf_customaction_array_appender(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_cust_actscr_meta_t *d = data;

    UPKLIST_APPEND(d);

    strncpy(d->thisp->name, key, UPK_MAX_STRING_LEN - 1);
    d->thisp->script = calloc(1, strlen(v.val.str.c_str) + 1);
    strcpy(d->thisp->script, v.val.str.c_str);
    return;
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static inline void
upk_svcconf_setup_object_handlers(upk_json_handlers_t * handlers)
{
    upk_json_handler_t     *restrict json_null = &handlers->json_null;
    upk_json_handler_t     *restrict json_bool = &handlers->json_bool;
    upk_json_handler_t     *restrict json_double = &handlers->json_double;
    upk_json_handler_t     *restrict json_int = &handlers->json_int;
    upk_json_handler_t     *restrict json_string = &handlers->json_string;
    upk_json_handler_t     *restrict json_object = &handlers->json_object;
    upk_json_handler_t     *restrict json_array = &handlers->json_array;
    upk_json_handler_t     *restrict after_json_obj_pop = &handlers->after_json_obj_pop;
    upk_json_handler_t     *restrict after_json_array_pop = &handlers->after_json_array_pop;

    *json_null = NULL;
    *json_bool = upk_conf_error_handler;
    *json_double = upk_conf_error_handler;
    *json_int = upk_conf_error_handler;
    *json_string = upk_svcconf_customaction_array_appender;
    *json_array = upk_conf_error_handler;
    *json_object = upk_conf_error_handler;
    *after_json_obj_pop = NULL;
    *after_json_array_pop = NULL;
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_svcconf_nested_object_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_svc_desc_t         *d = data;
    upk_json_stack_node_t   service_array_node = { 0 };

    if(strcasecmp(key, "CustomActions") == 0) {
        if(!d->CustomActions)
            d->CustomActions = calloc(1, sizeof(*d->CustomActions));
        service_array_node.data = d->CustomActions;
        upk_svcconf_setup_object_handlers(&service_array_node.handlers);
        upk_json_stack_push(meta, &service_array_node);
    }
    return;
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static inline void
upk_svcconf_setup_handlers(upk_json_handlers_t * handlers)
{
    upk_json_handler_t     *restrict json_null = &handlers->json_null;
    upk_json_handler_t     *restrict json_bool = &handlers->json_bool;
    upk_json_handler_t     *restrict json_double = &handlers->json_double;
    upk_json_handler_t     *restrict json_int = &handlers->json_int;
    upk_json_handler_t     *restrict json_string = &handlers->json_string;
    upk_json_handler_t     *restrict json_object = &handlers->json_object;
    upk_json_handler_t     *restrict json_array = &handlers->json_array;
    upk_json_handler_t     *restrict after_json_obj_pop = &handlers->after_json_obj_pop;
    upk_json_handler_t     *restrict after_json_array_pop = &handlers->after_json_array_pop;

    *json_null = NULL;
    *json_bool = upk_svcconf_bool_handler;
    *json_double = upk_conf_error_handler;
    *json_int = upk_svcconf_int_handler;
    *json_string = upk_svcconf_string_handler;
    *json_array = upk_svcconf_array_handler;
    *json_object = upk_svcconf_nested_object_handler;
    *after_json_obj_pop = NULL;
    *after_json_array_pop = NULL;
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_ctrlconf_object_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_controller_config_t *d = data;

    upk_json_stack_node_t   service_node = {
        .data = &d->ServiceDefaults,
    };
    upk_svcconf_setup_handlers(&service_node.handlers);

    if(strcasecmp(key, "ServiceDefaults") == 0) {
        upk_parse_svc_id(key, &d->ServiceDefaults);
        upk_json_stack_push(meta, &service_node);
    }
    return;
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_ctrlconf_string_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_controller_config_t *d = data;

    if(strcasecmp(key, "StateDir") == 0)
        strncpy(d->StateDir, v.val.str.c_str, UPK_MAX_STRING_LEN - 1);
    else if(strcasecmp(key, "SvcConfigPath") == 0)
        strncpy(d->SvcConfigPath, v.val.str.c_str, UPK_MAX_STRING_LEN - 1);
    else if(strcasecmp(key, "SvcRunPath") == 0)
        strncpy(d->SvcRunPath, v.val.str.c_str, UPK_MAX_STRING_LEN - 1);
    else if(strcasecmp(key, "UpkBuddyPath") == 0)
        strncpy(d->UpkBuddyPath, v.val.str.c_str, UPK_MAX_STRING_LEN - 1);
    else
        upk_fatal("invalid Configuration: %s\n", key);
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_ctrlconf_int_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_controller_config_t *d = data;

    if(strcasecmp(key, "BuddyVerbosity") == 0)
        d->BuddyVerbosity = v.val.i;
    else if(strcasecmp(key, "BuddyPollingInterval") == 0)
        d->BuddyPollingInterval = v.val.i;
    else
        upk_fatal("invalid Configuration: %s\n", key);
}



/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_ctrlconf_double_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_controller_config_t *d = data;

    if(strcasecmp(key, "BuddyPollingInterval") == 0) {
        d->BuddyPollingInterval = v.val.dbl;
    } else {
        upk_fatal("Invalid configuration: %s\n", key);
    }
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_ctrlconf_toplvl_obj(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_json_stack_node_t   ctrl_node = {.data = data };

    upk_json_handler_t     *restrict json_null = &ctrl_node.handlers.json_null;
    upk_json_handler_t     *restrict json_bool = &ctrl_node.handlers.json_bool;
    upk_json_handler_t     *restrict json_double = &ctrl_node.handlers.json_double;
    upk_json_handler_t     *restrict json_int = &ctrl_node.handlers.json_int;
    upk_json_handler_t     *restrict json_string = &ctrl_node.handlers.json_string;
    upk_json_handler_t     *restrict json_object = &ctrl_node.handlers.json_object;
    upk_json_handler_t     *restrict json_array = &ctrl_node.handlers.json_array;
    upk_json_handler_t     *restrict after_json_obj_pop = &ctrl_node.handlers.after_json_obj_pop;
    upk_json_handler_t     *restrict after_json_array_pop = &ctrl_node.handlers.after_json_array_pop;

    *json_null = upk_conf_error_handler;
    *json_bool = upk_conf_error_handler;
    *json_double = upk_ctrlconf_double_handler;
    *json_int = upk_ctrlconf_int_handler;
    *json_string = upk_ctrlconf_string_handler;
    *json_array = upk_conf_error_handler;
    *json_object = upk_ctrlconf_object_handler;
    *after_json_obj_pop = NULL;
    *after_json_array_pop = NULL;

    upk_json_stack_push(meta, &ctrl_node);
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_svcconf_object_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_controller_config_t *d = data;
    upk_json_stack_node_t   service_node = { 0 };

    upk_svcconf_setup_handlers(&service_node.handlers);

    if(!d->svclist)
        d->svclist = calloc(1, sizeof(*d->svclist));

    UPKLIST_APPEND(d->svclist);
    service_node.data = d->svclist->thisp;

    upk_svc_desc_clear(d->svclist->thisp);

    upk_parse_svc_id(key, d->svclist->thisp);
    upk_json_stack_push(meta, &service_node);
    return;
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_svcconf_toplvl_obj(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_json_stack_node_t   svc_node = {.data = data };

    upk_json_handler_t     *restrict json_null = &svc_node.handlers.json_null;
    upk_json_handler_t     *restrict json_bool = &svc_node.handlers.json_bool;
    upk_json_handler_t     *restrict json_double = &svc_node.handlers.json_double;
    upk_json_handler_t     *restrict json_int = &svc_node.handlers.json_int;
    upk_json_handler_t     *restrict json_string = &svc_node.handlers.json_string;
    upk_json_handler_t     *restrict json_object = &svc_node.handlers.json_object;
    upk_json_handler_t     *restrict json_array = &svc_node.handlers.json_array;
    upk_json_handler_t     *restrict after_json_obj_pop = &svc_node.handlers.after_json_obj_pop;
    upk_json_handler_t     *restrict after_json_array_pop = &svc_node.handlers.after_json_array_pop;

    *json_null = upk_conf_error_handler;
    *json_bool = upk_conf_error_handler;
    *json_double = upk_conf_error_handler;
    *json_int = upk_conf_error_handler;
    *json_string = upk_conf_error_handler;
    *json_array = upk_conf_error_handler;
    *json_object = upk_svcconf_object_handler;
    *after_json_obj_pop = NULL;
    *after_json_array_pop = NULL;

    upk_json_stack_push(meta, &svc_node);
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_ctrlconf_pack(upk_controller_config_t * cfg, const char *json_string)
{
    struct json_object     *obj = NULL;
    upk_json_stack_node_t   init_node = {.data = cfg };
    upk_json_handler_t     *restrict json_nullh = &init_node.handlers.json_null;
    upk_json_handler_t     *restrict json_boolh = &init_node.handlers.json_bool;
    upk_json_handler_t     *restrict json_doubleh = &init_node.handlers.json_double;
    upk_json_handler_t     *restrict json_inth = &init_node.handlers.json_int;
    upk_json_handler_t     *restrict json_stringh = &init_node.handlers.json_string;
    upk_json_handler_t     *restrict json_objecth = &init_node.handlers.json_object;
    upk_json_handler_t     *restrict json_arrayh = &init_node.handlers.json_array;
    upk_json_handler_t     *restrict after_json_obj_poph = &init_node.handlers.after_json_obj_pop;
    upk_json_handler_t     *restrict after_json_array_poph = &init_node.handlers.after_json_array_pop;
    upk_json_stack_meta_t  *meta = calloc(1, sizeof(*meta));

    *json_nullh = upk_conf_error_handler;
    *json_boolh = upk_conf_error_handler;
    *json_doubleh = upk_conf_error_handler;
    *json_inth = upk_conf_error_handler;
    *json_stringh = upk_conf_error_handler;
    *json_arrayh = upk_conf_error_handler;
    *json_objecth = upk_ctrlconf_toplvl_obj;
    *after_json_obj_poph = NULL;
    *after_json_array_poph = NULL;

    upk_json_stack_push(meta, &init_node);

    obj = upk_json_parse_string(json_string);
    if(obj) {
        upk_json_parse_node(meta, NULL, obj);
        json_object_put(obj);
    }

    UPKLIST_FREE(meta);

    return;
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_svcconf_pack(upk_controller_config_t * cfg, const char *json_string)
{
    struct json_object     *obj = NULL;
    upk_json_stack_node_t   init_node = {.data = cfg };
    upk_json_handler_t     *restrict json_nullh = &init_node.handlers.json_null;
    upk_json_handler_t     *restrict json_boolh = &init_node.handlers.json_bool;
    upk_json_handler_t     *restrict json_doubleh = &init_node.handlers.json_double;
    upk_json_handler_t     *restrict json_inth = &init_node.handlers.json_int;
    upk_json_handler_t     *restrict json_stringh = &init_node.handlers.json_string;
    upk_json_handler_t     *restrict json_objecth = &init_node.handlers.json_object;
    upk_json_handler_t     *restrict json_arrayh = &init_node.handlers.json_array;
    upk_json_handler_t     *restrict after_json_obj_poph = &init_node.handlers.after_json_obj_pop;
    upk_json_handler_t     *restrict after_json_array_poph = &init_node.handlers.after_json_array_pop;

    *json_nullh = upk_conf_error_handler;
    *json_boolh = upk_conf_error_handler;
    *json_doubleh = upk_conf_error_handler;
    *json_inth = upk_conf_error_handler;
    *json_stringh = upk_conf_error_handler;
    *json_arrayh = upk_conf_error_handler;
    *json_objecth = upk_svcconf_toplvl_obj;
    *after_json_obj_poph = NULL;
    *after_json_array_poph = NULL;

    upk_json_stack_meta_t  *meta = calloc(1, sizeof(*meta));

    upk_json_stack_push(meta, &init_node);

    obj = upk_json_parse_string(json_string);
    if(obj) {
        upk_json_parse_node(meta, NULL, obj);
        json_object_put(obj);
    }

    UPKLIST_FREE(meta);
    return;
}



/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_svcconf_bool_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_svc_desc_t         *d = data;

    if((strcasecmp(key, "RandomizeRateLimit") == 0))
        d->RandomizeRateLimit = v.val.bl;
    else if(strcasecmp(key, "UnconfigureOnFileRemoval") == 0)
        d->UnconfigureOnFileRemoval = v.val.bl;
    else if(strcasecmp(key, "PreferBuddyStateForStopped") == 0)
        d->PreferBuddyStateForStopped = v.val.bl;
    else if(strcasecmp(key, "PreferBuddyStateForRunning") == 0)
        d->PreferBuddyStateForRunning = v.val.bl;
    else if(strcasecmp(key, "UnconfigureOnFileRemoval") == 0)
        d->UnconfigureOnFileRemoval = v.val.bl;
    else
        upk_alert("invalid Configuration: %s", key);
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_svcconf_int_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_svc_desc_t         *d = data;

    if((strcasecmp(key, "StartPriority") == 0))
        d->StartPriority = v.val.i;
    else if(strcasecmp(key, "KillTimeout") == 0)
        d->KillTimeout = v.val.i;
    else if(strcasecmp(key, "MaxConsecutiveFailures") == 0)
        d->MaxConsecutiveFailures = v.val.i;
    else if(strcasecmp(key, "UserMaxRestarts") == 0)
        d->UserMaxRestarts = v.val.i;
    else if(strcasecmp(key, "UserRestartWindow") == 0)
        d->UserRestartWindow = v.val.i;
    else if(strcasecmp(key, "UserRateLimit") == 0)
        d->UserRateLimit = v.val.i;
    else if(strcasecmp(key, "SetUID") == 0)
        d->SetUID = v.val.i;
    else if(strcasecmp(key, "SetGID") == 0)
        d->SetGID = v.val.i;
    else if(strcasecmp(key, "RingbufferSize") == 0)
        d->RingbufferSize = v.val.i;
    else if(strcasecmp(key, "ReconnectRetries") == 0)
        d->ReconnectRetries = v.val.i;
    else
        upk_alert("invalid Configuration: %s", key);
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_svcconf_string_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_svc_desc_t         *d = data;
    struct passwd           pwbuf;
    struct passwd          *pwbufp = &pwbuf;

#ifdef _SC_GETPW_R_SIZE_MAX
    char                   *buf = NULL;
    size_t                  buflen = sysconf(_SC_GETPW_R_SIZE_MAX);
#else
    char                    buf[2048] = "";
    size_t                  buflen = sizeof(buf);
#endif

    if((strcasecmp(key, "Package") == 0)) {
        strncpy(d->Package, v.val.str.c_str, UPK_MAX_STRING_LEN - 1);
    } else if(strcasecmp(key, "Name") == 0) {
        strncpy(d->Name, v.val.str.c_str, UPK_MAX_STRING_LEN - 1);
    } else if(strcasecmp(key, "UUID") == 0) {
        upk_string_to_uuid(&d->UUID, v.val.str.c_str);
    } else if(strcasecmp(key, "ShortDescription") == 0) {
        strncpy(d->ShortDescription, v.val.str.c_str, UPK_MAX_STRING_LEN - 1);
    } else if(strcasecmp(key, "LongDescription") == 0) {
        d->LongDescription = calloc(1, strnlen(v.val.str.c_str, 16384) + 1);
        strcpy(d->LongDescription, v.val.str.c_str);
    } else if(strcasecmp(key, "ExecStart") == 0) {
        strncpy(d->ExecStart, v.val.str.c_str, UPK_MAX_PATH_LEN - 1);
    } else if(strcasecmp(key, "StartScript") == 0) {
        d->StartScript = calloc(1, strlen(v.val.str.c_str) + 1);
        strcpy(d->StartScript, v.val.str.c_str);
    } else if(strcasecmp(key, "ExecStop") == 0) {
        strncpy(d->ExecStop, v.val.str.c_str, UPK_MAX_PATH_LEN - 1);
    } else if(strcasecmp(key, "StopScript") == 0) {
        d->StopScript = calloc(1, strlen(v.val.str.c_str) + 1);
        strcpy(d->StopScript, v.val.str.c_str);
    } else if(strcasecmp(key, "ExecReload") == 0) {
        strncpy(d->ExecReload, v.val.str.c_str, UPK_MAX_PATH_LEN - 1);
    } else if(strcasecmp(key, "ReloadScript") == 0) {
        d->ReloadScript = calloc(1, strlen(v.val.str.c_str) + 1);
        strcpy(d->ReloadScript, v.val.str.c_str);
    } else if(strcasecmp(key, "PipeStdoutScript") == 0) {
        d->PipeStdoutScript = calloc(1, strlen(v.val.str.c_str) + 1);
        strcpy(d->PipeStdoutScript, v.val.str.c_str);
    } else if(strcasecmp(key, "PipeStderrScript") == 0) {
        d->PipeStderrScript = calloc(1, strlen(v.val.str.c_str) + 1);
        strcpy(d->PipeStderrScript, v.val.str.c_str);
    } else if(strcasecmp(key, "RedirectStdout") == 0) {
        strncpy(d->RedirectStdout, v.val.str.c_str, UPK_MAX_PATH_LEN - 1);
    } else if(strcasecmp(key, "RedirectStderr") == 0) {
        strncpy(d->RedirectStderr, v.val.str.c_str, UPK_MAX_PATH_LEN - 1);
    } else if(strcasecmp(key, "InitialState") == 0) {
        if((strcasecmp(v.val.str.c_str, "running") == 0) || (strcasecmp(v.val.str.c_str, "up") == 0)
           || (strcasecmp(v.val.str.c_str, "start") == 0)
           || (strcasecmp(v.val.str.c_str, "run") == 0)) {
            d->InitialState = UPK_STATE_RUNNING;
        } else {
            d->InitialState = UPK_STATE_SHUTDOWN;
        }
    } else if(strcasecmp(key, "SetUID") == 0) {
#ifdef _SC_GETPW_R_SIZE_MAX
        buf = calloc(1, sysconf(_SC_GETPW_R_SIZE_MAX));
#endif
        getpwnam_r(v.val.str.c_str, &pwbuf, buf, buflen, &pwbufp);
        if(pwbufp)
            d->SetUID = pwbuf.pw_uid;
#ifdef _SC_GETPW_R_SIZE_MAX
        free(buf);
#endif
    } else if(strcasecmp(key, "SetGID") == 0) {
#ifdef _SC_GETPW_R_SIZE_MAX
        buf = calloc(1, sysconf(_SC_GETPW_R_SIZE_MAX));
#endif
        getpwnam_r(v.val.str.c_str, &pwbuf, buf, buflen, &pwbufp);
        if(pwbufp)
            d->SetGID = pwbuf.pw_gid;
#ifdef _SC_GETPW_R_SIZE_MAX
        free(buf);
#endif
    } else {
        upk_alert("invalid Configuration: %s", key);
    }
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static void
upk_conf_error_handler(upk_json_stack_meta_t * meta, void *data, char *key, upk_json_val_t v)
{
    upk_alert("invalid Configuration: %s\n", key);
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static char            *
upk_config_loadfile(const char *filename)
{
    FILE                   *conf;
    char                    buf[UPK_MAX_STRING_LEN] = "";
    char                   *json_string = NULL;
    size_t                  len = 0, add = 0, size = 64;

    json_string = calloc(1, size);

    errno = 0;
    if((conf = fopen(filename, "r"))) {
        while(fgets(buf, sizeof(buf) - 1, conf)) {
            len += (add = strnlen(buf, sizeof(buf)));
            if(len > size) {
                size *= 2;
                json_string = realloc(json_string, size + 1);
            }
            strncat(json_string, buf, add);
        }
        fclose(conf);
    } else {
        upk_warn("Could not open: %s: %s Using default configuration\n", filename, strerror(errno));
        upk_debug1("%s\n", upk_default_configuration_str);
    }

    return json_string;
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_ctrl_load_config(void)
{
    char                   *json;

    upk_svc_desc_clear(&upk_default_configuration.ServiceDefaults);
    upk_ctrlconf_pack(&upk_default_configuration, upk_default_configuration_str);

    strncpy(upk_default_configuration.controller_socket, upk_default_configuration.StateDir, UPK_MAX_STRING_LEN - 1);
    strcat(upk_default_configuration.controller_socket, "/.controler.sock");

    strncpy(upk_default_configuration.controller_buddy_sock, upk_default_configuration.StateDir, UPK_MAX_STRING_LEN - 1);
    strcat(upk_default_configuration.controller_buddy_sock, "/.buddy.sock");

    upk_file_configuration.BuddyPollingInterval = DBL_MIN;

    if(strlen(upk_ctrl_configuration_file) > 0) {
        json = upk_config_loadfile(upk_ctrl_configuration_file);
        if(json && strlen(json) > 0) {
            upk_svc_desc_clear(&upk_file_configuration.ServiceDefaults);
            upk_ctrlconf_pack(&upk_file_configuration, json);
        }
        free(json);
    }
    upk_svc_desc_clear(&upk_runtime_configuration.ServiceDefaults);
    upk_overlay_ctrlconf_values(&upk_runtime_configuration, &upk_default_configuration);
    upk_overlay_ctrlconf_values(&upk_runtime_configuration, &upk_file_configuration);
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_svc_desc_clear(upk_svc_desc_t * svc)
{
    upk_svc_desc_t          empty = {
        .StartPriority = INT32_MIN,
        .KillTimeout = INT32_MIN,
        .MaxConsecutiveFailures = INT32_MIN,
        .UserMaxRestarts = INT32_MIN,
        .UserRestartWindow = INT32_MIN,
        .UserRateLimit = INT32_MIN,
        .RandomizeRateLimit = INT8_MIN,
        .SetUID = INT32_MIN,
        .SetGID = INT32_MIN,
        .RingbufferSize = INT32_MIN,
        .ReconnectRetries = INT32_MIN,
        .UnconfigureOnFileRemoval = INT8_MIN,
        .PreferBuddyStateForStopped = INT8_MIN,
        .PreferBuddyStateForRunning = INT8_MIN,
    };
    memcpy(svc, &empty, sizeof(*svc) - sizeof(svc->next));
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
static inline void     *
upk_json_serialize_or_null(enum json_type type, void *val)
{
    int32_t                 cmpint = 0;
    int8_t                  cmpbyte = 0;
    struct lh_table        *table = NULL;

    switch (type) {
    case json_type_string:
        if(val && strlen(val))
            return json_object_new_string(val);
        break;
    case json_type_array:
        if(val && json_object_array_length(val) > 0)
            return val;
        json_object_put(val);
        break;
    case json_type_int:
        cmpint = *((int32_t *) val);
        if(cmpint != INT32_MIN)
            return json_object_new_int(cmpint);
        break;
    case json_type_boolean:
        cmpbyte = *((int8_t *) val);
        if(cmpbyte != INT8_MIN)
            return json_object_new_boolean(cmpbyte);
        break;
    case json_type_object:
        table = json_object_get_object(val);
        if(table->count > 0)
            return val;
        json_object_put(val);
        break;
    default:
        break;
    }
    return NULL;
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
char                   *
upk_concat_svcid(char *dest, const char *pkg, const char *name)
{
    size_t                  remainder = UPK_MAX_STRING_LEN - 1, len = 0;

    memset(dest, 0, UPK_MAX_STRING_LEN);
    if(pkg && (len = strlen(pkg))) {
        strncpy(dest, pkg, remainder);
        remainder -= len;
        strncat(dest, "::", remainder);
        remainder -= 2;
    }

    strncat(dest, name, remainder);
    return dest;
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
char                   *
upk_svc_id(char *dest, upk_svc_desc_t * svc)
{
    return upk_concat_svcid(dest, svc->Package, svc->Name);
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_parse_svc_id(char *key, upk_svc_desc_t * svc)
{
    char                   *p;

    if((p = strstr(key, "::"))) {
        *p = '\0';
        memset(svc->Package, 0, sizeof(svc->Package));
        strncpy(svc->Package, key, sizeof(svc->Package) - 1);
        memset(svc->Name, 0, sizeof(svc->Name));
        strncpy(svc->Name, p + 2, sizeof(svc->Name) - 1);
    } else {
        strncpy(svc->Name, key, sizeof(svc->Name) - 1);
    }
}

#define _joa json_object_object_add
#define jt_string json_type_string
#define jt_int json_type_int
#define jt_boolean json_type_boolean
#define jt_array json_type_array


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
struct json_object     *
upk_svclist_to_json_obj(upk_svc_desc_meta_t * svclist)
{
    struct json_object     *top, *p;
    char                    idbuf[UPK_MAX_STRING_LEN] = "";

    if(!svclist)
        return NULL;

    top = json_object_new_object();

    UPKLIST_FOREACH(svclist) {
        upk_svc_id(idbuf, svclist->thisp);
        p = upk_svc_desc_to_json_obj(svclist->thisp);
        _joa(top, idbuf, p);
    }

    return top;
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
struct json_object     *
upk_svc_desc_to_json_obj(upk_svc_desc_t * svc)
{
    struct json_object     *obj, *p;
    char                    uuid_buf[37] = "";
    char                    svc_id_buf[UPK_MAX_STRING_LEN] = "";

    obj = json_object_new_object();

    json_object_object_add(obj, "Package", upk_json_serialize_or_null(jt_string, svc->Package));

    p = json_object_new_array();
    if(svc->Provides) {
        UPKLIST_FOREACH(svc->Provides) {
            upk_concat_svcid(svc_id_buf, svc->Provides->thisp->pkg, svc->Provides->thisp->name);
            json_object_array_add(p, json_object_new_string(svc_id_buf));
        }
    }
    _joa(obj, "Provides", upk_json_serialize_or_null(jt_array, p));

    upk_string_to_uuid(&svc->UUID, uuid_buf);
    _joa(obj, "UUID", upk_json_serialize_or_null(jt_string, uuid_buf));
    _joa(obj, "ShortDescription", upk_json_serialize_or_null(jt_string, svc->ShortDescription));
    _joa(obj, "LongDescription", upk_json_serialize_or_null(jt_string, svc->LongDescription));

    p = json_object_new_array();
    if(svc->Prerequisites) {
        UPKLIST_FOREACH(svc->Prerequisites) {
            upk_concat_svcid(svc_id_buf, svc->Prerequisites->thisp->pkg, svc->Prerequisites->thisp->name);
            json_object_array_add(p, json_object_new_string(svc_id_buf));
        }
    }
    _joa(obj, "Prerequisites", upk_json_serialize_or_null(jt_array, p));

    _joa(obj, "StartPriority", upk_json_serialize_or_null(jt_int, &svc->StartPriority));
    _joa(obj, "KillTimeout", upk_json_serialize_or_null(jt_int, &svc->KillTimeout));
    _joa(obj, "MaxConsecutiveFailures", upk_json_serialize_or_null(jt_int, &svc->MaxConsecutiveFailures));
    _joa(obj, "UserMaxRestarts", upk_json_serialize_or_null(jt_int, &svc->UserMaxRestarts));
    _joa(obj, "UserRestartWindow", upk_json_serialize_or_null(jt_int, &svc->UserRestartWindow));
    _joa(obj, "UserRateLimit", upk_json_serialize_or_null(jt_int, &svc->UserRateLimit));
    _joa(obj, "RandomizeRateLimit", upk_json_serialize_or_null(jt_boolean, &svc->RandomizeRateLimit));
    _joa(obj, "SetUID", upk_json_serialize_or_null(jt_int, &svc->SetUID));
    _joa(obj, "SetGID", upk_json_serialize_or_null(jt_int, &svc->SetGID));
    _joa(obj, "RingbufferSize", upk_json_serialize_or_null(jt_int, &svc->RingbufferSize));
    _joa(obj, "ReconnectRetries", upk_json_serialize_or_null(jt_int, &svc->ReconnectRetries));
    _joa(obj, "ExecStart", upk_json_serialize_or_null(jt_string, svc->ExecStart));
    _joa(obj, "StartScript", upk_json_serialize_or_null(jt_string, svc->StartScript));
    _joa(obj, "ExecStop", upk_json_serialize_or_null(jt_string, svc->ExecStop));
    _joa(obj, "StopScript", upk_json_serialize_or_null(jt_string, svc->StopScript));
    _joa(obj, "ExecReload", upk_json_serialize_or_null(jt_string, svc->ExecReload));
    _joa(obj, "ReloadScript", upk_json_serialize_or_null(jt_string, svc->ReloadScript));

    p = json_object_new_object();
    if(svc->CustomActions) {
        UPKLIST_FOREACH(svc->CustomActions) {
            _joa(p, svc->CustomActions->thisp->name, json_object_new_string(svc->CustomActions->thisp->script));
        }
    }
    _joa(obj, "CustomActions", upk_json_serialize_or_null(json_type_object, p));

    _joa(obj, "PipeStdoutScript", upk_json_serialize_or_null(jt_string, svc->PipeStdoutScript));
    _joa(obj, "PipeStderrScript", upk_json_serialize_or_null(jt_string, svc->PipeStderrScript));
    _joa(obj, "RedirectStdout", upk_json_serialize_or_null(jt_string, svc->RedirectStdout));
    _joa(obj, "RedirectStderr", upk_json_serialize_or_null(jt_string, svc->RedirectStderr));
    _joa(obj, "InitialState",
         ((svc->InitialState ==
           UPK_STATE_RUNNING) ? json_object_new_string("running") : ((svc->InitialState ==
                                                                      UPK_STATE_STOPPED) ? json_object_new_string("stopped")
                                                                     : NULL)));
    _joa(obj, "UnconfigureOnFileRemoval", upk_json_serialize_or_null(jt_boolean, &svc->UnconfigureOnFileRemoval));
    _joa(obj, "PreferBuddyStateForStopped", upk_json_serialize_or_null(jt_boolean, &svc->PreferBuddyStateForStopped));
    _joa(obj, "PreferBuddyStateForRunning", upk_json_serialize_or_null(jt_boolean, &svc->PreferBuddyStateForRunning));

    return obj;
}

#undef _joa
#undef jt_string
#undef jt_int
#undef jt_boolean
#undef jt_array


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
char                   *
upk_json_serialize_svc_config(upk_svc_desc_t * svc, upk_json_data_output_opts_t opts)
{
    struct json_object     *top;
    char                   *cp;
    upk_svc_desc_meta_t    *svclist;

    svclist = calloc(1, sizeof(*svclist));

    UPKLIST_APPEND(svclist);
    memcpy(svclist->thisp, svc, sizeof(*svc) - sizeof(svc->next));

    top = upk_svclist_to_json_obj(svclist);
    cp = upk_json_obj_to_string(top, opts);
    json_object_put(top);

    UPKLIST_FREE(svclist);
    return cp;
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_overlay_svcconf_values(upk_svc_desc_t * dest, upk_svc_desc_t * high)
{
    if(high->StartPriority != INT32_MIN)
        dest->StartPriority = high->StartPriority;

    if(strlen(high->Name) > 0) {
        memset(dest->Name, 0, sizeof(dest->Name));
        strncpy(dest->Name, high->Name, sizeof(dest->Name) - 1);
    }

    if(strlen(high->Package) > 0) {
        memset(dest->Package, 0, sizeof(dest->Package));
        strncpy(dest->Package, high->Package, sizeof(dest->Package) - 1);
    }

    if(high->Provides && high->Provides->count > 0) {
        if(!dest->Provides)
            dest->Provides = calloc(1, sizeof(*dest->Provides));

        UPKLIST_FOREACH(high->Provides) {
            UPKLIST_APPEND(dest->Provides);
            memcpy(dest->Provides->thisp, high->Provides->thisp, sizeof(*dest->Provides->thisp) - sizeof(dest->Provides->thisp->next));
        }
    }

    if(high->UUID.time_low || high->UUID.time_mid || high->UUID.time_high_and_version || high->UUID.clk_seq_high || high->UUID.clk_seq_low
       || high->UUID.node[0] || high->UUID.node[1]
       || high->UUID.node[2] || high->UUID.node[3]
       || high->UUID.node[4] || high->UUID.node[5])
        dest->UUID = high->UUID;

    if(strlen(high->ShortDescription) > 0) {
        memset(dest->ShortDescription, 0, sizeof(dest->ShortDescription));
        strncpy(dest->ShortDescription, high->ShortDescription, sizeof(dest->ShortDescription) - 1);
    }

    if(high->LongDescription && strlen(high->LongDescription) > 0) {
        dest->LongDescription = calloc(1, strlen(high->LongDescription) + 1);
        strcpy(dest->LongDescription, high->LongDescription);
    }

    if(high->Prerequisites && high->Prerequisites->count > 0) {
        if(!dest->Prerequisites)
            dest->Prerequisites = calloc(1, sizeof(*dest->Prerequisites));

        UPKLIST_FOREACH(high->Prerequisites) {
            UPKLIST_APPEND(dest->Prerequisites);
            memcpy(dest->Prerequisites->thisp, high->Prerequisites->thisp,
                   sizeof(*dest->Prerequisites->thisp) - sizeof(dest->Prerequisites->thisp->next));
        }
    }

    if(high->KillTimeout != INT32_MIN)
        dest->KillTimeout = high->KillTimeout;

    if(high->MaxConsecutiveFailures != INT32_MIN)
        dest->MaxConsecutiveFailures = high->MaxConsecutiveFailures;

    if(high->UserMaxRestarts != INT32_MIN)
        dest->UserMaxRestarts = high->UserMaxRestarts;

    if(high->UserRestartWindow != INT32_MIN)
        dest->UserRestartWindow = high->UserRestartWindow;

    if(high->UserRateLimit != INT32_MIN)
        dest->UserRateLimit = high->UserRateLimit;

    if(high->RandomizeRateLimit != INT8_MIN)
        dest->RandomizeRateLimit = high->RandomizeRateLimit;

    if(high->SetUID != INT32_MIN)
        dest->SetUID = high->SetUID;

    if(high->SetGID != INT32_MIN)
        dest->SetGID = high->SetGID;

    if(high->RingbufferSize != INT32_MIN)
        dest->RingbufferSize = high->RingbufferSize;

    if(high->ReconnectRetries != INT32_MIN)
        dest->ReconnectRetries = high->ReconnectRetries;

    if(strlen(high->ExecStart) > 0) {
        memset(dest->ExecStart, 0, sizeof(dest->ExecStart));
        strncpy(dest->ExecStart, high->ExecStart, sizeof(dest->ExecStart) - 1);
    }

    if(high->StartScript && strlen(high->StartScript) > 0) {
        if(dest->StartScript)
            free(dest->StartScript);
        dest->StartScript = calloc(1, strlen(high->StartScript) + 1);
        strcpy(dest->StartScript, high->StartScript);
    }

    if(strlen(high->ExecStop) > 0) {
        memset(dest->ExecStop, 0, sizeof(dest->ExecStop));
        strncpy(dest->ExecStop, high->ExecStop, sizeof(dest->ExecStop) - 1);
    }

    if(high->StopScript && strlen(high->StopScript) > 0) {
        if(dest->StopScript)
            free(dest->StopScript);
        dest->StopScript = calloc(1, strlen(high->StopScript) + 1);
        strcpy(dest->StopScript, high->StopScript);
    }

    if(strlen(high->ExecReload) > 0) {
        memset(dest->ExecReload, 0, sizeof(dest->ExecReload));
        strncpy(dest->ExecReload, high->ExecReload, sizeof(dest->ExecReload) - 1);
    }

    if(high->ReloadScript && strlen(high->ReloadScript) > 0) {
        if(dest->ReloadScript)
            free(dest->ReloadScript);
        dest->ReloadScript = calloc(1, strlen(high->ReloadScript) + 1);
        strcpy(dest->ReloadScript, high->ReloadScript);
    }

    if(high->CustomActions && high->CustomActions->count > 0) {
        if(dest->CustomActions) {
            UPKLIST_FOREACH(dest->CustomActions) {
                if(dest->CustomActions->thisp->script) {
                    free(dest->CustomActions->thisp->script);
                    dest->CustomActions->thisp->script = NULL;
                }
                UPKLIST_UNLINK(dest->CustomActions);
                //UPKLIST_FREE(dest->CustomActions);
            }
        }
        dest->CustomActions = calloc(1, sizeof(*dest->CustomActions)); 

        UPKLIST_FOREACH(high->CustomActions) {
            UPKLIST_APPEND(dest->CustomActions);
            memcpy(dest->CustomActions->thisp, high->CustomActions->thisp,
                   sizeof(*dest->CustomActions->thisp) - sizeof(dest->CustomActions->thisp->next));
            if(high->CustomActions->thisp->script) { 
                dest->CustomActions->thisp->script = calloc(1, strlen(high->CustomActions->thisp->script) + 1);
                strcpy(dest->CustomActions->thisp->script, high->CustomActions->thisp->script);
            }
        }
    }

    if(high->PipeStdoutScript && strlen(high->PipeStdoutScript) > 0) {
        if(dest->PipeStdoutScript)
            free(dest->PipeStdoutScript);
        dest->PipeStdoutScript = calloc(1, strlen(high->PipeStdoutScript) + 1);
        strcpy(dest->PipeStdoutScript, high->PipeStdoutScript);
    }

    if(high->PipeStderrScript && strlen(high->PipeStderrScript) > 0) {
        if(dest->PipeStdoutScript)
            free(dest->PipeStdoutScript);
        dest->PipeStderrScript = calloc(1, strlen(high->PipeStderrScript) + 1);
        strcpy(dest->PipeStderrScript, high->PipeStderrScript);
    }

    if(strlen(high->RedirectStdout) > 0) {
        memset(dest->RedirectStdout, 0, sizeof(dest->RedirectStdout));
        strncpy(dest->RedirectStdout, high->RedirectStdout, sizeof(dest->RedirectStdout) - 1);
    }

    if(strlen(high->RedirectStderr) > 0) {
        memset(dest->RedirectStderr, 0, sizeof(dest->RedirectStderr));
        strncpy(dest->RedirectStderr, high->RedirectStderr, sizeof(dest->RedirectStderr) - 1);
    }

    if(high->InitialState)
        dest->InitialState = high->InitialState;

    if(high->UnconfigureOnFileRemoval != INT8_MIN)
        dest->UnconfigureOnFileRemoval = high->UnconfigureOnFileRemoval;

    if(high->PreferBuddyStateForStopped != INT8_MIN)
        dest->PreferBuddyStateForStopped = high->PreferBuddyStateForStopped;

    if(high->PreferBuddyStateForRunning != INT8_MIN)
        dest->PreferBuddyStateForRunning = high->PreferBuddyStateForRunning;
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_overlay_ctrlconf_values(upk_controller_config_t * dest, upk_controller_config_t * high)
{
    if(strlen(high->StateDir) > 0) {
        memset(dest->StateDir, 0, sizeof(dest->StateDir));
        strncpy(dest->StateDir, high->StateDir, sizeof(dest->StateDir) - 1);
    }

    if(strlen(high->SvcConfigPath) > 0) {
        memset(dest->SvcConfigPath, 0, sizeof(dest->SvcConfigPath));
        strncpy(dest->SvcConfigPath, high->SvcConfigPath, sizeof(dest->SvcConfigPath) - 1);
    }

    if(strlen(high->SvcRunPath) > 0) {
        memset(dest->SvcRunPath, 0, sizeof(dest->SvcRunPath));
        strncpy(dest->SvcRunPath, high->SvcRunPath, sizeof(dest->SvcRunPath) - 1);
    }

    if(strlen(high->UpkBuddyPath) > 0) {
        memset(dest->UpkBuddyPath, 0, sizeof(dest->UpkBuddyPath));
        strncpy(dest->UpkBuddyPath, high->UpkBuddyPath, sizeof(dest->UpkBuddyPath) - 1);
    }

    if(high->BuddyPollingInterval != DBL_MIN)
        dest->BuddyPollingInterval = high->BuddyPollingInterval;

    if(strlen(high->controller_socket) > 0) {
        memset(dest->controller_socket, 0, sizeof(dest->controller_socket));
        strncpy(dest->controller_socket, high->controller_socket, sizeof(dest->controller_socket) - 1);
    }

    if(strlen(high->controller_buddy_sock) > 0) {
        memset(dest->controller_buddy_sock, 0, sizeof(dest->controller_buddy_sock));
        strncpy(dest->controller_buddy_sock, high->controller_buddy_sock, sizeof(dest->controller_buddy_sock) - 1);
    }

    if(strlen(high->ServiceDefaults.Name) > 0)
        upk_overlay_svcconf_values(&dest->ServiceDefaults, &high->ServiceDefaults);

    if(high->svclist && high->svclist->count > 0) {
        if(dest->svclist)
            UPKLIST_FREE(dest->svclist);
        dest->svclist = calloc(1, sizeof(*dest->svclist));

        UPKLIST_FOREACH(high->svclist) {
            UPKLIST_APPEND(dest->svclist);
            memcpy(dest->svclist->thisp, high->svclist->thisp, sizeof(*dest->svclist->thisp) - sizeof(dest->svclist->thisp->next));
        }
    }
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_finalize_svc_desc(upk_svc_desc_t * dest, upk_svc_desc_t * orig)
{
    upk_overlay_svcconf_values(dest, &upk_runtime_configuration.ServiceDefaults);
    if(orig)
        upk_overlay_svcconf_values(dest, orig);

    if(dest->StartScript)
        upk_replace_string(&dest->StartScript, "%(EXEC_START)", dest->ExecStart);
    if(dest->StopScript)
        upk_replace_string(&dest->StopScript, "%(EXEC_STOP)", dest->ExecStop);
    if(dest->ReloadScript)
        upk_replace_string(&dest->ReloadScript, "%(EXEC_RELOAD)", dest->ExecReload);
}

/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_load_runtime_service_file(const char *filename)
{
}


/* ******************************************************************************************************************
   ****************************************************************************************************************** */
void
upk_load_runtime_services(void)
{
    DIR                    *dir;
    struct dirent          *ent = NULL;
    size_t                  filename_len;
    char                   *filename_suffix = ".conf";
    size_t                  suffix_len = strlen(filename_suffix);
    char                   *p, path_buf[UPK_MAX_PATH_LEN], *pathp;
    upk_json_data_output_opts_t opts = {.sep = "\n",.indent = "    ",.pad = " " };

    strncpy(path_buf, upk_runtime_configuration.SvcConfigPath, sizeof(path_buf) - 1);
    pathp = path_buf + strnlen(path_buf, sizeof(path_buf));

    upk_file_configuration.svclist = calloc(1, sizeof(*upk_file_configuration.svclist));

    if(strnlen(upk_runtime_configuration.SvcConfigPath, sizeof(upk_runtime_configuration.SvcConfigPath)) > 0) {
        errno = 0;
        if((dir = opendir(upk_runtime_configuration.SvcConfigPath))) {
            errno = 0;
            while((ent = readdir(dir))) {
                filename_len = strlen(ent->d_name);
                if(strcmp(ent->d_name + filename_len - suffix_len, filename_suffix) == 0) {
                    *pathp = '\0';
                    strcat(path_buf, "/");
                    strncat(path_buf, ent->d_name, sizeof(path_buf) - 1);

                    upk_info("Opening %s\n", path_buf);
                    p = upk_config_loadfile(path_buf);
                    upk_debug1("JSON:\n%s\n", p);
                    upk_svcconf_pack(&upk_file_configuration, p);
                    free(p);
                }
            }
            closedir(dir);
            if(upk_runtime_configuration.svclist)
                UPKLIST_FREE(upk_runtime_configuration.svclist);
            upk_runtime_configuration.svclist = calloc(1, sizeof(*upk_runtime_configuration.svclist));

            UPKLIST_FOREACH(upk_file_configuration.svclist) {
                UPKLIST_APPEND(upk_runtime_configuration.svclist);
                upk_svc_desc_clear(upk_runtime_configuration.svclist->thisp);
                upk_finalize_svc_desc(upk_runtime_configuration.svclist->thisp, upk_file_configuration.svclist->thisp);
                if(upk_diag_verbosity >= UPK_DIAGLVL_DEBUG1) {
                    p = upk_json_serialize_svc_config(upk_runtime_configuration.svclist->thisp, opts);
                    upk_debug1("Finalized service:\n%s\n", p);
                    free(p);
                }
            }
        } else {
            perror(upk_runtime_configuration.SvcConfigPath);
        }
    }
}



/**
  @}
  */
