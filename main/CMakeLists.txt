# Treat all warnings as errors for C++
 idf_component_register(
    SRCS 
        "Tasks/max3485_sender.cpp"
        "Tasks/foot_switch.cpp"
        "Tasks/dmx_preset_changer.cpp"
        "Tasks/nvs_storage.cpp"
        "Tasks/osc_sender.cpp"
        "Tasks/seven_segment_display.cpp"
        "Tasks/artnet_sender.cpp"
        "Tasks/web_server.cpp"
        "Tasks/dmx_controller.cpp"
        "Base/rtos_task.cpp"
        "main.cpp"
        "Data/dmx_presets.cpp"
    INCLUDE_DIRS "." "Tasks"
    REQUIRES esp_https_ota app_update nvs_flash esp_wifi esp_event driver json esp_http_server spiffs
)

