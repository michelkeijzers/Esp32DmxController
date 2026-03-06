#include <gtest/gtest.h>
#include "../../main/Tasks/nv_storage.hpp"
#include "../../main/Data/configuration.hpp"
#include "../../main/Data/dmx_presets.hpp"
#include "Mocks/mock_assert.hpp"


#include "Mocks/mock_assert.hpp"

class NvStorageTest : public ::testing::Test {
protected:
    ::testing::NiceMock<MockAssert> mockAssert;
    Configuration config;
    DmxPresets presets;
    NvStorage nvStorage;

    NvStorageTest()
        : config(&mockAssert), presets(&mockAssert), nvStorage(&mockAssert, config, presets) {}
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

// Add more tests for loadConfiguration, loadDmxPresets, and error cases as needed.
