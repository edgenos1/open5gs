/*
 * request_trigger.h
 *
 * Possible values are - LOC_CH: Location change (tracking area). The tracking area of the UE has changed. - PRA_CH: Change of UE presence in PRA. The UE is entering/leaving a Presence Reporting Area. - SERV_AREA_CH: Service Area Restriction change. The UDM notifies the AMF that the subscribed service area restriction information has changed. - RFSP_CH: RFSP index change. The UDM notifies the AMF that the subscribed RFSP index has changed. - ALLOWED_NSSAI_CH: Allowed NSSAI change. The AMF notifies that the set of UE allowed S-NSSAIs has changed. - UE_AMBR_CH: UE-AMBR change. The UDM notifies the AMF that the subscribed UE-AMBR has changed. - SMF_SELECT_CH: SMF selection information change. The UE requested for an unsupported DNN or UE requested for a DNN within the list of DNN candidates for replacement per S-NSSAI. - ACCESS_TYPE_CH: Access Type change. The the AMF notifies that the access type and the RAT type combinations available in the AMF for a UE with simultaneous 3GPP and non-3GPP connectivity has changed.  - RG_TMBR_CH: Subscribed RG-TMBR change. The UDM notifies the AMF that the subscribed RG-TMBR has changed.
 */

#ifndef _OpenAPI_request_trigger_H_
#define _OpenAPI_request_trigger_H_

#include <string.h>
#include "../external/cJSON.h"
#include "../include/list.h"
#include "../include/keyValuePair.h"
#include "../include/binary.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OpenAPI_request_trigger_s OpenAPI_request_trigger_t;
typedef struct OpenAPI_request_trigger_s {
} OpenAPI_request_trigger_t;

OpenAPI_request_trigger_t *OpenAPI_request_trigger_create(
    );
void OpenAPI_request_trigger_free(OpenAPI_request_trigger_t *request_trigger);
OpenAPI_request_trigger_t *OpenAPI_request_trigger_parseFromJSON(cJSON *request_triggerJSON);
cJSON *OpenAPI_request_trigger_convertToJSON(OpenAPI_request_trigger_t *request_trigger);
OpenAPI_request_trigger_t *OpenAPI_request_trigger_copy(OpenAPI_request_trigger_t *dst, OpenAPI_request_trigger_t *src);

#ifdef __cplusplus
}
#endif

#endif /* _OpenAPI_request_trigger_H_ */

