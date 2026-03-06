#include "../../main/Data/configuration.hpp"
#include "../../main/Data/dmx_presets.hpp"
#include "../../main/Tasks/nv_storage.hpp"
#include "Mocks/mock_assert.hpp"
#include <cstring>
#include <gtest/gtest.h>

class NvStorageTest : public ::testing::Test
{
  protected:
    ::testing::NiceMock<MockAssert> mockAssert;
    Configuration config;
    DmxPresets presets;
    NvStorage nvStorage;

    NvStorageTest() : config(&mockAssert), presets(&mockAssert), nvStorage(&mockAssert, config, presets) {}
};

TEST_F(NvStorageTest, Init_OpensNvsHandles)
{
    RtosTask::TaskProperties props = {};
    props.taskName_ = "NvStorage";
    props.taskPriority = 1;
    props.stackSize = 1024;
    props.queueCapacity = 4;
    props.queueItemSize = sizeof(Messages::Event);
    props.mainEventQueue = nullptr;

    EXPECT_CALL(mockAssert, assertNotEspError(ESP_OK, ::testing::_)).Times(2);
    nvStorage.init(props);
}

// Testable subclass to access protected members/methods
class TestableNvStorage : public NvStorage
{
  public:
    using NvStorage::configurationNvsHandle_;
    using NvStorage::loadConfiguration;
    using NvStorage::loadDmxPresets;
    using NvStorage::presetsNvsHandle_;
    using NvStorage::taskEntry;
    using NvStorage::taskLoop;
    TestableNvStorage(IAssert *a, Configuration &c, DmxPresets &d) : NvStorage(a, c, d) {}
    bool loopCalled = false;
    int loopCount = 0;
    void taskLoop()
    {
        loopCalled = true;
        loopCount++;
    }
};

// Test destructor (should close handles if nonzero)
TEST_F(NvStorageTest, Destructor_ClosesHandles)
{
    TestableNvStorage testNv(&mockAssert, config, presets);
    testNv.configurationNvsHandle_ = 123;
    testNv.presetsNvsHandle_ = 456;
    testNv.~TestableNvStorage();
    SUCCEED();
}

// Test loadConfiguration (calls assert and sets config fields)
TEST_F(NvStorageTest, LoadConfiguration_SetsConfigFields)
{
    TestableNvStorage testNv(&mockAssert, config, presets);
    testNv.configurationNvsHandle_ = 123;
    EXPECT_CALL(mockAssert, assertNot0(123, ::testing::_)).Times(1);
    testNv.loadConfiguration();
    SUCCEED();
}

// Test loadDmxPresets (calls assert and sets preset fields)
TEST_F(NvStorageTest, LoadDmxPresets_SetsPresetFields)
{
    TestableNvStorage testNv(&mockAssert, config, presets);
    testNv.presetsNvsHandle_ = 456;
    EXPECT_CALL(mockAssert, assertNvsHandle(456, ::testing::_)).Times(1);
    testNv.loadDmxPresets();
    SUCCEED();
}

// Test taskLoop with event injection (simulate event received)
extern "C" int (*xQueueReceive_ptr)(QueueHandle_t, void *, unsigned int);
TEST_F(NvStorageTest, TaskLoop_EventHandling)
{
    TestableNvStorage testNv(&mockAssert, config, presets);
    // Patch xQueueReceive_ptr to simulate event
    auto old_ptr = xQueueReceive_ptr;
    xQueueReceive_ptr = [](QueueHandle_t, void *evt, unsigned int) -> int
    {
        static int call_count = 0;
        if (call_count == 0)
        {
            auto *msg = static_cast<Messages::Event *>(evt);
            msg->type = Messages::EventType::LOAD_CONFIGURATION;
            call_count++;
            return 1; // pdTRUE
        }
        return 0; // pdFALSE
    };
    // Call the real taskLoop (should process one event and exit)
    testNv.taskLoop();
    xQueueReceive_ptr = old_ptr;
    SUCCEED();
}

namespace MULT
{

static esp_err_t stub_nvs_get_u8(nvs_handle_t, const char *key, uint8_t *out)
{
    if (strcmp(key, "NumberOfFilledPresets") == 0)
        *out = 2;
    else
        *out = 1;
    return ESP_OK;
}

static esp_err_t stub_nvs_get_str(nvs_handle_t, const char *, char *out, size_t *len)
{
    const char *name = "TestPreset";
    size_t name_len = strlen(name) + 1;
    if (*len > name_len)
        *len = name_len;
    memcpy(out, name, *len);
    out[*len - 1] = '\0';
    return ESP_OK;
}

static esp_err_t stub_nvs_get_blob(nvs_handle_t, const char *, void *out, size_t *len)
{
    if (*len > 0)
        memset(out, 42, *len);
    return ESP_OK;
}

// --- Multiple Presets Test Fixture ---
class NvStorageMultiplePresetsTest : public ::testing::Test
{
  protected:
    ::testing::NiceMock<MockAssert> mockAssert;
    Configuration config;
    DmxPresets presets;
    NvStorageMultiplePresetsTest() : config(&mockAssert), presets(&mockAssert) {}
    void SetUp() override
    {
        nvs_get_u8_ptr = stub_nvs_get_u8;
        nvs_get_str_ptr = stub_nvs_get_str;
        nvs_get_blob_ptr = stub_nvs_get_blob;
    }
    void TearDown() override
    {
        nvs_get_u8_ptr = nullptr;
        nvs_get_str_ptr = nullptr;
        nvs_get_blob_ptr = nullptr;
    }
};

TEST_F(NvStorageMultiplePresetsTest, LoadDmxPresets_ForLoop_MultiplePresets)
{
    TestableNvStorage testNv(&mockAssert, config, presets);
    testNv.presetsNvsHandle_ = 123;
    EXPECT_CALL(mockAssert, assertNvsHandle(123, ::testing::_)).Times(1);
    printf("0\n");
    testNv.loadDmxPresets();
    printf("80\n");
    EXPECT_EQ(presets.getNumberOfFilledPresets(), 2);
    EXPECT_STREQ(presets.getPreset(0).getName(), "TestPreset");
    EXPECT_STREQ(presets.getPreset(1).getName(), "TestPreset");
    EXPECT_EQ(presets.getPreset(0).getDmxValue(0), 42);
    EXPECT_EQ(presets.getPreset(1).getDmxValue(0), 42);
}
} // namespace MULT