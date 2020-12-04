
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "request_trigger.h"

OpenAPI_request_trigger_t *OpenAPI_request_trigger_create(
    )
{
    OpenAPI_request_trigger_t *request_trigger_local_var = OpenAPI_malloc(sizeof(OpenAPI_request_trigger_t));
    if (!request_trigger_local_var) {
        return NULL;
    }

    return request_trigger_local_var;
}

void OpenAPI_request_trigger_free(OpenAPI_request_trigger_t *request_trigger)
{
    if (NULL == request_trigger) {
        return;
    }
    OpenAPI_lnode_t *node;
    ogs_free(request_trigger);
}

cJSON *OpenAPI_request_trigger_convertToJSON(OpenAPI_request_trigger_t *request_trigger)
{
    cJSON *item = NULL;

    if (request_trigger == NULL) {
        ogs_error("OpenAPI_request_trigger_convertToJSON() failed [RequestTrigger]");
        return NULL;
    }

    item = cJSON_CreateObject();
end:
    return item;
}

OpenAPI_request_trigger_t *OpenAPI_request_trigger_parseFromJSON(cJSON *request_triggerJSON)
{
    OpenAPI_request_trigger_t *request_trigger_local_var = NULL;
    request_trigger_local_var = OpenAPI_request_trigger_create (
        );

    return request_trigger_local_var;
end:
    return NULL;
}

OpenAPI_request_trigger_t *OpenAPI_request_trigger_copy(OpenAPI_request_trigger_t *dst, OpenAPI_request_trigger_t *src)
{
    cJSON *item = NULL;
    char *content = NULL;

    ogs_assert(src);
    item = OpenAPI_request_trigger_convertToJSON(src);
    if (!item) {
        ogs_error("OpenAPI_request_trigger_convertToJSON() failed");
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

    OpenAPI_request_trigger_free(dst);
    dst = OpenAPI_request_trigger_parseFromJSON(item);
    cJSON_Delete(item);

    return dst;
}

