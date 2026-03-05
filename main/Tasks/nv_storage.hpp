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
#include "../Base/assert.hpp"
#include "../Base/rtos_task.hpp"
#include "../Data/configuration.hpp"
#include "../Data/dmx_presets.hpp"
#include "../messages.hpp"

class NvStorage : public RtosTask
{
  public:
    NvStorage(IAssert *assert, Configuration &configuration, DmxPresets &dmxPresets);
    ~NvStorage();

    virtual void init(RtosTask::TaskProperties taskProperties);

  private:
    int taskPriority_;
    int queueCapacity_;

    void loadConfiguration();
    void loadDmxPresets();

    Configuration &getConfiguration() { return configuration_; }
    DmxPresets &getDmxPresets() { return dmxPresets_; }

  private:
    nvs_handle_t configurationNvsHandle_;
    nvs_handle_t presetsNvsHandle_;
    const char *configurationNamespaceName_;
    const char *presetsNamespaceName_;

    void taskEntry(void *param) override;
    void taskLoop();

    Configuration &configuration_;
    DmxPresets &dmxPresets_;
    IAssert *assert_;
};

#endif // NV_STORAGE_HPP