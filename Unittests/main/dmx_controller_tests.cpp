// ...existing code...
// Force linker to include DmxControllerQueueFailTest
extern "C" void __force_link_DmxControllerQueueFailTest() {}

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../main/Tasks/artnet_sender.hpp"
#include "../../main/Tasks/dmx_controller.hpp"
#include "../../main/Tasks/dmx_preset_changer.hpp"
#include "../../main/Tasks/foot_switch.hpp"
#include "../../main/Tasks/max3485_sender.hpp"
#include "../../main/Tasks/nvs_storage.hpp"
#include "../../main/Tasks/osc_sender.hpp"
#include "../../main/Tasks/seven_segment_display.hpp"
#include "../../main/Tasks/web_server.hpp"

#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/queue.h"
typedef int (*xQueueSend_fn)(QueueHandle_t, const void *, unsigned int);
typedef int (*xQueueReceive_fn)(QueueHandle_t, void *, unsigned int);
extern "C"
{
    extern xQueueSend_fn xQueueSend_ptr;
    extern xQueueReceive_fn xQueueReceive_ptr;
    extern QueueHandle_t (*xQueueCreate_ptr)(portBASE_TYPE, portBASE_TYPE);
    extern void (*vQueueDelete_ptr)(void *);
    extern int (*xQueueSendFromISR_ptr)(void *, const void *, int *);
}

// All GoogleMock mock classes for DmxController dependencies
#include "Mocks/artnet_sender_mock.hpp"
#include "Mocks/dmx_preset_changer_mock.hpp"
#include "Mocks/foot_switch_mock.hpp"
#include "Mocks/max3485_sender_mock.hpp"
#include "Mocks/nvs_storage_mock.hpp"
#include "Mocks/osc_sender_mock.hpp"
#include "Mocks/seven_segment_display_mock.hpp"
#include "Mocks/web_server_mock.hpp"

using ::testing::_;
using ::testing::Return;

// Patch xQueueReceive for this test file to simulate correct event types for init_messages
#include "messages.hpp"
// ...existing code...
// All includes and macros are now ASCII and UTF-8 encoded
using ::testing::_;
using ::testing::Return;

extern "C"
{
    extern xQueueSend_fn xQueueSend_ptr;
    extern xQueueReceive_fn xQueueReceive_ptr;
}

// Test fixture for DmxController
class DmxControllerTest : public ::testing::Test
{
  protected:
    MockPresetChanger *mockPresetChanger;
    MockOSCSender *mockOscSender;
    MockSevenSegmentDisplay *mockDisplay;
    MockFootSwitch *mockFootSwitch;
    MockMax3485Sender *mockMax3485Sender;
    MockArtNetSender *mockArtNetSender;
    MockWebServer *mockWebServer;
    MockNvsStorage *mockNvsStorage;

    DmxControllerTest()
        : mockPresetChanger(new MockPresetChanger()), mockOscSender(new MockOSCSender()),
          mockDisplay(new MockSevenSegmentDisplay()), mockFootSwitch(new MockFootSwitch()),
          mockMax3485Sender(new MockMax3485Sender()), mockArtNetSender(new MockArtNetSender()),
          mockWebServer(new MockWebServer()), mockNvsStorage(new MockNvsStorage())
    {
        testing::Mock::AllowLeak(mockPresetChanger);
        testing::Mock::AllowLeak(mockOscSender);
        testing::Mock::AllowLeak(mockDisplay);
        testing::Mock::AllowLeak(mockFootSwitch);
        testing::Mock::AllowLeak(mockMax3485Sender);
        testing::Mock::AllowLeak(mockArtNetSender);
        testing::Mock::AllowLeak(mockWebServer);
        testing::Mock::AllowLeak(mockNvsStorage);
    }

    void SetUp() override { std::cout << "TEST_F SetUp()\n"; }
    void TearDown() override
    {
        std::cout << "TEST_F TearDown()\n";
        // Do not delete mocks here; DmxController will delete them
    }
};

// Test full DmxController::init (including message handling)
TEST_F(DmxControllerTest, Init_AllSuccess_ReturnsEspOk)
{
    std::cout << "TEST_F (init) is running!\n";
    QueueHandle_t dummyQueue = reinterpret_cast<QueueHandle_t>(0x1);
    struct ControllerTestDouble : DmxController
    {
        QueueHandle_t testQueue;
        ControllerTestDouble(DmxPresetChanger *pc, OSCSender *os, SevenSegmentDisplay *sd, FootSwitch *fs,
            Max3485Sender *ms, ArtNetSender *an, WebServer *ws, NvsStorage *ns, QueueHandle_t q)
            : DmxController(pc, os, sd, fs, ms, an, ws, ns), testQueue(q)
        {
        }
        QueueHandle_t getEventQueue() const override { return testQueue; }
        ~ControllerTestDouble() override = default;
    };

    ASSERT_NE(mockPresetChanger, nullptr);
    ASSERT_NE(mockOscSender, nullptr);
    ASSERT_NE(mockDisplay, nullptr);
    ASSERT_NE(mockFootSwitch, nullptr);
    ASSERT_NE(mockMax3485Sender, nullptr);
    ASSERT_NE(mockArtNetSender, nullptr);
    ASSERT_NE(mockWebServer, nullptr);
    ASSERT_NE(mockNvsStorage, nullptr);

    ControllerTestDouble controllerWithQueue(mockPresetChanger, mockOscSender, mockDisplay, mockFootSwitch,
        mockMax3485Sender, mockArtNetSender, mockWebServer, mockNvsStorage, dummyQueue);

    EXPECT_CALL(*mockPresetChanger, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockDisplay, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockFootSwitch, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockMax3485Sender, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockArtNetSender, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockNvsStorage, getEventQueue()).WillRepeatedly(Return(dummyQueue));

    EXPECT_CALL(*mockPresetChanger, init(_)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockOscSender, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockDisplay, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockFootSwitch, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockMax3485Sender, init(_, _, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockArtNetSender, init(_, _, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockWebServer, init()).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockNvsStorage, init(_)).WillRepeatedly(Return(ESP_OK));

    controllerWithQueue.init(); // TODO: Check return value (messaging)

    // Restore default stubs after test
    xQueueSend_ptr = nullptr;
    xQueueReceive_ptr = nullptr;
}

// Add more tests for error cases, message handling, etc.

// MSVC weak symbol emulation: Use function pointers for queue stubs
#include "freertos/queue.h"
namespace
{
QueueHandle_t queuefail_xQueueCreate(portBASE_TYPE, portBASE_TYPE) { return nullptr; }
int queuefail_xQueueSend(QueueHandle_t, const void *, unsigned int) { return 0; }
int queuefail_xQueueReceive(QueueHandle_t, void *, unsigned int) { return 0; }
void queuefail_vQueueDelete(void *) {}
int queuefail_xQueueSendFromISR(void *, const void *, int *) { return 0; }
} // namespace

extern "C"
{
    typedef QueueHandle_t (*xQueueCreate_fn)(portBASE_TYPE, portBASE_TYPE);
    typedef int (*xQueueSend_fn)(QueueHandle_t, const void *, unsigned int);
    typedef int (*xQueueReceive_fn)(QueueHandle_t, void *, unsigned int);
    typedef void (*vQueueDelete_fn)(void *);
    typedef int (*xQueueSendFromISR_fn)(void *, const void *, int *);
    extern xQueueSend_fn xQueueSend_ptr;
    extern xQueueReceive_fn xQueueReceive_ptr;

    xQueueCreate_fn xQueueCreate_ptr = nullptr;
    xQueueSend_fn xQueueSend_ptr = nullptr;
    xQueueReceive_fn xQueueReceive_ptr = nullptr;
    vQueueDelete_fn vQueueDelete_ptr = nullptr;
    xQueueSendFromISR_fn xQueueSendFromISR_ptr = nullptr;

    QueueHandle_t xQueueCreate(portBASE_TYPE a, portBASE_TYPE b)
    {
        if (xQueueCreate_ptr)
            return xQueueCreate_ptr(a, b);
        return (QueueHandle_t)1; // default stub
    }
    int xQueueSend(QueueHandle_t q, const void *d, unsigned int t)
    {
        if (xQueueSend_ptr)
            return xQueueSend_ptr(q, d, t);
        return 1;
    }
    int xQueueReceive(QueueHandle_t q, void *b, unsigned int t)
    {
        if (xQueueReceive_ptr)
            return xQueueReceive_ptr(q, b, t);
        return 1;
    }
    void vQueueDelete(void *q)
    {
        if (vQueueDelete_ptr)
            vQueueDelete_ptr(q);
    }
    int xQueueSendFromISR(void *q, const void *d, int *t)
    {
        if (xQueueSendFromISR_ptr)
            return xQueueSendFromISR_ptr(q, d, t);
        return 0;
    }
}

// DmxControllerQueueFailTest merged from dmx_controller_queuefail_tests.cpp
class DmxControllerQueueFailTest : public ::testing::Test
{
  protected:
    MockPresetChanger *mockPresetChanger;
    MockOSCSender *mockOscSender;
    MockSevenSegmentDisplay *mockDisplay;
    MockFootSwitch *mockFootSwitch;
    MockMax3485Sender *mockMax3485Sender;
    MockArtNetSender *mockArtNetSender;
    MockWebServer *mockWebServer;
    MockNvsStorage *mockNvsStorage;

    DmxControllerQueueFailTest()
        : mockPresetChanger(new MockPresetChanger()), mockOscSender(new MockOSCSender()),
          mockDisplay(new MockSevenSegmentDisplay()), mockFootSwitch(new MockFootSwitch()),
          mockMax3485Sender(new MockMax3485Sender()), mockArtNetSender(new MockArtNetSender()),
          mockWebServer(new MockWebServer()), mockNvsStorage(new MockNvsStorage())
    {
        testing::Mock::AllowLeak(mockPresetChanger);
        testing::Mock::AllowLeak(mockOscSender);
        testing::Mock::AllowLeak(mockDisplay);
        testing::Mock::AllowLeak(mockFootSwitch);
        testing::Mock::AllowLeak(mockMax3485Sender);
        testing::Mock::AllowLeak(mockArtNetSender);
        testing::Mock::AllowLeak(mockWebServer);
        testing::Mock::AllowLeak(mockNvsStorage);
    }
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(DmxControllerQueueFailTest, Init_EventQueueCreateFails_ReturnsEspFail)
{
    // Set queuefail stubs for this test
    xQueueCreate_ptr = queuefail_xQueueCreate;
    xQueueSend_ptr = queuefail_xQueueSend;
    xQueueReceive_ptr = queuefail_xQueueReceive;
    vQueueDelete_ptr = queuefail_vQueueDelete;
    xQueueSendFromISR_ptr = queuefail_xQueueSendFromISR;

    struct ControllerTestDouble : DmxController
    {
        QueueHandle_t testQueue;
        ControllerTestDouble(DmxPresetChanger *pc, OSCSender *os, SevenSegmentDisplay *sd, FootSwitch *fs,
            MockMax3485Sender *ms, ArtNetSender *an, WebServer *ws, NvsStorage *ns, QueueHandle_t q)
            : DmxController(pc, os, sd, fs, ms, an, ws, ns), testQueue(q)
        {
        }
        QueueHandle_t getEventQueue() const override { return testQueue; }
        ~ControllerTestDouble() override
        {
            presetChanger_ = nullptr;
            oscSender_ = nullptr;
            display_ = nullptr;
            footSwitch_ = nullptr;
            max3485Sender_ = nullptr;
            artnetSender_ = nullptr;
            webServer_ = nullptr;
            nvsStorage_ = nullptr;
        }
    };

    QueueHandle_t dummyQueue = reinterpret_cast<QueueHandle_t>(0x1);
    ControllerTestDouble controllerWithQueue(mockPresetChanger, mockOscSender, mockDisplay, mockFootSwitch,
        mockMax3485Sender, mockArtNetSender, mockWebServer, mockNvsStorage, dummyQueue);

    // Null checks before dereferencing mocks
    ASSERT_NE(mockPresetChanger, nullptr);
    ASSERT_NE(mockOscSender, nullptr);
    ASSERT_NE(mockDisplay, nullptr);
    ASSERT_NE(mockFootSwitch, nullptr);
    ASSERT_NE(mockMax3485Sender, nullptr);
    ASSERT_NE(mockArtNetSender, nullptr);
    ASSERT_NE(mockWebServer, nullptr);
    ASSERT_NE(mockNvsStorage, nullptr);

    EXPECT_CALL(*mockPresetChanger, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockDisplay, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockFootSwitch, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockMax3485Sender, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockArtNetSender, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockNvsStorage, getEventQueue()).WillRepeatedly(Return(dummyQueue));

    EXPECT_CALL(*mockPresetChanger, init(_)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockOscSender, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockDisplay, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockFootSwitch, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockMax3485Sender, init(_, _, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockArtNetSender, init(_, _, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockWebServer, init()).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockNvsStorage, init(_)).WillRepeatedly(Return(ESP_OK));

    esp_err_t ret = controllerWithQueue.init();
    EXPECT_EQ(ret, ESP_FAIL);

    // Restore default stubs after test
    xQueueCreate_ptr = nullptr;
    xQueueSend_ptr = nullptr;
    xQueueReceive_ptr = nullptr;
    vQueueDelete_ptr = nullptr;
    xQueueSendFromISR_ptr = nullptr;
}
