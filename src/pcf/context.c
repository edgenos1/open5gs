/*
 * Copyright (C) 2019,2020 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "sbi-path.h"

static pcf_context_t self;

int __pcf_log_domain;

static OGS_POOL(pcf_ue_pool, pcf_ue_t);

static int context_initialized = 0;

void pcf_context_init(void)
{
    ogs_assert(context_initialized == 0);

    /* Initialize PCF context */
    memset(&self, 0, sizeof(pcf_context_t));

    ogs_log_install_domain(&__pcf_log_domain, "pcf", ogs_core()->log.level);

    ogs_pool_init(&pcf_ue_pool, ogs_app()->max.ue);

    ogs_list_init(&self.pcf_ue_list);
    self.suci_hash = ogs_hash_make();
    self.supi_hash = ogs_hash_make();

    context_initialized = 1;
}

void pcf_context_final(void)
{
    ogs_assert(context_initialized == 1);

    pcf_ue_remove_all();

    ogs_assert(self.suci_hash);
    ogs_hash_destroy(self.suci_hash);
    ogs_assert(self.supi_hash);
    ogs_hash_destroy(self.supi_hash);

    ogs_pool_final(&pcf_ue_pool);

    context_initialized = 0;
}

pcf_context_t *pcf_self(void)
{
    return &self;
}

static int pcf_context_prepare(void)
{
    self.nf_type = OpenAPI_nf_type_PCF;

    return OGS_OK;
}

static int pcf_context_validation(void)
{
    return OGS_OK;
}

int pcf_context_parse_config(void)
{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;

    document = ogs_app()->document;
    ogs_assert(document);

    rv = pcf_context_prepare();
    if (rv != OGS_OK) return rv;

    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        ogs_assert(root_key);
        if (!strcmp(root_key, "pcf")) {
            ogs_yaml_iter_t pcf_iter;
            ogs_yaml_iter_recurse(&root_iter, &pcf_iter);
            while (ogs_yaml_iter_next(&pcf_iter)) {
                const char *pcf_key = ogs_yaml_iter_key(&pcf_iter);
                ogs_assert(pcf_key);
                if (!strcmp(pcf_key, "sbi")) {
                    /* handle config in sbi library */
                } else
                    ogs_warn("unknown key `%s`", pcf_key);
            }
        }
    }

    rv = pcf_context_validation();
    if (rv != OGS_OK) return rv;

    return OGS_OK;
}

pcf_ue_t *pcf_ue_add(char *suci)
{
    pcf_event_t e;
    pcf_ue_t *pcf_ue = NULL;

    ogs_assert(suci);

    ogs_pool_alloc(&pcf_ue_pool, &pcf_ue);
    ogs_assert(pcf_ue);
    memset(pcf_ue, 0, sizeof *pcf_ue);

    pcf_ue->ctx_id = ogs_msprintf("%d",
            (int)ogs_pool_index(&pcf_ue_pool, pcf_ue));
    ogs_assert(pcf_ue->ctx_id);

    pcf_ue->suci = ogs_strdup(suci);
    ogs_assert(pcf_ue->suci);
    ogs_hash_set(self.suci_hash, pcf_ue->suci, strlen(pcf_ue->suci), pcf_ue);

    pcf_ue->supi = ogs_supi_from_suci(pcf_ue->suci);
    ogs_assert(pcf_ue->supi);
    ogs_hash_set(self.supi_hash, pcf_ue->supi, strlen(pcf_ue->supi), pcf_ue);

    memset(&e, 0, sizeof(e));
    e.pcf_ue = pcf_ue;
    ogs_fsm_create(&pcf_ue->sm, pcf_ue_state_initial, pcf_ue_state_final);
    ogs_fsm_init(&pcf_ue->sm, &e);

    ogs_list_add(&self.pcf_ue_list, pcf_ue);

    return pcf_ue;
}

void pcf_ue_remove(pcf_ue_t *pcf_ue)
{
    pcf_event_t e;

    ogs_assert(pcf_ue);

    ogs_list_remove(&self.pcf_ue_list, pcf_ue);

    memset(&e, 0, sizeof(e));
    e.pcf_ue = pcf_ue;
    ogs_fsm_fini(&pcf_ue->sm, &e);
    ogs_fsm_delete(&pcf_ue->sm);

    /* Free SBI object memory */
    ogs_sbi_object_free(&pcf_ue->sbi);

    OpenAPI_auth_event_free(pcf_ue->auth_event);
    OpenAPI_amf3_gpp_access_registration_free(
            pcf_ue->amf_3gpp_access_registration);

    ogs_assert(pcf_ue->ctx_id);
    ogs_free(pcf_ue->ctx_id);

    ogs_assert(pcf_ue->suci);
    ogs_hash_set(self.suci_hash, pcf_ue->suci, strlen(pcf_ue->suci), NULL);
    ogs_free(pcf_ue->suci);

    ogs_assert(pcf_ue->supi);
    ogs_hash_set(self.supi_hash, pcf_ue->supi, strlen(pcf_ue->supi), NULL);
    ogs_free(pcf_ue->supi);

    if (pcf_ue->serving_network_name)
        ogs_free(pcf_ue->serving_network_name);
    if (pcf_ue->ausf_instance_id)
        ogs_free(pcf_ue->ausf_instance_id);
    if (pcf_ue->amf_instance_id)
        ogs_free(pcf_ue->amf_instance_id);
    if (pcf_ue->dereg_callback_uri)
        ogs_free(pcf_ue->dereg_callback_uri);

    ogs_pool_free(&pcf_ue_pool, pcf_ue);
}

void pcf_ue_remove_all()
{
    pcf_ue_t *pcf_ue = NULL, *next = NULL;;

    ogs_list_for_each_safe(&self.pcf_ue_list, next, pcf_ue)
        pcf_ue_remove(pcf_ue);
}

pcf_ue_t *pcf_ue_find_by_suci(char *suci)
{
    ogs_assert(suci);
    return (pcf_ue_t *)ogs_hash_get(self.suci_hash, suci, strlen(suci));
}

pcf_ue_t *pcf_ue_find_by_supi(char *supi)
{
    ogs_assert(supi);
    return (pcf_ue_t *)ogs_hash_get(self.supi_hash, supi, strlen(supi));
}

pcf_ue_t *pcf_ue_find_by_suci_or_supi(char *suci_or_supi)
{
    ogs_assert(suci_or_supi);
    if (strncmp(suci_or_supi, "suci-", strlen("suci-")) == 0)
        return pcf_ue_find_by_suci(suci_or_supi);
    else
        return pcf_ue_find_by_supi(suci_or_supi);
}

pcf_ue_t *pcf_ue_find_by_ctx_id(char *ctx_id)
{
    ogs_assert(ctx_id);
    return ogs_pool_find(&pcf_ue_pool, atoll(ctx_id));
}

pcf_ue_t *pcf_ue_cycle(pcf_ue_t *pcf_ue)
{
    return ogs_pool_cycle(&pcf_ue_pool, pcf_ue);
}
