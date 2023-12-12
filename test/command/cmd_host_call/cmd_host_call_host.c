/*
 * Copyright (c) 2023, Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "test_database.h"
#include "val_host_rmi.h"


void cmd_host_call_host(void)
{
    val_host_realm_ts realm;
    uint64_t ret;
    val_host_rec_enter_ts *rec_enter = NULL;

    val_memset(&realm, 0, sizeof(realm));

    val_host_realm_params(&realm);

    /* Populate realm with one REC*/
    if (val_host_realm_setup(&realm, 1))
    {
        LOG(ERROR, "\tRealm setup failed\n", 0, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(1)));
        goto destroy_realm;
    }

    /* Enter REC[0]  */
    ret = val_host_rmi_rec_enter(realm.rec[0], realm.run[0]);
    if (ret)
    {
        LOG(ERROR, "\tRec enter failed, ret=%x\n", ret, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(2)));
        goto destroy_realm;
    }
    else if (val_host_check_realm_exit_host_call((val_host_rec_run_ts *)realm.run[0]))
    {
        LOG(ERROR, "\tREC_EXIT:  Host call should failed.\n", 0, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(3)));
        goto destroy_realm;
    }

    rec_enter = &(((val_host_rec_run_ts *)realm.run[0])->enter);

    /* Set the value for gprs[1] to FF */
    rec_enter->gprs[1] = 0xFF;

    ret = val_host_rmi_rec_enter(realm.rec[0], realm.run[0]);
    if (ret)
    {
        LOG(ERROR, "\tRec enter failed, ret=%x\n", ret, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(4)));
        goto destroy_realm;
    }

    val_set_status(RESULT_PASS(VAL_SUCCESS));

    /* Free test resources */
destroy_realm:
    return;
}
