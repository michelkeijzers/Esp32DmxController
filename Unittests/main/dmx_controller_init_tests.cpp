#include "../../main/Tasks/dmx_controller.hpp"
#include "Mocks/foot_switch_mock.hpp"
#include "Mocks/max3485_sender_mock.hpp"
#include "Mocks/nv_storage_mock.hpp"
#include "Mocks/seven_segment_display_mock.hpp"
#include "Mocks/web_server_mock.hpp"
#include "messages.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
using ::testing::_;
using ::testing::Return;

// Test fixture for DmxController init
class DmxControllerInitTest : public ::testing::Test
{
  protected:
    MockSevenSegmentDisplay *mockDisplay;
    MockFootSwitch *mockFootSwitch;
    MockMax3485Sender *mockMax3485Sender;
    MockWebServer *mockWebServer;
    MockNvStorage *mockNvStorage;

    DmxControllerInitTest()
        : mockDisplay(new MockSevenSegmentDisplay()), mockFootSwitch(new MockFootSwitch()),
          mockMax3485Sender(new MockMax3485Sender()), mockWebServer(new MockWebServer()),
          mockNvStorage(new MockNvStorage())
    {
        testing::Mock::AllowLeak(mockDisplay);
        testing::Mock::AllowLeak(mockFootSwitch);
        testing::Mock::AllowLeak(mockMax3485Sender);
        testing::Mock::AllowLeak(mockWebServer);
        testing::Mock::AllowLeak(mockNvStorage);
    }
    void SetUp() override {}
    void TearDown() override
    {
        mockDisplay = nullptr;
        mockFootSwitch = nullptr;
        mockMax3485Sender = nullptr;
        mockWebServer = nullptr;
        mockNvStorage = nullptr;
    }
};

TEST_F(DmxControllerInitTest, Init_AllSuccess_ReturnsEspOk)
{
    // ...copy the test body from your original file...
}
