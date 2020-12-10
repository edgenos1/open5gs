/*
 * Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>
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

#include "npcf-handler.h"

#include "sbi-path.h"
#include "nas-path.h"

int smf_npcf_smpolicycontrol_handle_create(
        smf_sess_t *sess, ogs_sbi_message_t *recvmsg)
{
    int rv;
    smf_ue_t *smf_ue = NULL;

    OpenAPI_sm_policy_decision_t *SmPolicyDecision = NULL;

    ogs_sbi_message_t message;
    ogs_sbi_header_t header;

    ogs_assert(sess);
    smf_ue = sess->smf_ue;
    ogs_assert(smf_ue);
    ogs_assert(recvmsg);

    if (recvmsg->res_status != OGS_SBI_HTTP_STATUS_CREATED) {
        ogs_error("[%s:%d] HTTP response error [%d]",
                smf_ue->supi, sess->psi, recvmsg->res_status);
        return OGS_ERROR;
    }

    if (!recvmsg->http.location) {
        ogs_error("[%s:%d] No http.location", smf_ue->supi, sess->psi);
        return OGS_ERROR;
    }

    SmPolicyDecision = recvmsg->SmPolicyDecision;
    if (!SmPolicyDecision) {
        ogs_error("[%s:%d] No SmPolicyDecision", smf_ue->supi, sess->psi);
        return OGS_ERROR;
    }

    memset(&header, 0, sizeof(header));
    header.uri = recvmsg->http.location;

    rv = ogs_sbi_parse_header(&message, &header);
    if (rv != OGS_OK) {
        ogs_error("[%s:%d] Cannot parse http.location [%s]",
                smf_ue->supi, sess->psi, recvmsg->http.location);
        return OGS_ERROR;
    }

    if (!message.h.resource.component[1]) {
        ogs_error("[%s:%d] No Assocation ID [%s]",
                smf_ue->supi, sess->psi, recvmsg->http.location);

        ogs_sbi_header_free(&header);
        return OGS_ERROR;
    }

    if (sess->policy_association_id)
        ogs_free(sess->policy_association_id);
    sess->policy_association_id = ogs_strdup(message.h.resource.component[1]);

    ogs_sbi_header_free(&header);

    return OGS_OK;
}
