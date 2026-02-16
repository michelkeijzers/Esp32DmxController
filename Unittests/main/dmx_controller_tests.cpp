
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../main/artnet_sender.hpp"

#include "../../main/dmx_controller.hpp"
#include "../../main/dmx_preset_changer.hpp"
#include "../../main/foot_switch.hpp"
#include "../../main/nvs_storage.hpp"
#include "../../main/osc_sender.hpp"
#include "../../main/seven_segment_display.hpp"
#include "../../main/web_server.hpp"

#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/queue.h"

// #include "dmx_controller.hpp"
// #include "messages.hpp"

struct FileLoadedNotifier
{
    FileLoadedNotifier() { std::cout << "dmx_controller_tests.cpp is compiled and linked\n"; }
};
FileLoadedNotifier fileLoadedNotifierInstance;

// Sanity test to verify test registration
TEST(SanityCheck, AlwaysPasses) { EXPECT_EQ(1, 1); }

using ::testing::_;
using ::testing::Return;

// Mock classes for all DmxController dependencies
class MockPresetChanger : public DmxPresetChanger
{
  public:
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t));
    MOCK_METHOD(QueueHandle_t, getEventQueue, (), ());
};
class MockOSCSender : public OSCSender
{
  public:
    MOCK_METHOD(esp_err_t, init, (const char *, uint16_t));
};
class MockSevenSegmentDisplay : public SevenSegmentDisplay
{
  public:
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t, const gpio_num_t *));
    MOCK_METHOD(QueueHandle_t, getEventQueue, (), ());
};
class MockFootSwitch : public FootSwitch
{
  public:
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t, gpio_num_t));
    MOCK_METHOD(QueueHandle_t, getEventQueue, (), ());
};
class MockArtNetSender : public ArtNetSender
{
  public:
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t, const char *, uint16_t));
    MOCK_METHOD(QueueHandle_t, getEventQueue, (), ());
};
class MockWebServer : public WebServer
{
  public:
    MOCK_METHOD(esp_err_t, init, ());
};
class MockNvsStorage : public NvsStorage
{
  public:
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t));
    MOCK_METHOD(QueueHandle_t, getEventQueue, (), ());
};

// Patch xQueueReceive for this test file to simulate correct event types for init_messages
#include "messages.hpp"
extern "C"
{
    // Save original pointer for restoration if needed
    static int xQueueReceive_call_count = 0;
    static int xQueueReceive_last_result = 1;
    static int xQueueReceive(QueueHandle_t, void *pvBuffer, unsigned int)
    {
        using MessagesEvent = Messages::Event;
        xQueueReceive_call_count++;
        if (!pvBuffer)
            return 0;
        MessagesEvent *event = static_cast<MessagesEvent *>(pvBuffer);
        // Simulate the sequence in init_messages:
        // 1st call: config response, 2nd call: presets response
        if (xQueueReceive_call_count == 1)
        {
            event->type = Messages::EventType::CONFIGURATION_RESPONSE;
        }
        else if (xQueueReceive_call_count == 2)
        {
            event->type = Messages::EventType::PRESETS_RESPONSE;
        }
        return 1; // pdTRUE
    }
}

// Test fixture for DmxController
class DmxControllerTest : public ::testing::Test
{
  protected:
    MockPresetChanger *mockPresetChanger;
    MockOSCSender *mockOscSender;
    MockSevenSegmentDisplay *mockDisplay;
    MockFootSwitch *mockFootSwitch;
    MockArtNetSender *mockArtNetSender;
    MockWebServer *mockWebServer;
    MockNvsStorage *mockNvsStorage;
    DmxController controller;

    DmxControllerTest()
        : mockPresetChanger(new MockPresetChanger()), mockOscSender(new MockOSCSender()),
          mockDisplay(new MockSevenSegmentDisplay()), mockFootSwitch(new MockFootSwitch()),
          mockArtNetSender(new MockArtNetSender()), mockWebServer(new MockWebServer()),
          mockNvsStorage(new MockNvsStorage()), controller(mockPresetChanger, mockOscSender, mockDisplay,
                                                    mockFootSwitch, mockArtNetSender, mockWebServer, mockNvsStorage)
    {
        testing::Mock::AllowLeak(mockPresetChanger);
        testing::Mock::AllowLeak(mockOscSender);
        testing::Mock::AllowLeak(mockDisplay);
        testing::Mock::AllowLeak(mockFootSwitch);
        testing::Mock::AllowLeak(mockArtNetSender);
        testing::Mock::AllowLeak(mockWebServer);
        testing::Mock::AllowLeak(mockNvsStorage);
        xQueueReceive_call_count = 0;
    }

    void SetUp() override
    {
        std::cout << "TEST_F SetUp()\n";
        xQueueReceive_call_count = 0;
    }
    void TearDown() override
    {
        std::cout << "TEST_F TearDown()\n";
        // Do not delete mocks here; DmxController will delete them
        xQueueReceive_call_count = 0;
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
            ArtNetSender *an, WebServer *ws, NvsStorage *ns, QueueHandle_t q)
            : DmxController(pc, os, sd, fs, an, ws, ns), testQueue(q)
        {
        }
        QueueHandle_t getEventQueue() const override { return testQueue; }
        ~ControllerTestDouble() override
        {
            presetChanger_ = nullptr;
            oscSender_ = nullptr;
            display_ = nullptr;
            footSwitch_ = nullptr;
            artnetSender_ = nullptr;
            webServer_ = nullptr;
            nvsStorage_ = nullptr;
        }
    };

    ASSERT_NE(mockPresetChanger, nullptr);
    ASSERT_NE(mockOscSender, nullptr);
    ASSERT_NE(mockDisplay, nullptr);
    ASSERT_NE(mockFootSwitch, nullptr);
    ASSERT_NE(mockArtNetSender, nullptr);
    ASSERT_NE(mockWebServer, nullptr);
    ASSERT_NE(mockNvsStorage, nullptr);

    ControllerTestDouble controllerWithQueue(mockPresetChanger, mockOscSender, mockDisplay, mockFootSwitch,
        mockArtNetSender, mockWebServer, mockNvsStorage, dummyQueue);

    EXPECT_CALL(*mockPresetChanger, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockDisplay, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockFootSwitch, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockArtNetSender, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockNvsStorage, getEventQueue()).WillRepeatedly(Return(dummyQueue));

    EXPECT_CALL(*mockPresetChanger, init(_)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockOscSender, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockDisplay, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockFootSwitch, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockArtNetSender, init(_, _, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockWebServer, init()).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockNvsStorage, init(_)).WillRepeatedly(Return(ESP_OK));

    // The queue stubs in esp_idf_stubs/freertos/queue.h always return success, so init_messages will succeed
    esp_err_t ret = controllerWithQueue.init();
    printf("Init_AllSuccess_ReturnsEspOk: ret = %d\n", ret);
    printf("ESP_OK = %d\n", ESP_OK);
    EXPECT_EQ(ret, ESP_OK);
}

TEST_F(DmxControllerTest, InitSubTasks_AllSuccess_ReturnsEspOk)
{
    std::cout << "TEST_F is running!\n";
    // Set up getEventQueue() for all mocks that use it

    QueueHandle_t dummyQueue = reinterpret_cast<QueueHandle_t>(0x1);
    // Use a test double or lambda to override controller.getEventQueue()
    struct ControllerTestDouble : DmxController
    {
        QueueHandle_t testQueue;
        ControllerTestDouble(DmxPresetChanger *pc, OSCSender *os, SevenSegmentDisplay *sd, FootSwitch *fs,
            ArtNetSender *an, WebServer *ws, NvsStorage *ns, QueueHandle_t q)
            : DmxController(pc, os, sd, fs, an, ws, ns), testQueue(q)
        {
        }
        QueueHandle_t getEventQueue() const override { return testQueue; }
        ~ControllerTestDouble() override
        {
            // Prevent base destructor from deleting mocks (owned by test)
            presetChanger_ = nullptr;
            oscSender_ = nullptr;
            display_ = nullptr;
            footSwitch_ = nullptr;
            artnetSender_ = nullptr;
            webServer_ = nullptr;
            nvsStorage_ = nullptr;
        }
    };

    // Ensure all mocks are non-null and used
    ASSERT_NE(mockPresetChanger, nullptr);
    ASSERT_NE(mockOscSender, nullptr);
    ASSERT_NE(mockDisplay, nullptr);
    ASSERT_NE(mockFootSwitch, nullptr);
    ASSERT_NE(mockArtNetSender, nullptr);
    ASSERT_NE(mockWebServer, nullptr);
    ASSERT_NE(mockNvsStorage, nullptr);

    ControllerTestDouble controllerWithQueue(mockPresetChanger, mockOscSender, mockDisplay, mockFootSwitch,
        mockArtNetSender, mockWebServer, mockNvsStorage, dummyQueue);

    EXPECT_CALL(*mockPresetChanger, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockDisplay, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockFootSwitch, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockArtNetSender, getEventQueue()).WillRepeatedly(Return(dummyQueue));
    EXPECT_CALL(*mockNvsStorage, getEventQueue()).WillRepeatedly(Return(dummyQueue));

    // Set expectations on the mocks (allow any order, always return ESP_OK)
    EXPECT_CALL(*mockPresetChanger, init(_)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockOscSender, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockDisplay, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockFootSwitch, init(_, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockArtNetSender, init(_, _, _)).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockWebServer, init()).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(*mockNvsStorage, init(_)).WillRepeatedly(Return(ESP_OK));

    // Use the test double controller instance, but only test sub-task initialization
    esp_err_t ret = controllerWithQueue.init_sub_tasks();
    EXPECT_EQ(ret, ESP_OK);
}

// Add more tests for error cases, message handling, etc.
