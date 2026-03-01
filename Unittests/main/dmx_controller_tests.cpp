// Force linker to include DmxControllerQueueFailTest
extern "C" void __force_link_DmxControllerQueueFailTest() {}

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../main/Tasks/dmx_controller.hpp"
#include "../../main/Tasks/dmx_preset_changer.hpp"
#include "../../main/Tasks/foot_switch.hpp"
#include "../../main/Tasks/max3485_sender.hpp"
#include "../../main/Tasks/nv_storage.hpp"
#include "../../main/Tasks/seven_segment_display.hpp"
#include "../../main/Tasks/web_server.hpp"

#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/queue.h"

// All GoogleMock mock classes for DmxController dependencies
#include "Mocks/dmx_preset_changer_mock.hpp"
#include "Mocks/foot_switch_mock.hpp"
#include "Mocks/max3485_sender_mock.hpp"
#include "Mocks/nv_storage_mock.hpp"
#include "Mocks/seven_segment_display_mock.hpp"
#include "Mocks/web_server_mock.hpp"

// Patch xQueueReceive to simulate correct event sequence for init_messages
#include "messages.hpp"
// ...existing code...
// All includes and macros are now ASCII and UTF-8 encoded
using ::testing::_;
using ::testing::Return;

typedef int (*xQueueSend_fn)(QueueHandle_t, const void *, unsigned int);
typedef int (*xQueueReceive_fn)(QueueHandle_t, void *, unsigned int);
extern "C"
{
    extern void (*vQueueDelete_ptr)(void *);
    extern int (*xQueueSendFromISR_ptr)(void *, const void *, int *);

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

    // xQueueCreate is defined globally above, do not redefine here
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

// MSVC weak symbol emulation: Use function pointers for queue stubs
QueueHandle_t queuefail_xQueueCreate(portBASE_TYPE, portBASE_TYPE) { return nullptr; }
int queuefail_xQueueSend(QueueHandle_t, const void *, unsigned int) { return 0; }
int queuefail_xQueueReceive(QueueHandle_t, void *, unsigned int) { return 0; }
void queuefail_vQueueDelete(void *) {}
int queuefail_xQueueSendFromISR(void *, const void *, int *) { return 0; }

// Patch xQueueSendFromISR for this test to always succeed
class XQueueSendFromISRRestorer
{
  public:
    int (*origPtr)(void *, const void *, int *);
    XQueueSendFromISRRestorer()
    {
        origPtr = xQueueSendFromISR_ptr;
        xQueueSendFromISR_ptr = [](void *, const void *, int *) { return 1; };
    }
    ~XQueueSendFromISRRestorer() { xQueueSendFromISR_ptr = origPtr; }
};

// Patch xQueueCreate for this test to always succeed
class XQueueCreateRestorer
{
  public:
    QueueHandle_t (*origPtr)(portBASE_TYPE, portBASE_TYPE);
    XQueueCreateRestorer()
    {
        origPtr = xQueueCreate_ptr;
        xQueueCreate_ptr = [](portBASE_TYPE, portBASE_TYPE) { return reinterpret_cast<QueueHandle_t>(0xDEADBEEF); };
    }
    ~XQueueCreateRestorer() { xQueueCreate_ptr = origPtr; }
};

// File-scope stub for xQueueReceive event sequence
struct ConfigAndPresetsResponseStub
{
    static int call_count;
    static int xQueueReceive(QueueHandle_t, void *event, unsigned int)
    {
        auto *msg = static_cast<Messages::Event *>(event);
        if (call_count == 0)
        {
            msg->type = Messages::EventType::CONFIGURATION_RESPONSE;
        }
        else
        {
            msg->type = Messages::EventType::PRESETS_RESPONSE;
        }
        ++call_count;
        return 1; // pdTRUE
    }
};
int ConfigAndPresetsResponseStub::call_count = 0;

// Global override for xQueueCreate to ensure all calls use the stub
extern "C" QueueHandle_t xQueueCreate(portBASE_TYPE a, portBASE_TYPE b)
{
    if (xQueueCreate_ptr)
        return xQueueCreate_ptr(a, b);
    return reinterpret_cast<QueueHandle_t>(static_cast<uintptr_t>(0xDEADBEEF)); // default stub, pointer-sized value
}
// Removed stray assignment to xQueueCreate_ptr

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
    MockSevenSegmentDisplay *mockDisplay;
    MockFootSwitch *mockFootSwitch;
    MockMax3485Sender *mockMax3485Sender;
    MockWebServer *mockWebServer;
    MockNvStorage *mockNvStorage;

    DmxControllerTest()
        : mockPresetChanger(new MockPresetChanger()), mockDisplay(new MockSevenSegmentDisplay()),
          mockFootSwitch(new MockFootSwitch()), mockMax3485Sender(new MockMax3485Sender()),
          mockWebServer(new MockWebServer()), mockNvStorage(new MockNvStorage())
    {
        testing::Mock::AllowLeak(mockPresetChanger);
        testing::Mock::AllowLeak(mockDisplay);
        testing::Mock::AllowLeak(mockFootSwitch);
        testing::Mock::AllowLeak(mockMax3485Sender);
        testing::Mock::AllowLeak(mockWebServer);
        testing::Mock::AllowLeak(mockNvStorage);
    }

    void SetUp() override { std::cout << "TEST_F SetUp()\n"; }
    void TearDown() override
    {
        std::cout << "TEST_F TearDown()\n";
        // Prevent double deletion by nulling out pointers after controller destruction
        mockPresetChanger = nullptr;
        mockDisplay = nullptr;
        mockFootSwitch = nullptr;
        mockMax3485Sender = nullptr;
        mockWebServer = nullptr;
        mockNvStorage = nullptr;
    }
};

// Test full DmxController::init (including message handling)
TEST_F(DmxControllerTest, Init_AllSuccess_ReturnsEspOk)
{
    XQueueCreateRestorer queueCreateStub;
    XQueueSendFromISRRestorer isrSendStub;
    std::cout << "TEST_F (init) is running!\n";
    QueueHandle_t dummyQueue = reinterpret_cast<QueueHandle_t>(0x1);
    // Test double for DmxController that does NOT override destructor or set member pointers to nullptr
    struct ControllerTestDouble : DmxController
    {
        QueueHandle_t testQueue;
        ControllerTestDouble(DmxPresetChanger *pc, SevenSegmentDisplay *sd, FootSwitch *fs, Max3485Sender *ms,
            WebServer *ws, NvStorage *ns, QueueHandle_t q)
            : DmxController(pc, sd, fs, ms, ws, ns), testQueue(q)
        {
        }
        QueueHandle_t getEventQueue() const override { return testQueue; }
    };

    ASSERT_NE(mockPresetChanger, nullptr);
    ASSERT_NE(mockDisplay, nullptr);
    ASSERT_NE(mockFootSwitch, nullptr);
    ASSERT_NE(mockMax3485Sender, nullptr);
    ASSERT_NE(mockWebServer, nullptr);
    ASSERT_NE(mockNvStorage, nullptr);

    ControllerTestDouble controllerWithQueue(
        mockPresetChanger, mockDisplay, mockFootSwitch, mockMax3485Sender, mockWebServer, mockNvStorage, dummyQueue);

    // Null checks before dereferencing mocks
    ASSERT_NE(mockPresetChanger, nullptr);
    ASSERT_NE(mockDisplay, nullptr);
    ASSERT_NE(mockFootSwitch, nullptr);
    ASSERT_NE(mockMax3485Sender, nullptr);
    ASSERT_NE(mockWebServer, nullptr);
    ASSERT_NE(mockNvStorage, nullptr);

    EXPECT_CALL(*mockPresetChanger, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockDisplay, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockFootSwitch, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockMax3485Sender, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockNvStorage, getEventQueue()).WillRepeatedly(Return(dummyQueue));

    EXPECT_CALL(*mockPresetChanger, init(_)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockDisplay, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockFootSwitch, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockMax3485Sender, init(_, _, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockWebServer, init()).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockNvStorage, init(_)).WillRepeatedly(Return(ESP_OK));

    // Patch xQueueReceive to simulate correct event sequence for init_messages
    xQueueReceive_ptr = &ConfigAndPresetsResponseStub::xQueueReceive;

    auto result = controllerWithQueue.DmxController::init();
    EXPECT_EQ(result, ESP_OK);

    // Restore default stubs after test
    xQueueSend_ptr = nullptr;
    xQueueReceive_ptr = nullptr;
}

// DmxControllerQueueFailTest merged from dmx_controller_queuefail_tests.cpp
class DmxControllerQueueFailTest : public ::testing::Test
{
  protected:
    MockPresetChanger *mockPresetChanger;
    MockSevenSegmentDisplay *mockDisplay;
    MockFootSwitch *mockFootSwitch;
    MockMax3485Sender *mockMax3485Sender;
    MockWebServer *mockWebServer;
    MockNvStorage *mockNvStorage;

    DmxControllerQueueFailTest()
        : mockPresetChanger(new MockPresetChanger()), mockDisplay(new MockSevenSegmentDisplay()),
          mockFootSwitch(new MockFootSwitch()), mockMax3485Sender(new MockMax3485Sender()),
          mockWebServer(new MockWebServer()), mockNvStorage(new MockNvStorage())
    {
        testing::Mock::AllowLeak(mockPresetChanger);
        testing::Mock::AllowLeak(mockDisplay);
        testing::Mock::AllowLeak(mockFootSwitch);
        testing::Mock::AllowLeak(mockMax3485Sender);
        testing::Mock::AllowLeak(mockWebServer);
        testing::Mock::AllowLeak(mockNvStorage);
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
        ControllerTestDouble(DmxPresetChanger *pc, SevenSegmentDisplay *sd, FootSwitch *fs, MockMax3485Sender *ms,
            WebServer *ws, NvStorage *ns, QueueHandle_t q)
            : DmxController(pc, sd, fs, ms, ws, ns), testQueue(q)
        {
        }
        QueueHandle_t getEventQueue() const override { return testQueue; }
    };

    QueueHandle_t dummyQueue = reinterpret_cast<QueueHandle_t>(0x1);
    ControllerTestDouble controllerWithQueue(
        mockPresetChanger, mockDisplay, mockFootSwitch, mockMax3485Sender, mockWebServer, mockNvStorage, dummyQueue);

    // Null checks before dereferencing mocks
    ASSERT_NE(mockPresetChanger, nullptr);
    ASSERT_NE(mockDisplay, nullptr);
    ASSERT_NE(mockFootSwitch, nullptr);
    ASSERT_NE(mockMax3485Sender, nullptr);
    ASSERT_NE(mockWebServer, nullptr);
    ASSERT_NE(mockNvStorage, nullptr);

    EXPECT_CALL(*mockPresetChanger, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockDisplay, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockFootSwitch, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockMax3485Sender, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockNvStorage, getEventQueue()).WillRepeatedly(Return(dummyQueue));

    EXPECT_CALL(*mockPresetChanger, init(_)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockDisplay, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockFootSwitch, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockMax3485Sender, init(_, _, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockWebServer, init()).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockNvStorage, init(_)).WillRepeatedly(Return(ESP_OK));

    esp_err_t ret = controllerWithQueue.init();
    EXPECT_EQ(ret, ESP_FAIL);

    // Restore default stubs after test
    xQueueCreate_ptr = nullptr;
    xQueueSend_ptr = nullptr;
    xQueueReceive_ptr = nullptr;
    vQueueDelete_ptr = nullptr;
    xQueueSendFromISR_ptr = nullptr;
}
