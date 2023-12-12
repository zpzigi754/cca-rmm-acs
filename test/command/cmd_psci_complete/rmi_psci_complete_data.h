/*
 * Copyright (c) 2023, Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "val_rmm.h"

enum test_intent {
    ALIAS = 0X0,
    CALLING_UNALIGNED = 0x1,
    CALLING_DEV_MEM = 0x2,
    CALLING_OUT_OF_PERMITTED_PA = 0x3,
    CALLING_GRAN_STATE_UNDELEGATED = 0x4,
    CALLING_GRAN_STATE_DELEGATED = 0x5,
    CALLING_GRAN_STATE_RD = 0x6,
    CALLING_GRAN_STATE_RTT = 0x7,
    CALLING_GRAN_STATE_DATA = 0x8,
    TARGET_UNALIGNED = 0x9,
    TARGET_DEV_MEM = 0xA,
    TARGET_OUT_OF_PERMITTED_PA = 0xB,
    TARGET_GRAN_STATE_UNDELEGATED = 0xC,
    TARGET_GRAN_STATE_DELEGATED = 0xD,
    TARGET_GRAN_STATE_RD = 0xE,
    TARGET_GRAN_STATE_RTT = 0xF,
    TARGET_GRAN_STATE_DATA = 0x10,
    NO_PSCI_REQUEST = 0x11,
    TARGET_OTHER_OWNER = 0x12,
    TARGET_OTHER_MPIDR = 0x13,
    STATUS_NOT_PERMITTED = 0x14
};

struct stimulus {
    char msg[100];
    uint64_t abi;
    enum test_intent label;
    uint64_t status;
    uint64_t index;
};

static struct stimulus test_data[] = {
    {.msg = "alias",
    .abi = RMI_PSCI_COMPLETE,
    .label = ALIAS,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "calling_align",
    .abi = RMI_PSCI_COMPLETE,
    .label = CALLING_UNALIGNED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "calling_bound",
    .abi = RMI_PSCI_COMPLETE,
    .label = CALLING_DEV_MEM,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "calling_bound",
    .abi = RMI_PSCI_COMPLETE,
    .label = CALLING_OUT_OF_PERMITTED_PA,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "calling state",
    .abi = RMI_PSCI_COMPLETE,
    .label = CALLING_GRAN_STATE_UNDELEGATED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "calling state",
    .abi = RMI_PSCI_COMPLETE,
    .label = CALLING_GRAN_STATE_DELEGATED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "calling state",
    .abi = RMI_PSCI_COMPLETE,
    .label = CALLING_GRAN_STATE_RD,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "calling state",
    .abi = RMI_PSCI_COMPLETE,
    .label = CALLING_GRAN_STATE_RTT,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "calling state",
    .abi = RMI_PSCI_COMPLETE,
    .label = CALLING_GRAN_STATE_DATA,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "target_align",
    .abi = RMI_PSCI_COMPLETE,
    .label = TARGET_UNALIGNED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "target_bound",
    .abi = RMI_PSCI_COMPLETE,
    .label = TARGET_DEV_MEM,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "target_bound",
    .abi = RMI_PSCI_COMPLETE,
    .label = TARGET_OUT_OF_PERMITTED_PA,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "target state",
    .abi = RMI_PSCI_COMPLETE,
    .label = TARGET_GRAN_STATE_UNDELEGATED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "target state",
    .abi = RMI_PSCI_COMPLETE,
    .label = TARGET_GRAN_STATE_DELEGATED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "target state",
    .abi = RMI_PSCI_COMPLETE,
    .label = TARGET_GRAN_STATE_RD,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "target state",
    .abi = RMI_PSCI_COMPLETE,
    .label = TARGET_GRAN_STATE_RTT,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "target state",
    .abi = RMI_PSCI_COMPLETE,
    .label = TARGET_GRAN_STATE_DATA,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "pending",
    .abi = RMI_PSCI_COMPLETE,
    .label = NO_PSCI_REQUEST,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "owner",
    .abi = RMI_PSCI_COMPLETE,
    .label = TARGET_OTHER_OWNER,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "target",
    .abi = RMI_PSCI_COMPLETE,
    .label = TARGET_OTHER_MPIDR,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "status",
    .abi = RMI_PSCI_COMPLETE,
    .label = STATUS_NOT_PERMITTED,
    .status = RMI_ERROR_INPUT,
    .index = 0}
};
