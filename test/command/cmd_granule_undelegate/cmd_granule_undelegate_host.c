/*
 * Copyright (c) 2023, Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test_database.h"
#include "val_host_rmi.h"
#include "rmi_granule_undelegate_data.h"
#include "command_common_host.h"

#define IPA_ADDR_DATA  (4 * PAGE_SIZE)
#define IPA_ADDR_DATA1  (6 * PAGE_SIZE)

#define VALID_REALM 0
#define NUM_REALMS 3

static val_host_realm_ts realm_test[NUM_REALMS];

static struct argument_store {
    uint64_t addr_valid;
} c_args;

static struct invalid_argument_store {
    uint64_t rd_gran;
    uint64_t undelegated_gran;
} c_args_invalid;

struct arguments {
    uint64_t addr;
};

static uint64_t addr_valid_prep_sequence(void)
{
    return g_delegated_prep_sequence();
}

static uint64_t g_rd_new_prep_sequence(uint16_t vmid)
{
    val_host_realm_ts realm_init;
    val_host_rmifeatureregister0_ts features_0;

    val_memset(&realm_init, 0, sizeof(realm_init));
    features_0.s2sz = 40;
    val_memcpy(&realm_init.realm_feat_0, &features_0, sizeof(features_0));

    realm_init.hash_algo = RMI_HASH_SHA_256;
    realm_init.s2_starting_level = 0;
    realm_init.num_s2_sl_rtts = 1;
    realm_init.vmid = vmid;

    if (val_host_realm_create_common(&realm_init))
    {
        LOG(ERROR, "\tRealm create failed\n", 0, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(1)));
    }
    realm_test[vmid].rd = realm_init.rd;
    realm_test[vmid].rtt_l0_addr = realm_init.rtt_l0_addr;
    return realm_init.rd;
}

static uint64_t g_rec_ready_prep_sequence(uint64_t rd)
{
    val_host_realm_ts realm;
    val_host_rec_params_ts rec_params;

    realm.rec_count = 1;
    realm.rd = rd;
    rec_params.pc = 0;
    rec_params.flags = RMI_RUNNABLE;
    rec_params.mpidr = 0;

    if (val_host_rec_create_common(&realm, &rec_params))
    {
        LOG(ERROR, "\tCouldn't destroy the Realm\n", 0, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(2)));
        return VAL_TEST_PREP_SEQ_FAILED;
    }
    return realm.rec[0];
}

static uint64_t valid_input_args_prep_sequence(void)
{
    c_args.addr_valid = addr_valid_prep_sequence();
    if (c_args.addr_valid == VAL_TEST_PREP_SEQ_FAILED)
        return VAL_TEST_PREP_SEQ_FAILED;

    return VAL_SUCCESS;
}

static uint64_t invalid_input_args_prep_sequence(void)
{
    c_args_invalid.rd_gran = g_rd_new_prep_sequence(VALID_REALM);
    if (c_args_invalid.rd_gran == VAL_TEST_PREP_SEQ_FAILED)
        return VAL_TEST_PREP_SEQ_FAILED;

    return VAL_SUCCESS;
}

static uint64_t intent_to_seq(struct stimulus *test_data, struct arguments *args)
{
    enum test_intent label = test_data->label;

    switch (label)
    {
        case ADDR_UNALIGNED:
            args->addr = g_unaligned_prep_sequence(c_args.addr_valid);
            break;

        case ADDR_DEV_MEM_MMIO:
            args->addr = g_dev_mem_prep_sequence();;
            break;

        case ADDR_OUTSIDE_OF_PERMITTED_PA:
            args->addr = g_outside_of_permitted_pa_prep_sequence();
            break;

        case ADDR_UNDELEGATED:
            args->addr = g_undelegated_prep_sequence();
            c_args_invalid.undelegated_gran = args->addr;
            if (args->addr == VAL_TEST_PREP_SEQ_FAILED)
                return VAL_ERROR;
            break;

        case ADDR_REC:
            args->addr = g_rec_ready_prep_sequence(c_args_invalid.rd_gran);
            if (args->addr == VAL_TEST_PREP_SEQ_FAILED)
                return VAL_ERROR;
            break;

        case ADDR_DATA:
            args->addr = g_data_prep_sequence(c_args_invalid.rd_gran, IPA_ADDR_DATA);
            if (args->addr == VAL_TEST_PREP_SEQ_FAILED)
                return VAL_ERROR;
            break;

        case ADDR_RTT:
            args->addr = realm_test[VALID_REALM].rtt_l0_addr;
            break;

        case ADDR_RD:
            args->addr = c_args_invalid.rd_gran;
            break;

        default:
            val_set_status(RESULT_FAIL(VAL_ERROR_POINT(3)));
            LOG(ERROR, "\n\tUnknown intent label encountered\n", 0, 0);
            return VAL_ERROR;
    }

    return VAL_SUCCESS;
}


void cmd_granule_undelegate_host(void)
{
    uint64_t ret, i;
    struct arguments args;

    if (valid_input_args_prep_sequence() == VAL_TEST_PREP_SEQ_FAILED) {
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(4)));
        goto fail;
    }

    if (invalid_input_args_prep_sequence() == VAL_TEST_PREP_SEQ_FAILED) {
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(5)));
        goto fail;
    }

    for (i = 0; i < (sizeof(test_data) / sizeof(struct stimulus)); i++)
    {
        LOG(TEST, "\n\tCheck %d : ", i + 1, 0);
        LOG(TEST, test_data[i].msg, 0, 0);
        LOG(TEST, "; intent id : 0x%x \n", test_data[i].label, 0);

        if (intent_to_seq(&test_data[i], &args)) {
            val_set_status(RESULT_FAIL(VAL_ERROR_POINT(6)));
            goto fail;
        }

        ret = val_host_rmi_granule_undelegate(args.addr);

        if (ret != PACK_CODE(test_data[i].status, test_data[i].index))
        {
            LOG(ERROR, "\tERROR status code : %d index %d\n", test_data[i].status,
                                                              test_data[i].index);
            val_set_status(RESULT_FAIL(VAL_ERROR_POINT(7)));
            goto fail;
        }
    }

    /* granule(addr).PAS=Non-secure, an access to addr from the NS world
     * should be successful.
     */
    uint64_t *addr = (uint64_t *)c_args_invalid.undelegated_gran;
    *addr = 0x01;       // writing to non-secure addr

    LOG(TEST, "\n\tPositive Observability Check\n", 0, 0);
    ret = val_host_rmi_granule_undelegate(c_args.addr_valid);
    if (ret != 0)
    {
        LOG(ERROR, "\n\tGranule undelegate command failed, ret value is: %x\n", ret, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(8)));
        goto fail;
    }

    LOG(TEST, "\n\tNegative Observability Check\n", 0, 0);
    ret = val_host_rmi_granule_undelegate(c_args.addr_valid);
    if (ret == 0)
    {
        LOG(ERROR, "\n\tGranule undelegate command should failed, ret value is: %x\n", ret, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(9)));
        goto fail;
    }

    /* Check granule(addr).PAS,granule(addr).content by data_create, data_destroy
     * and undelegate command and see contents are wiped
     */
    if (create_mapping(IPA_ADDR_DATA1, true, c_args_invalid.rd_gran))
    {
        LOG(ERROR, "\tCouldn't create the assigned protected mapping\n", 0, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(10)));
        goto fail;
    }

    uint64_t data = g_delegated_prep_sequence();
    uint64_t src = g_undelegated_prep_sequence();
    uint64_t flags = RMI_NO_MEASURE_CONTENT;
    val_memset((void *)src, 0xa, PAGE_SIZE);

    if (val_host_rmi_data_create(data, c_args_invalid.rd_gran, IPA_ADDR_DATA1, src, flags))
    {
        LOG(ERROR, "\tCouldn't complete the assigned protected mapping\n", 0, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(11)));
        goto fail;
    }

    if (val_host_rmi_data_destroy(c_args_invalid.rd_gran, IPA_ADDR_DATA1))
    {
        LOG(ERROR, "\tdata destroy failed\n", 0, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(12)));
        goto fail;
    }

    if (val_host_rmi_granule_undelegate(data))
    {
        LOG(ERROR, "\tundelegated failed\n", 0, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(13)));
        goto fail;
    }

    if (!val_memcmp((void *)src, (void *)data, PAGE_SIZE))
    {
        LOG(ERROR, "\t src and data contents are matched, i.e. contents are not wiped\n", 0, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(14)));
        goto fail;
    }

    /* For granule(addr).PAS = Realm && granule(addr).state = REC/DATA/RD/RTT execute the
     * respective destroy command and verify that it is successful
     */
    if (val_host_realm_destroy(c_args_invalid.rd_gran))
    {
        LOG(ERROR, "\t realm destroy failedddd", 0, 0);
        val_set_status(RESULT_FAIL(VAL_ERROR_POINT(15)));
        goto fail;
    }

    val_set_status(RESULT_PASS(VAL_SUCCESS));
    return;

fail:
    val_set_status(RESULT_FAIL(VAL_ERROR_POINT(16)));
    return;
}