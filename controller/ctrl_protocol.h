#ifndef _CTRL_PROTOCOL_H
#define _CTRL_PROTOCOL_H

#include "buddyinc.h"

typedef enum {
    UPK_CTRL_SHUTDOWN = 1,
    UPK_CTRL_STATUS_REQ,
    UPK_CTRL_ACTION_START,
    UPK_CTRL_ACTION_STOP,
    UPK_CTRL_ACTION_RELOAD,
    UPK_CTRL_ACTION_RUNONCE,                               /* ??? */
    UPK_CTRL_CUSTOM_ACTION_01,
    UPK_CTRL_CUSTOM_ACTION_02,
    UPK_CTRL_CUSTOM_ACTION_03,
    UPK_CTRL_CUSTOM_ACTION_04,
    UPK_CTRL_CUSTOM_ACTION_05,
    UPK_CTRL_CUSTOM_ACTION_06,
    UPK_CTRL_CUSTOM_ACTION_07,
    UPK_CTRL_CUSTOM_ACTION_08,
    UPK_CTRL_CUSTOM_ACTION_09,
    UPK_CTRL_CUSTOM_ACTION_10,
    UPK_CTRL_CUSTOM_ACTION_11,
    UPK_CTRL_CUSTOM_ACTION_12,
    UPK_CTRL_CUSTOM_ACTION_13,
    UPK_CTRL_CUSTOM_ACTION_14,
    UPK_CTRL_CUSTOM_ACTION_15,
    UPK_CTRL_CUSTOM_ACTION_16,
    UPK_CTRL_CUSTOM_ACTION_17,
    UPK_CTRL_CUSTOM_ACTION_18,
    UPK_CTRL_CUSTOM_ACTION_19,
    UPK_CTRL_CUSTOM_ACTION_20,
    UPK_CTRL_CUSTOM_ACTION_21,
    UPK_CTRL_CUSTOM_ACTION_22,
    UPK_CTRL_CUSTOM_ACTION_23,
    UPK_CTRL_CUSTOM_ACTION_24,
    UPK_CTRL_CUSTOM_ACTION_25,
    UPK_CTRL_CUSTOM_ACTION_26,
    UPK_CTRL_CUSTOM_ACTION_27,
    UPK_CTRL_CUSTOM_ACTION_28,
    UPK_CTRL_CUSTOM_ACTION_29,
    UPK_CTRL_CUSTOM_ACTION_30,
    UPK_CTRL_CUSTOM_ACTION_31,
    UPK_CTRL_CUSTOM_ACTION_32,
    UPK_CTRL_SIGNAL_01,
    UPK_CTRL_SIGNAL_02,
    UPK_CTRL_SIGNAL_03,
    UPK_CTRL_SIGNAL_04,
    UPK_CTRL_SIGNAL_05,
    UPK_CTRL_SIGNAL_06,
    UPK_CTRL_SIGNAL_07,
    UPK_CTRL_SIGNAL_08,
    UPK_CTRL_SIGNAL_09,
    UPK_CTRL_SIGNAL_10,
    UPK_CTRL_SIGNAL_11,
    UPK_CTRL_SIGNAL_12,
    UPK_CTRL_SIGNAL_13,
    UPK_CTRL_SIGNAL_14,
    UPK_CTRL_SIGNAL_15,
    UPK_CTRL_SIGNAL_16,
    UPK_CTRL_SIGNAL_17,
    UPK_CTRL_SIGNAL_18,
    UPK_CTRL_SIGNAL_19,
    UPK_CTRL_SIGNAL_20,
    UPK_CTRL_SIGNAL_21,
    UPK_CTRL_SIGNAL_22,
    UPK_CTRL_SIGNAL_23,
    UPK_CTRL_SIGNAL_24,
    UPK_CTRL_SIGNAL_25,
    UPK_CTRL_SIGNAL_26,
    UPK_CTRL_SIGNAL_27,
    UPK_CTRL_SIGNAL_28,
    UPK_CTRL_SIGNAL_29,
    UPK_CTRL_SIGNAL_30,
    UPK_CTRL_SIGNAL_31,
    UPK_CTRL_SIGNAL_32,
} buddy_commands_t;


typedef struct _buddy_info buddy_info_t;
struct _buddy_info {
    bool                    has_data;
    pid_t                   service_pid;
    buddy_commands_t        command;
    siginfo_t               siginfo;
    int                     wait_status;
    time_t                  timestamp;
    buddy_info_t           *next;
};


#endif
