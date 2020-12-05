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

void pcf_state_initial(ogs_fsm_t *s, pcf_event_t *e)
{
    pcf_sm_debug(e);

    ogs_assert(s);

    OGS_FSM_TRAN(s, &pcf_state_operational);
}

void pcf_state_final(ogs_fsm_t *s, pcf_event_t *e)
{
    pcf_sm_debug(e);
}

void pcf_state_operational(ogs_fsm_t *s, pcf_event_t *e)
{
    int rv;

    ogs_sbi_stream_t *stream = NULL;
    ogs_sbi_request_t *request = NULL;

    ogs_sbi_nf_instance_t *nf_instance = NULL;
    ogs_sbi_subscription_t *subscription = NULL;
    ogs_sbi_response_t *response = NULL;
    ogs_sbi_message_t message;
    ogs_sbi_xact_t *sbi_xact = NULL;

    pcf_ue_t *pcf_ue = NULL;

    pcf_sm_debug(e);

    ogs_assert(s);

    switch (e->id) {
    case OGS_FSM_ENTRY_SIG:
        rv = pcf_sbi_open();
        if (rv != OGS_OK) {
            ogs_fatal("Can't establish SBI path");
        }

        break;

    case OGS_FSM_EXIT_SIG:
        pcf_sbi_close();
        break;

    case PCF_EVT_SBI_SERVER:
        request = e->sbi.request;
        ogs_assert(request);
        stream = e->sbi.data;
        ogs_assert(stream);

        rv = ogs_sbi_parse_request(&message, request);
        if (rv != OGS_OK) {
            /* 'message' buffer is released in ogs_sbi_parse_request() */
            ogs_error("cannot parse HTTP message");
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                    NULL, "cannot parse HTTP message", NULL);
            break;
        }

        if (strcmp(message.h.api.version, OGS_SBI_API_V1) != 0) {
            ogs_error("Not supported version [%s]", message.h.api.version);
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                    &message, "Not supported version", NULL);
            ogs_sbi_message_free(&message);
            break;
        }

        SWITCH(message.h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NNRF_NFM)

            SWITCH(message.h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_NF_STATUS_NOTIFY)
                SWITCH(message.h.method)
                CASE(OGS_SBI_HTTP_METHOD_POST)
                    pcf_nnrf_handle_nf_status_notify(stream, &message);
                    break;

                DEFAULT
                    ogs_error("Invalid HTTP method [%s]",
                            message.h.method);
                    ogs_sbi_server_send_error(stream,
                            OGS_SBI_HTTP_STATUS_FORBIDDEN, &message,
                            "Invalid HTTP method", message.h.method);
                END
                break;

            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        message.h.resource.component[0]);
                ogs_sbi_server_send_error(stream,
                        OGS_SBI_HTTP_STATUS_BAD_REQUEST, &message,
                        "Unknown resource name",
                        message.h.resource.component[0]);
            END
            break;

#if 0
        CASE(OGS_SBI_SERVICE_NAME_NPCF_UEAU)
        CASE(OGS_SBI_SERVICE_NAME_NPCF_UECM)
        CASE(OGS_SBI_SERVICE_NAME_NPCF_SDM)
            if (!message.h.resource.component[0]) {
                ogs_error("Not found [%s]", message.h.method);
                ogs_sbi_server_send_error(stream,
                    OGS_SBI_HTTP_STATUS_NOT_FOUND,
                    &message, "Not found", message.h.method);
                break;
            }

            if (!message.h.resource.component[1]) {
                ogs_error("Invalid resource name [%s]", message.h.method);
                ogs_sbi_server_send_error(stream,
                    OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                    &message, "Invalid resource name", message.h.method);
                break;
            }

            SWITCH(message.h.resource.component[2])
            CASE(OGS_SBI_RESOURCE_NAME_AUTH_EVENTS)
                pcf_ue = pcf_ue_find_by_ctx_id(
                        message.h.resource.component[2]);
                break;

            DEFAULT
                pcf_ue = pcf_ue_find_by_suci_or_supi(
                        message.h.resource.component[0]);
                if (!pcf_ue) {
                    pcf_ue = pcf_ue_add(message.h.resource.component[0]);
                    ogs_assert(pcf_ue);
                }
            END

            if (!pcf_ue) {
                ogs_error("Not found [%s]", message.h.method);
                ogs_sbi_server_send_error(stream,
                    OGS_SBI_HTTP_STATUS_NOT_FOUND,
                    &message, "Not found", message.h.method);
                break;
            }

            ogs_assert(OGS_FSM_STATE(&pcf_ue->sm));

            e->pcf_ue = pcf_ue;
            e->sbi.message = &message;
            ogs_fsm_dispatch(&pcf_ue->sm, e);
            if (OGS_FSM_CHECK(&pcf_ue->sm, pcf_ue_state_exception)) {
                ogs_error("[%s] State machine exception", pcf_ue->suci);
                pcf_ue_remove(pcf_ue);
            }
            break;
#endif

        DEFAULT
            ogs_error("Invalid API name [%s]", message.h.service.name);
            ogs_sbi_server_send_error(stream,
                    OGS_SBI_HTTP_STATUS_BAD_REQUEST, &message,
                    "Invalid API name", message.h.service.name);
        END

        /* In lib/sbi/server.c, notify_completed() releases 'request' buffer. */
        ogs_sbi_message_free(&message);
        break;

    case PCF_EVT_SBI_CLIENT:
        ogs_assert(e);

        response = e->sbi.response;
        ogs_assert(response);
        rv = ogs_sbi_parse_response(&message, response);
        if (rv != OGS_OK) {
            ogs_error("cannot parse HTTP response");
            ogs_sbi_message_free(&message);
            ogs_sbi_response_free(response);
            break;
        }

        if (strcmp(message.h.api.version, OGS_SBI_API_V1) != 0) {
            ogs_error("Not supported version [%s]", message.h.api.version);
            ogs_sbi_message_free(&message);
            ogs_sbi_response_free(response);
            break;
        }

        SWITCH(message.h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NNRF_NFM)

            SWITCH(message.h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_NF_INSTANCES)
                nf_instance = e->sbi.data;
                ogs_assert(nf_instance);
                ogs_assert(OGS_FSM_STATE(&nf_instance->sm));

                e->sbi.message = &message;
                ogs_fsm_dispatch(&nf_instance->sm, e);
                break;

            CASE(OGS_SBI_RESOURCE_NAME_SUBSCRIPTIONS)
                subscription = e->sbi.data;
                ogs_assert(subscription);

                SWITCH(message.h.method)
                CASE(OGS_SBI_HTTP_METHOD_POST)
                    if (message.res_status == OGS_SBI_HTTP_STATUS_CREATED ||
                        message.res_status == OGS_SBI_HTTP_STATUS_OK) {
                        pcf_nnrf_handle_nf_status_subscribe(
                                subscription, &message);
                    } else {
                        ogs_error("[%s] HTTP response error [%d]",
                                subscription->id, message.res_status);
                    }
                    break;

                CASE(OGS_SBI_HTTP_METHOD_DELETE)
                    if (message.res_status ==
                            OGS_SBI_HTTP_STATUS_NO_CONTENT) {
                        ogs_sbi_subscription_remove(subscription);
                    } else {
                        ogs_error("[%s] HTTP response error [%d]",
                                subscription->id, message.res_status);
                    }
                    break;

                DEFAULT
                    ogs_error("[%s] Invalid HTTP method [%s]",
                            subscription->id, message.h.method);
                    ogs_assert_if_reached();
                END
                break;
            
            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        message.h.resource.component[0]);
                ogs_assert_if_reached();
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NNRF_DISC)
            SWITCH(message.h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_NF_INSTANCES)
                sbi_xact = e->sbi.data;
                ogs_assert(sbi_xact);

                SWITCH(message.h.method)
                CASE(OGS_SBI_HTTP_METHOD_GET)
                    if (message.res_status == OGS_SBI_HTTP_STATUS_OK)
                        pcf_nnrf_handle_nf_discover(sbi_xact, &message);
                    else
                        ogs_error("HTTP response error [%d]",
                                message.res_status);
                    break;

                DEFAULT
                    ogs_error("Invalid HTTP method [%s]", message.h.method);
                    ogs_assert_if_reached();
                END
                break;

            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        message.h.resource.component[0]);
                ogs_assert_if_reached();
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NUDR_DR)
            SWITCH(message.h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_SUBSCRIPTION_DATA)
                sbi_xact = e->sbi.data;
                ogs_assert(sbi_xact);

                pcf_ue = (pcf_ue_t *)sbi_xact->sbi_object;
                ogs_assert(pcf_ue);
                pcf_ue = pcf_ue_cycle(pcf_ue);
                ogs_assert(pcf_ue);

                e->pcf_ue = pcf_ue;
                e->sbi.message = &message;
                e->sbi.data = sbi_xact->assoc_stream;

                ogs_sbi_xact_remove(sbi_xact);

                ogs_fsm_dispatch(&pcf_ue->sm, e);
                if (OGS_FSM_CHECK(&pcf_ue->sm, pcf_ue_state_exception)) {
                    ogs_error("[%s] State machine exception", pcf_ue->suci);
                    pcf_ue_remove(pcf_ue);
                }

                break;

            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        message.h.resource.component[0]);
                ogs_assert_if_reached();
            END
            break;

        DEFAULT
            ogs_error("Invalid API name [%s]", message.h.service.name);
            ogs_assert_if_reached();
        END

        ogs_sbi_message_free(&message);
        ogs_sbi_response_free(response);
        break;

    case PCF_EVT_SBI_TIMER:
        ogs_assert(e);

        switch(e->timer_id) {
        case PCF_TIMER_NF_INSTANCE_REGISTRATION_INTERVAL:
        case PCF_TIMER_NF_INSTANCE_HEARTBEAT_INTERVAL:
        case PCF_TIMER_NF_INSTANCE_NO_HEARTBEAT:
        case PCF_TIMER_NF_INSTANCE_VALIDITY:
            nf_instance = e->sbi.data;
            ogs_assert(nf_instance);
            ogs_assert(OGS_FSM_STATE(&nf_instance->sm));

            ogs_fsm_dispatch(&nf_instance->sm, e);
            if (OGS_FSM_CHECK(&nf_instance->sm, pcf_nf_state_exception))
                ogs_error("[%s] State machine exception [%d]",
                        nf_instance->id, e->timer_id);
            break;

        case PCF_TIMER_SUBSCRIPTION_VALIDITY:
            subscription = e->sbi.data;
            ogs_assert(subscription);

            ogs_info("[%s] Subscription validity expired", subscription->id);
            ogs_sbi_subscription_remove(subscription);

            ogs_nnrf_nfm_send_nf_status_subscribe(subscription->client,
                    pcf_self()->nf_type, subscription->nf_instance_id);
            break;

        case PCF_TIMER_SBI_CLIENT_WAIT:
            sbi_xact = e->sbi.data;
            ogs_assert(sbi_xact);

            stream = sbi_xact->assoc_stream;
            ogs_assert(stream);

            ogs_sbi_xact_remove(sbi_xact);

            ogs_error("Cannot receive SBI message");
            ogs_sbi_server_send_error(stream,
                    OGS_SBI_HTTP_STATUS_GATEWAY_TIMEOUT, NULL,
                    "Cannot receive SBI message", NULL);
            break;

        default:
            ogs_error("Unknown timer[%s:%d]",
                    pcf_timer_get_name(e->timer_id), e->timer_id);
        }
        break;

    default:
        ogs_error("No handler for event %s", pcf_event_get_name(e));
        break;
    }
}
