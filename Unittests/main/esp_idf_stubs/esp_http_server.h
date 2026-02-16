#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    // Minimal stub for ESP-IDF esp_http_server.h

    typedef void *httpd_handle_t;
    typedef int esp_err_t;
    typedef struct httpd_req
    {
        int method;
        const char *uri;
        void *user_ctx;
    } httpd_req_t;
    typedef esp_err_t (*httpd_uri_handler_t)(httpd_req_t *);
    typedef struct
    {
        const char *uri;
        int method;
        httpd_uri_handler_t handler;
        void *user_ctx;
    } httpd_uri_t;
}
typedef struct
{
    int server_port;
    int max_uri_handlers;
} httpd_config_t;
typedef int httpd_err_code_t;

#define ESP_OK 0
#define ESP_FAIL -1
#define HTTP_GET 0
#define HTTP_POST 1
#define HTTPD_500_INTERNAL_SERVER_ERROR 500
#define HTTPD_404_NOT_FOUND 404
#define HTTPD_400_BAD_REQUEST 400
#define HTTPD_405_METHOD_NOT_ALLOWED 405

static inline httpd_config_t HTTPD_DEFAULT_CONFIG()
{
    httpd_config_t c = {80, 8};
    return c;
}
static inline int httpd_start(httpd_handle_t *h, const httpd_config_t *c)
{
    *h = (void *)1;
    return ESP_OK;
}
static inline int httpd_stop(httpd_handle_t h) { return ESP_OK; }
static inline int httpd_register_uri_handler(httpd_handle_t h, const httpd_uri_t *uri) { return ESP_OK; }
static inline int httpd_resp_send_err(httpd_req_t *req, httpd_err_code_t code, const char *msg) { return ESP_OK; }
static inline int httpd_resp_set_type(httpd_req_t *req, const char *type) { return ESP_OK; }
static inline int httpd_resp_send_chunk(httpd_req_t *req, const char *buf, size_t len) { return ESP_OK; }
static inline int httpd_resp_send(httpd_req_t *req, const char *buf, size_t len) { return ESP_OK; }
static inline int httpd_req_recv(httpd_req_t *req, char *buf, size_t bufsize) { return 0; }

#ifndef __GNUC__
#define __attribute__(unused)
#endif