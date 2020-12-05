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
#include "nnrf-handler.h"

#if 0
#include "npcf-handler.h"
#include "nudr-handler.h"
#endif

void pcf_ue_state_initial(ogs_fsm_t *s, pcf_event_t *e)
{
    ogs_assert(s);

    OGS_FSM_TRAN(s, &pcf_ue_state_operational);
}

void pcf_ue_state_final(ogs_fsm_t *s, pcf_event_t *e)
{
}

void pcf_ue_state_operational(ogs_fsm_t *s, pcf_event_t *e)
{
#if 0
    bool handled;
#endif
    pcf_ue_t *pcf_ue = NULL;

    ogs_sbi_stream_t *stream = NULL;
    ogs_sbi_message_t *message = NULL;

    ogs_assert(s);
    ogs_assert(e);

    pcf_sm_debug(e);

    pcf_ue = e->pcf_ue;
    ogs_assert(pcf_ue);

    switch (e->id) {
    case OGS_FSM_ENTRY_SIG:
        break;

    case OGS_FSM_EXIT_SIG:
        break;

    case PCF_EVT_SBI_SERVER:
        message = e->sbi.message;
        ogs_assert(message);
        stream = e->sbi.data;
        ogs_assert(stream);

#if 0
        SWITCH(message->h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NPCF_UEAU)
            SWITCH(message->h.method)
            CASE(OGS_SBI_HTTP_METHOD_POST)
                SWITCH(message->h.resource.component[1])
                CASE(OGS_SBI_RESOURCE_NAME_SECURITY_INFORMATION)
                    pcf_npcf_ueau_handle_get(pcf_ue, stream, message);
                    break;
                CASE(OGS_SBI_RESOURCE_NAME_AUTH_EVENTS)
                    pcf_npcf_ueau_handle_result_confirmation_inform(
                            pcf_ue, stream, message);
                    break;
                DEFAULT
                    ogs_error("[%s] Invalid resource name [%s]",
                            pcf_ue->suci, message->h.resource.component[1]);
                    ogs_sbi_server_send_error(stream,
                            OGS_SBI_HTTP_STATUS_BAD_REQUEST, message,
                            "Invalid resource name", message->h.method);
                END
                break;

            DEFAULT
                ogs_error("[%s] Invalid HTTP method [%s]",
                        pcf_ue->suci, message->h.method);
                ogs_sbi_server_send_error(stream,
                        OGS_SBI_HTTP_STATUS_FORBIDDEN, message,
                        "Invalid HTTP method", message->h.method);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NPCF_UECM)
            SWITCH(message->h.method)
            CASE(OGS_SBI_HTTP_METHOD_PUT)
                SWITCH(message->h.resource.component[1])
                CASE(OGS_SBI_RESOURCE_NAME_REGISTRATIONS)
                    pcf_npcf_uecm_handle_registration(pcf_ue, stream, message);
                    break;

                DEFAULT
                    ogs_error("[%s] Invalid resource name [%s]",
                            pcf_ue->suci, message->h.resource.component[1]);
                    ogs_sbi_server_send_error(stream,
                            OGS_SBI_HTTP_STATUS_BAD_REQUEST, message,
                            "Invalid HTTP method", message->h.method);
                END
                break;
            DEFAULT
                ogs_error("[%s] Invalid HTTP method [%s]",
                        pcf_ue->suci, message->h.method);
                ogs_sbi_server_send_error(stream,
                        OGS_SBI_HTTP_STATUS_FORBIDDEN, message,
                        "Invalid HTTP method", message->h.method);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NPCF_SDM)
            SWITCH(message->h.method)
            CASE(OGS_SBI_HTTP_METHOD_GET)
                SWITCH(message->h.resource.component[1])
                CASE(OGS_SBI_RESOURCE_NAME_AM_DATA)
                CASE(OGS_SBI_RESOURCE_NAME_SMF_SELECT_DATA)
                CASE(OGS_SBI_RESOURCE_NAME_SM_DATA)
                    pcf_sbi_discover_and_send(
                        OpenAPI_nf_type_UDR, pcf_ue, stream, message,
                        pcf_nudr_dr_build_query_subscription_provisioned);
                    break;

                CASE(OGS_SBI_RESOURCE_NAME_UE_CONTEXT_IN_SMF_DATA)
                    pcf_npcf_sdm_handle_subscription_provisioned(
                            pcf_ue, stream, message);
                    break;

                DEFAULT
                    ogs_error("[%s] Invalid resource name [%s]",
                            pcf_ue->suci, message->h.resource.component[1]);
                    ogs_sbi_server_send_error(stream,
                            OGS_SBI_HTTP_STATUS_BAD_REQUEST, message,
                            "Invalid resource name", message->h.method);
                END
                break;
            DEFAULT
                ogs_error("[%s] Invalid HTTP method [%s]",
                        pcf_ue->supi, message->h.method);
                ogs_sbi_server_send_error(stream,
                        OGS_SBI_HTTP_STATUS_NOT_FOUND, message,
                        "Invalid HTTP method", message->h.method);
            END
            break;

        DEFAULT
            ogs_error("Invalid API name [%s]", message->h.service.name);
            ogs_sbi_server_send_error(stream,
                    OGS_SBI_HTTP_STATUS_BAD_REQUEST, message,
                    "Invalid API name", message->h.service.name);
        END
#endif
        break;

    case PCF_EVT_SBI_CLIENT:
        message = e->sbi.message;
        ogs_assert(message);

        pcf_ue = e->pcf_ue;
        ogs_assert(pcf_ue);
        stream = e->sbi.data;
        ogs_assert(stream);

#if 0
        SWITCH(message->h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NUDR_DR)
            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_SUBSCRIPTION_DATA)
                SWITCH(message->h.resource.component[2])
                CASE(OGS_SBI_RESOURCE_NAME_AUTHENTICATION_DATA)
                    if (message->res_status != OGS_SBI_HTTP_STATUS_OK &&
                        message->res_status != OGS_SBI_HTTP_STATUS_NO_CONTENT) {
                        if (message->res_status ==
                                OGS_SBI_HTTP_STATUS_NOT_FOUND) {
                            ogs_warn("[%s] Cannot find SUPI [%d]",
                                pcf_ue->suci, message->res_status);
                        } else {
                            ogs_error("[%s] HTTP response error [%d]",
                                pcf_ue->suci, message->res_status);
                        }
                        ogs_sbi_server_send_error(
                            stream, message->res_status,
                            NULL, "HTTP response error", pcf_ue->suci);
                        break;
                    }

                    pcf_nudr_dr_handle_subscription_authentication(
                            pcf_ue, stream, message);
                    break;

                CASE(OGS_SBI_RESOURCE_NAME_CONTEXT_DATA)
                    if (message->res_status != OGS_SBI_HTTP_STATUS_NO_CONTENT) {
                        ogs_error("[%s] HTTP response error [%d]",
                            pcf_ue->suci, message->res_status);
                        ogs_sbi_server_send_error(
                            stream, message->res_status,
                            NULL, "HTTP response error", pcf_ue->suci);
                        break;
                    }

                    pcf_nudr_dr_handle_subscription_context(
                            pcf_ue, stream, message);
                    break;

                DEFAULT
                    SWITCH(message->h.resource.component[3])
                    CASE(OGS_SBI_RESOURCE_NAME_PROVISIONED_DATA)
                        handled = pcf_nudr_dr_handle_subscription_provisioned(
                                pcf_ue, stream, message);
                        if (!handled) {
                            ogs_sbi_server_send_error(
                                stream, message->res_status,
                                NULL, "HTTP response error", pcf_ue->suci);
                        }
                        break;

                    DEFAULT
                        ogs_error("Invalid resource name [%s]",
                                message->h.resource.component[2]);
                        ogs_assert_if_reached();
                    END
                END
                break;
            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        message->h.resource.component[0]);
                ogs_assert_if_reached();
            END
            break;

        DEFAULT
            ogs_error("Invalid API name [%s]", message->h.service.name);
            ogs_sbi_server_send_error(stream,
                    OGS_SBI_HTTP_STATUS_BAD_REQUEST, message,
                    "Invalid API name", message->h.resource.component[0]);
        END
#endif
        break;

    default:
        ogs_error("[%s] Unknown event %s", pcf_ue->suci, pcf_event_get_name(e));
        break;
    }
}

void pcf_ue_state_exception(ogs_fsm_t *s, pcf_event_t *e)
{
    pcf_ue_t *pcf_ue = NULL;
    ogs_assert(s);
    ogs_assert(e);

    pcf_sm_debug(e);

    pcf_ue = e->pcf_ue;
    ogs_assert(pcf_ue);

    switch (e->id) {
    case OGS_FSM_ENTRY_SIG:
        break;

    case OGS_FSM_EXIT_SIG:
        break;

    default:
        ogs_error("[%s] Unknown event %s", pcf_ue->suci, pcf_event_get_name(e));
        break;
    }
}
