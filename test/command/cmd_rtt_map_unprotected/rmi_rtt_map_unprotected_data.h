/*
 * Copyright (c) 2023, Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "val_rmm.h"

enum test_intent {
    MEM_ATTR_INVALID = 0X0,
    RD_UNALIGNED = 0X1,
    RD_DEV_MEM = 0X2,
    RD_OUTSIDE_OF_PERMITTED_PA = 0X3,
    RD_STATE_UNDELEGATED = 0X4,
    RD_STATE_DELEGATED = 0X5,
    RD_STATE_REC = 0X6,
    RD_STATE_RTT = 0X7,
    RD_STATE_DATA = 0X8,
    LEVEL_STARTING = 0X9,
    LEVEL_L1 = 0XA,
    LEVEL_OOB = 0XB,
    ADDR_UNALIGNED = 0XC,
    IPA_UNALIGNED = 0XD,
    IPA_PROTECTED = 0XE,
    IPA_OOB = 0XF,
    IPA_NOT_MAPPED = 0X10,
    RTTE_STATE_VALID_NS = 0X11
};

struct stimulus {
    char msg[100];
    uint64_t abi;
    enum test_intent label;
    uint64_t status;
    uint64_t index;
};

static struct stimulus test_data[] = {
    {.msg = "attr_valid",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = MEM_ATTR_INVALID,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_align",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = RD_UNALIGNED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_bound",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = RD_DEV_MEM,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_bound",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = RD_OUTSIDE_OF_PERMITTED_PA,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_state",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = RD_STATE_UNDELEGATED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_state",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = RD_STATE_DELEGATED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_state",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = RD_STATE_REC,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_state",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = RD_STATE_RTT,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rd_state",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = RD_STATE_DATA,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "level_bound",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = LEVEL_STARTING,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "level_bound",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = LEVEL_L1,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "level_bound",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = LEVEL_OOB,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "addr_align",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = ADDR_UNALIGNED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "ipa_align",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = IPA_UNALIGNED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "ipa_bound",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = IPA_PROTECTED,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "ipa_bound",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = IPA_OOB,
    .status = RMI_ERROR_INPUT,
    .index = 0},
    {.msg = "rtt_walk",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = IPA_NOT_MAPPED,
    .status = RMI_ERROR_RTT,
    .index = 2},
    {.msg = "rtte_state",
    .abi = RMI_RTT_MAP_UNPROTECTED,
    .label = RTTE_STATE_VALID_NS,
    .status = RMI_ERROR_RTT,
    .index = 3}
};