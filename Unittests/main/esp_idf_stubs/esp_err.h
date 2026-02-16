#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERROR_CHECK(x) (void)(x)
#define ESP_ERR_INVALID_ARG -2
#define ESP_ERR_INVALID_STATE -3

#ifdef __cplusplus
}
#endif
