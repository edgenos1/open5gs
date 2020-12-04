/*
 * policy_association_release_cause.h
 *
 * Possible values are - UNSPECIFIED: This value is used for unspecified reasons. - UE_SUBSCRIPTION: This value is used to indicate that the session needs to be terminated because the subscription of UE has changed (e.g. was removed). - INSUFFICIENT_RES: This value is used to indicate that the server is overloaded and needs to abort the session.
 */

#ifndef _OpenAPI_policy_association_release_cause_H_
#define _OpenAPI_policy_association_release_cause_H_

#include <string.h>
#include "../external/cJSON.h"
#include "../include/list.h"
#include "../include/keyValuePair.h"
#include "../include/binary.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OpenAPI_policy_association_release_cause_s OpenAPI_policy_association_release_cause_t;
typedef struct OpenAPI_policy_association_release_cause_s {
} OpenAPI_policy_association_release_cause_t;

OpenAPI_policy_association_release_cause_t *OpenAPI_policy_association_release_cause_create(
    );
void OpenAPI_policy_association_release_cause_free(OpenAPI_policy_association_release_cause_t *policy_association_release_cause);
OpenAPI_policy_association_release_cause_t *OpenAPI_policy_association_release_cause_parseFromJSON(cJSON *policy_association_release_causeJSON);
cJSON *OpenAPI_policy_association_release_cause_convertToJSON(OpenAPI_policy_association_release_cause_t *policy_association_release_cause);
OpenAPI_policy_association_release_cause_t *OpenAPI_policy_association_release_cause_copy(OpenAPI_policy_association_release_cause_t *dst, OpenAPI_policy_association_release_cause_t *src);

#ifdef __cplusplus
}
#endif

#endif /* _OpenAPI_policy_association_release_cause_H_ */

