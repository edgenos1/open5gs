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

#include "npcf-build.h"

ogs_sbi_request_t *amf_npcf_am_policy_control_build_create(
        amf_ue_t *amf_ue, void *data)
{
    ogs_sbi_message_t message;
    ogs_sbi_header_t header;
    ogs_sbi_request_t *request = NULL;
    ogs_sbi_server_t *server = NULL;

    OpenAPI_amf3_gpp_access_registration_t Amf3GppAccessRegistration;

    ogs_assert(amf_ue);
    ogs_assert(amf_ue->supi);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_POST;
    message.h.service.name =
        (char *)OGS_SBI_SERVICE_NAME_NPCF_AM_POLICY_CONTROL;
    message.h.api.version = (char *)OGS_SBI_API_V1;
    message.h.resource.component[0] = (char *)OGS_SBI_RESOURCE_NAME_POLICIES;

#if 0
    memset(&Amf3GppAccessRegistration, 0, sizeof(Amf3GppAccessRegistration));

    Amf3GppAccessRegistration.amf_instance_id = ogs_sbi_self()->nf_instance_id;

    server = ogs_list_first(&ogs_sbi_self()->server_list);
    ogs_assert(server);

    memset(&header, 0, sizeof(header));
    header.service.name = (char *)OGS_SBI_SERVICE_NAME_NPCF_UECM;
    header.api.version = (char *)OGS_SBI_API_V1;
    header.resource.component[0] = amf_ue->supi;
    header.resource.component[1] =
            (char *)OGS_SBI_RESOURCE_NAME_DEREG_NOTIFY;
    Amf3GppAccessRegistration.dereg_callback_uri =
                        ogs_sbi_server_uri(server, &header);
    ogs_assert(Amf3GppAccessRegistration.dereg_callback_uri);

    Amf3GppAccessRegistration.guami = ogs_sbi_build_guami(amf_ue->guami);
    Amf3GppAccessRegistration.rat_type = OpenAPI_rat_type_NR;

    message.Amf3GppAccessRegistration = &Amf3GppAccessRegistration;
#endif

    request = ogs_sbi_build_request(&message);
    ogs_assert(request);

#if 0
    if (Amf3GppAccessRegistration.guami)
        ogs_sbi_free_guami(Amf3GppAccessRegistration.guami);
    ogs_free(Amf3GppAccessRegistration.dereg_callback_uri);
#endif

    return request;
}
