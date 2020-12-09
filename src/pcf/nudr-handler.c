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

#include "nudr-handler.h"

bool pcf_nudr_dr_handle_query_am_data(
    pcf_ue_t *pcf_ue, ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg)
{
    ogs_sbi_server_t *server = NULL;

    ogs_sbi_message_t sendmsg;
    ogs_sbi_header_t header;
    ogs_sbi_response_t *response = NULL;

    int status;

    ogs_assert(pcf_ue);
    ogs_assert(stream);
    server = ogs_sbi_server_from_stream(stream);
    ogs_assert(server);

    ogs_assert(recvmsg);

    SWITCH(recvmsg->h.resource.component[1])
    CASE(OGS_SBI_RESOURCE_NAME_UES)
        SWITCH(recvmsg->h.resource.component[3])
        CASE(OGS_SBI_RESOURCE_NAME_AM_DATA)
            OpenAPI_policy_association_t PolicyAssociation;

            if (!recvmsg->AmPolicyData) {
                ogs_error("[%s] No AmPolicyData", pcf_ue->supi);
                ogs_sbi_server_send_error(
                        stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                        recvmsg, "No AmPolicyData", pcf_ue->supi);
                return false;
            }

            if (!pcf_ue->policy_association_request) {
                ogs_error("[%s] No PolicyAssociationRequest", pcf_ue->supi);
                ogs_sbi_server_send_error(
                        stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                        recvmsg, "No PolicyAssociationRequest", pcf_ue->supi);
                return false;
            }

            memset(&PolicyAssociation, 0, sizeof(PolicyAssociation));
            PolicyAssociation.request = pcf_ue->policy_association_request;
            PolicyAssociation.supp_feat = (char *)"";

            memset(&sendmsg, 0, sizeof(sendmsg));
            sendmsg.PolicyAssociation = &PolicyAssociation;

            status = OGS_SBI_HTTP_STATUS_CREATED;

            response = ogs_sbi_build_response(&sendmsg, status);
            ogs_assert(response);
            ogs_sbi_server_send_response(stream, response);

            return true;

        DEFAULT
            ogs_error("Invalid resource name [%s]",
                    recvmsg->h.resource.component[3]);
        END

    DEFAULT
        ogs_error("Invalid resource name [%s]",
                recvmsg->h.resource.component[1]);
    END

#if 0
        Amf3GppAccessRegistration = pcf_ue->amf_3gpp_access_registration;
        if (!Amf3GppAccessRegistration) {
            ogs_error("[%s] No Amf3GppAccessRegistration", pcf_ue->supi);
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                    recvmsg, "No Amf3GppAccessRegistration", pcf_ue->supi);
            return false;
        }

        if (!Amf3GppAccessRegistration->amf_instance_id) {
            ogs_error("[%s] No amfInstanceId", pcf_ue->supi);
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                    recvmsg, "No amfInstanceId", pcf_ue->supi);
            return false;
        }

        memset(&sendmsg, 0, sizeof(sendmsg));

        memset(&header, 0, sizeof(header));
        header.service.name = (char *)OGS_SBI_SERVICE_NAME_NPCF_UECM;
        header.api.version = (char *)OGS_SBI_API_V1;
        header.resource.component[0] = pcf_ue->supi;
        header.resource.component[1] =
            (char *)OGS_SBI_RESOURCE_NAME_REGISTRATIONS;
        header.resource.component[2] =
            (char *)OGS_SBI_RESOURCE_NAME_AMF_3GPP_ACCESS;

        if (pcf_ue->amf_instance_id &&
            strcmp(pcf_ue->amf_instance_id,
                Amf3GppAccessRegistration->amf_instance_id) == 0)
            status = OGS_SBI_HTTP_STATUS_OK;
        else
            status = OGS_SBI_HTTP_STATUS_CREATED;

        if (status == OGS_SBI_HTTP_STATUS_CREATED)
            sendmsg.http.location = ogs_sbi_server_uri(server, &header);

        sendmsg.Amf3GppAccessRegistration =
            OpenAPI_amf3_gpp_access_registration_copy(
                sendmsg.Amf3GppAccessRegistration,
                    pcf_ue->amf_3gpp_access_registration);

        response = ogs_sbi_build_response(&sendmsg, status);
        ogs_assert(response);
        ogs_sbi_server_send_response(stream, response);

        ogs_free(sendmsg.http.location);
        OpenAPI_amf3_gpp_access_registration_free(
                sendmsg.Amf3GppAccessRegistration);
        return true;
#endif

    return false;
}
