#pragma once
#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>
    typedef struct cJSON
    {
        int type;
        char *valuestring;
        double valuedouble;
    } cJSON;

    static inline cJSON *cJSON_CreateObject(void)
    {
        static cJSON obj;
        return &obj;
    }
    static inline cJSON *cJSON_CreateArray(void)
    {
        static cJSON arr;
        return &arr;
    }
    static inline cJSON *cJSON_CreateNumber(double)
    {
        static cJSON num;
        return &num;
    }
    static inline void cJSON_Delete(cJSON *) {}
    static inline cJSON *cJSON_Parse(const char *)
    {
        static cJSON obj;
        return &obj;
    }
    static inline char *cJSON_Print(const cJSON *)
    {
        static char dummy[4] = "{}";
        return dummy;
    }
    static inline void cJSON_free(void *) {}
    static inline int cJSON_IsArray(const cJSON *) { return 1; }
    static inline int cJSON_IsObject(const cJSON *) { return 1; }
    static inline int cJSON_IsNumber(const cJSON *) { return 1; }
    static inline int cJSON_IsString(const cJSON *) { return 1; }
    static inline int cJSON_GetArraySize(const cJSON *) { return 0; }
    static inline cJSON *cJSON_GetArrayItem(const cJSON *, int)
    {
        static cJSON item;
        return &item;
    }
    static inline cJSON *cJSON_GetObjectItem(const cJSON *, const char *)
    {
        static cJSON item;
        return &item;
    }
    static inline void cJSON_AddItemToArray(cJSON *, cJSON *) {}
    static inline void cJSON_AddItemToObject(cJSON *, const char *, cJSON *) {}
    static inline void cJSON_AddNumberToObject(cJSON *, const char *, double) {}
    static inline void cJSON_AddStringToObject(cJSON *, const char *, const char *) {}
#ifdef __cplusplus
}
#endif
