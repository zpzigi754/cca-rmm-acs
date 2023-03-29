/*
 * Copyright (c) 2023, Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "val_rmm.h"

enum test_intent {
    RD_UNALIGNED = 0X0,
    RD_DEV_MEM = 0X1,
    RD_OUTSIDE_OF_PERMITTED_PA = 0X2,
    RD_STATE_UNDELEGATED = 0X3,
    RD_STATE_DELEGATED = 0X4,
    RD_STATE_REC = 0X5,
    RD_STATE_RTT = 0X6,
    RD_STATE_DATA = 0X7,
    LEVEL_OOB = 0X8,
    IPA_UNALIGNED = 0X9,
    IPA_OOB = 0XA,
    REALM_ACTIVE = 0XB,
    REALM_SYSTEM_OFF = 0XC,
    REALM_NULL = 0XD,
    IPA_NOT_MAPPED = 0XE,
    RTTE_STATE_TABLE = 0XF,
    RTTE_STATE_ASSIGNED = 0X10,
    RTTE_STATE_DESTROYED = 0X11
};

struct stimulus {
    char msg[100];
    uint64_t abi;
    enum test_intent label;
    uint64_t status;
    uint64_t index;
};

static struct stimulus test_data[] = {
    {.msg = "rd_align",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = RD_UNALIGNED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_bound",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = RD_DEV_MEM,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_bound",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = RD_OUTSIDE_OF_PERMITTED_PA,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_state",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = RD_STATE_UNDELEGATED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_state",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = RD_STATE_DELEGATED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_state",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = RD_STATE_REC,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_state",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = RD_STATE_RTT,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_state",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = RD_STATE_DATA,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "level_bound",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = LEVEL_OOB,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "ipa_align",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = IPA_UNALIGNED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "ipa_bound",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = IPA_OOB,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "realm_state",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = REALM_ACTIVE,
    .status = RMI_ERROR_REALM,
    .index = 0},
    {.msg = "realm_state",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = REALM_SYSTEM_OFF,
    .status = RMI_ERROR_REALM,
    .index = 0},
    {.msg = "realm_state_null",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = REALM_NULL,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rtt_walk",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = IPA_NOT_MAPPED,
    .status = RMI_ERROR_RTT,
    .index = 2},
    {.msg = "rtte_state_0",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = RTTE_STATE_TABLE,
    .status = RMI_ERROR_RTT,
    .index = 2},
    {.msg = "rtte_state_1",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = RTTE_STATE_ASSIGNED,
    .status = RMI_ERROR_RTT,
    .index = 3},
    {.msg = "rtte_state_1",
    .abi = RMI_RTT_INIT_RIPAS,
    .label = RTTE_STATE_DESTROYED,
    .status = RMI_ERROR_RTT,
    .index = 3}
};