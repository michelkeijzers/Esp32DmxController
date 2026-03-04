#include "../../main/Tasks/seven_segment_display.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class SevenSegmentDisplayTest : public ::testing::Test
{
  protected:
    SevenSegmentDisplay display;
    gpio_num_t pins[8] = {
        GPIO_NUM_0, GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7};
};

TEST_F(SevenSegmentDisplayTest, InitWithValidPins_ReturnsEspOk)
{
    RtosTask::TaskProperties props = {};
    props.taskName_ = "SevenSeg";
    props.logTag = "SevenSeg";
    props.taskPriority = 1;
    props.stackSize = 1024;
    props.queueCapacity = 4;
    props.queueItemSize = sizeof(SevenSegmentDisplay::Event);
    props.mainEventQueue = nullptr;
    EXPECT_EQ(display.init(props, pins), ESP_OK);
}

TEST_F(SevenSegmentDisplayTest, InitWithNullPins_ReturnsInvalidArg)
{
    RtosTask::TaskProperties props = {};
    props.taskName_ = "SevenSeg";
    props.logTag = "SevenSeg";
    props.taskPriority = 1;
    props.stackSize = 1024;
    props.queueCapacity = 4;
    props.queueItemSize = sizeof(SevenSegmentDisplay::Event);
    props.mainEventQueue = nullptr;
    EXPECT_EQ(display.init(props, nullptr), ESP_ERR_INVALID_ARG);
}

// Friend test class for private access
class SevenSegmentDisplayTest_Friend : public SevenSegmentDisplay
{
  public:
    esp_err_t callDisplayDigit(char c, bool dot) { return displayDigit(c, dot); }
    esp_err_t callUpdateDisplay() { return updateDisplay(); }
    uint8_t getCurrentPattern() const { return currentPattern_; }
    bool getDecimalPointOn() const { return decimalPointOn_; }
};

TEST(SevenSegmentDisplayTest_Friend, DisplayDigit_ValidChar_UpdatesPatternAndReturnsOk)
{
    SevenSegmentDisplayTest_Friend testDisplay;
    RtosTask::TaskProperties props = {};
    props.taskName_ = "SevenSeg";
    props.logTag = "SevenSeg";
    props.taskPriority = 1;
    props.stackSize = 1024;
    props.queueCapacity = 4;
    props.queueItemSize = sizeof(SevenSegmentDisplay::Event);
    props.mainEventQueue = nullptr;
    gpio_num_t pins[8] = {
        GPIO_NUM_0, GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7};
    ASSERT_EQ(testDisplay.init(props, pins), ESP_OK);
    EXPECT_EQ(testDisplay.callDisplayDigit('2', false), ESP_OK);
    EXPECT_EQ(testDisplay.getCurrentPattern(), 0b01011011);
    EXPECT_FALSE(testDisplay.getDecimalPointOn());
}

TEST(SevenSegmentDisplayTest_Friend, DisplayDigit_InvalidChar_ReturnsInvalidArg)
{
    SevenSegmentDisplayTest_Friend testDisplay;
    RtosTask::TaskProperties props = {};
    props.taskName_ = "SevenSeg";
    props.logTag = "SevenSeg";
    props.taskPriority = 1;
    props.stackSize = 1024;
    props.queueCapacity = 4;
    props.queueItemSize = sizeof(SevenSegmentDisplay::Event);
    props.mainEventQueue = nullptr;
    gpio_num_t pins[8] = {
        GPIO_NUM_0, GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7};
    ASSERT_EQ(testDisplay.init(props, pins), ESP_OK);
    EXPECT_EQ(testDisplay.callDisplayDigit('?', false), ESP_ERR_INVALID_ARG);
}

TEST(SevenSegmentDisplayTest_Friend, UpdateDisplay_NotInitialized_ReturnsInvalidState)
{
    SevenSegmentDisplayTest_Friend testDisplay;
    EXPECT_EQ(testDisplay.callUpdateDisplay(), ESP_ERR_INVALID_STATE);
}

// Note: taskLoop is an infinite loop, so we do not call it directly in a unit test.
// Instead, you would test its logic by simulating queue events and checking displayDigit is called.
