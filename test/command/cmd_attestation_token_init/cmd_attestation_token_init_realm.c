/*
 * Copyright (c) 2023, Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test_database.h"
#include "val_realm_rsi.h"
#include "val_realm_framework.h"

void cmd_attestation_token_init_realm(void)
{
    val_smc_param_ts args = {0,};
    uint64_t token_size = 0, size, max_size, len;
    __attribute__((aligned (PAGE_SIZE))) uint64_t token[MAX_REALM_CCA_TOKEN_SIZE/8] = {0,};
    uint64_t *granule = token;
    uint64_t challenge[8] = {0xb4ea40d262abaf22,
                             0xe8d966127b6d78e2,
                             0x7ce913f20b954277,
                             0x3155ff12580f9e60,
                             0x8a3843cb95120bf6,
                             0xd52c4fca64420f43,
                             0xb75961661d52e8ce,
                             0xc7f17650fe9fca60};

    args = val_realm_rsi_attestation_token_init(challenge[0], challenge[1],
                                                  challenge[2], challenge[3], challenge[4],
                                                 challenge[5], challenge[6], challenge[7]);
    if (args.x0)
    {
        LOG(ERROR, "\tToken init command failed, ret=%x\n", args.x0, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(1)));
        goto exit;
    }

    max_size = args.x1;

    /* Check the attest state is in ATTEST_IN_PROGRESS */
    do {
        uint64_t offset = 0;
        do {
            size = PAGE_SIZE - offset;
            args = val_realm_rsi_attestation_token_continue((uint64_t)granule, offset, size, &len);
            offset += len;
            token_size = token_size + len;
        } while (args.x0 == RSI_ERROR_INCOMPLETE && offset < PAGE_SIZE);

        if (args.x0 == RSI_ERROR_INCOMPLETE)
        {
            granule += PAGE_SIZE;
        }

    } while ((args.x0 == RSI_ERROR_INCOMPLETE) && (granule < token + max_size));

    if (args.x0)
    {
        LOG(ERROR, "\tToken continue should passed, ret=%x\n", args.x0, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(2)));
        goto exit;
    }

exit:
    val_realm_return_to_host();
}


