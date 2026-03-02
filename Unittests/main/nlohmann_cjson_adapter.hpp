#pragma once

#ifdef _WIN32
#include "json.hpp"
using nlohmann::json;

// Adapter types
using cJSON = json;

// Adapter functions
inline cJSON *cJSON_Parse(const char *str)
{
    try
    {
        return new json(json::parse(str));
    }
    catch (...)
    {
        return nullptr;
    }
}

inline void cJSON_Delete(cJSON *j) { delete j; }

inline bool cJSON_IsArray(const cJSON *j) { return j && j->is_array(); }

inline bool cJSON_IsObject(const cJSON *j) { return j && j->is_object(); }

inline bool cJSON_IsNumber(const cJSON *j) { return j && (j->is_number_integer() || j->is_number_float()); }

inline bool cJSON_IsString(const cJSON *j) { return j && j->is_string(); }

inline int cJSON_GetArraySize(const cJSON *j) { return j && j->is_array() ? static_cast<int>(j->size()) : 0; }

inline cJSON *cJSON_GetArrayItem(const cJSON *j, int idx)
{
    if (!j || !j->is_array() || idx < 0 || idx >= j->size())
        return nullptr;
    return new json((*j)[idx]);
}

inline cJSON *cJSON_GetObjectItem(const cJSON *j, const char *key)
{
    if (!j || !j->is_object())
        return nullptr;
    auto it = j->find(key);
    if (it == j->end())
        return nullptr;
    return new json(*it);
}

inline char *cJSON_Print(const cJSON *j)
{
    if (!j)
        return nullptr;
    std::string s = j->dump(2);
    char *out = new char[s.size() + 1];
    strcpy(out, s.c_str());
    return out;
}

inline void cJSON_free(char *ptr) { delete[] ptr; }

#else
#include <cJSON.h>
#endif
