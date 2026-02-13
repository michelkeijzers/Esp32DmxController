#ifndef NVS_STORAGE_HPP
#define NVS_STORAGE_HPP

#include <esp_err.h>
#include <nvs.h>
#include <string>
#include <vector>

extern "C" {
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
}
#include "dmx_presets.hpp"
#include "messages.hpp"
#include "rtos_task.hpp"

class NvsStorage : public RtosTask {
  public:
    NvsStorage();
    ~NvsStorage();

    esp_err_t init(QueueHandle_t dmxControllerEventQueue);

    // Synchronous wrappers (for compatibility)
    esp_err_t setConfiguration(const Messages::ConfigurationEventData &config);
    esp_err_t requestConfiguration(Messages::ConfigurationEventData &config);
    esp_err_t setPresets(const Messages::PresetsEventData &presets);
    esp_err_t requestPresets(Messages::PresetsEventData &presets);

  private:
    nvs_handle_t configuration_nvs_handle;
    nvs_handle_t presets_nvs_handle;
    const char *configuration_namespace_name;
    const char *presets_namespace_name;

    void taskEntry(void *param) override;
    void taskLoop();
};

#endif // NVS_STORAGE_HPP