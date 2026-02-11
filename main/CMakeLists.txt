 # Treat all warnings as errors for C++
 idf_component_register(SRCS "dmx_controller.cpp" "rtos_task.cpp" "main.cpp" "foot_switch.cpp" "dmx_preset_changer.cpp" "nvs_storage.cpp" "osc_sender.cpp" "seven_segment_display.cpp" "dmx_preset.cpp" "dmx_presets.cpp" "artnet_sender.cpp" "web_server.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES esp_https_ota app_update nvs_flash esp_wifi esp_event driver json  esp_http_server spiffs)

