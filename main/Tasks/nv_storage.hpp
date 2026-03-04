#ifndef NV_STORAGE_HPP
#define NV_STORAGE_HPP

#include <esp_err.h>
#include <nvs.h>
#include <string>
#include <vector>

extern "C"
{
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
}
#include "../Data/dmx_presets.hpp"
#include "Base/rtos_task.hpp"
#include "messages.hpp"

class NvStorage : public RtosTask
{
  public:
    NvStorage();
    ~NvStorage();

    virtual void init(RtosTask::TaskProperties taskProperties);

  private:
    const char *log_tag_;
    int task_priority_;
    int queue_capacity_;

    // Synchronous wrappers (for compatibility)
    void setConfiguration(const Messages::ConfigurationEventData &config);
    void requestConfiguration(Messages::ConfigurationEventData &config);
    void setPresets(const Messages::PresetsEventData &presets);
    void requestPresets(Messages::PresetsEventData &presets);

  private:
    nvs_handle_t configuration_nvs_handle;
    nvs_handle_t presets_nvs_handle;
    const char *configuration_namespace_name;
    const char *presets_namespace_name;

    void taskEntry(void *param) override;
    void taskLoop();
};

#endif // NV_STORAGE_HPP