
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "policy_association_release_cause.h"

OpenAPI_policy_association_release_cause_t *OpenAPI_policy_association_release_cause_create(
    )
{
    OpenAPI_policy_association_release_cause_t *policy_association_release_cause_local_var = OpenAPI_malloc(sizeof(OpenAPI_policy_association_release_cause_t));
    if (!policy_association_release_cause_local_var) {
        return NULL;
    }

    return policy_association_release_cause_local_var;
}

void OpenAPI_policy_association_release_cause_free(OpenAPI_policy_association_release_cause_t *policy_association_release_cause)
{
    if (NULL == policy_association_release_cause) {
        return;
    }
    OpenAPI_lnode_t *node;
    ogs_free(policy_association_release_cause);
}

cJSON *OpenAPI_policy_association_release_cause_convertToJSON(OpenAPI_policy_association_release_cause_t *policy_association_release_cause)
{
    cJSON *item = NULL;

    if (policy_association_release_cause == NULL) {
        ogs_error("OpenAPI_policy_association_release_cause_convertToJSON() failed [PolicyAssociationReleaseCause]");
        return NULL;
    }

    item = cJSON_CreateObject();
end:
    return item;
}

OpenAPI_policy_association_release_cause_t *OpenAPI_policy_association_release_cause_parseFromJSON(cJSON *policy_association_release_causeJSON)
{
    OpenAPI_policy_association_release_cause_t *policy_association_release_cause_local_var = NULL;
    policy_association_release_cause_local_var = OpenAPI_policy_association_release_cause_create (
        );

    return policy_association_release_cause_local_var;
end:
    return NULL;
}

OpenAPI_policy_association_release_cause_t *OpenAPI_policy_association_release_cause_copy(OpenAPI_policy_association_release_cause_t *dst, OpenAPI_policy_association_release_cause_t *src)
{
    cJSON *item = NULL;
    char *content = NULL;

    ogs_assert(src);
    item = OpenAPI_policy_association_release_cause_convertToJSON(src);
    if (!item) {
        ogs_error("OpenAPI_policy_association_release_cause_convertToJSON() failed");
        return NULL;
    }

    content = cJSON_Print(item);
    cJSON_Delete(item);

    if (!content) {
        ogs_error("cJSON_Print() failed");
        return NULL;
    }

    item = cJSON_Parse(content);
    ogs_free(content);
    if (!item) {
        ogs_error("cJSON_Parse() failed");
        return NULL;
    }

    OpenAPI_policy_association_release_cause_free(dst);
    dst = OpenAPI_policy_association_release_cause_parseFromJSON(item);
    cJSON_Delete(item);

    return dst;
}

