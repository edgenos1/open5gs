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

    OpenAPI_policy_association_request_t PolicyAssociationRequest;

    ogs_assert(amf_ue);
    ogs_assert(amf_ue->supi);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_POST;
    message.h.service.name =
        (char *)OGS_SBI_SERVICE_NAME_NPCF_AM_POLICY_CONTROL;
    message.h.api.version = (char *)OGS_SBI_API_V1;
    message.h.resource.component[0] = (char *)OGS_SBI_RESOURCE_NAME_POLICIES;

    memset(&PolicyAssociationRequest, 0, sizeof(PolicyAssociationRequest));

    PolicyAssociationRequest.supi = amf_ue->supi;
    PolicyAssociationRequest.pei = amf_ue->pei;

    server = ogs_list_first(&ogs_sbi_self()->server_list);
    ogs_assert(server);

    memset(&header, 0, sizeof(header));
    header.service.name = (char *)OGS_SBI_SERVICE_NAME_NAMF_CALLBACK;
    header.api.version = (char *)OGS_SBI_API_V1;
    header.resource.component[0] = amf_ue->supi;
    header.resource.component[1] =
            (char *)OGS_SBI_RESOURCE_NAME_AM_POLICY_NOTIFY;
    PolicyAssociationRequest.notification_uri =
                        ogs_sbi_server_uri(server, &header);
    ogs_assert(PolicyAssociationRequest.notification_uri);

    PolicyAssociationRequest.supp_feat = (char *)"";

    message.PolicyAssociationRequest = &PolicyAssociationRequest;

    request = ogs_sbi_build_request(&message);
    ogs_assert(request);

    ogs_free(PolicyAssociationRequest.notification_uri);

    return request;
}
