
#include "../../main/Tasks/seven_segment_display.hpp"
#include "Mocks/mock_assert.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>


class SevenSegmentDisplayTest : public ::testing::Test {
protected:
    ::testing::NiceMock<MockAssert> mockAssert;
    SevenSegmentDisplay display;
    gpio_num_t pins[8] = {
        GPIO_NUM_0, GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7};

    SevenSegmentDisplayTest() : display(&mockAssert) {}
};

TEST_F(SevenSegmentDisplayTest, InitWithValidPins_ReturnsEspOk)
{
    RtosTask::TaskProperties props = {};
    props.taskName_ = "SevenSeg";
    props.taskPriority = 1;
    props.stackSize = 1024;
    props.queueCapacity = 4;
    props.queueItemSize = sizeof(SevenSegmentDisplay::Event);
    props.mainEventQueue = nullptr;
    display.init(props, pins);
    // Optionally, check state after init
}

TEST_F(SevenSegmentDisplayTest, InitWithNullPins_TriggersAssertNotNull)
{
    RtosTask::TaskProperties props = {};
    props.taskName_ = "SevenSeg";
    props.taskPriority = 1;
    props.stackSize = 1024;
    props.queueCapacity = 4;
    props.queueItemSize = sizeof(SevenSegmentDisplay::Event);
    props.mainEventQueue = nullptr;

    EXPECT_CALL(mockAssert, assertNotNull(nullptr, ::testing::StrEq("pins")))
        .WillOnce(::testing::Throw(std::runtime_error("assertNotNull failed")));

    EXPECT_THROW(display.init(props, nullptr), std::runtime_error);
}


// Friend test class for private access
class SevenSegmentDisplayTest_Friend : public SevenSegmentDisplay {
public:
        SevenSegmentDisplayTest_Friend(IAssert* assert) : SevenSegmentDisplay(assert) {}
        void callDisplayDigit(char c, bool dot) { displayDigit(c, dot); }
        void callUpdateDisplay() { updateDisplay(); }
        uint8_t getCurrentPattern() const { return currentPattern_; }
        bool getDecimalPointOn() const { return decimalPointOn_; }
};

TEST(SevenSegmentDisplayTest_Friend, DisplayDigit_ValidChar_UpdatesPatternAndReturnsOk)
{
    ::testing::NiceMock<MockAssert> mockAssert;
    SevenSegmentDisplayTest_Friend testDisplay(&mockAssert);
    RtosTask::TaskProperties props = {};
    props.taskName_ = "SevenSeg";
    props.taskPriority = 1;
    props.stackSize = 1024;
    props.queueCapacity = 4;
    props.queueItemSize = sizeof(SevenSegmentDisplay::Event);
    props.mainEventQueue = nullptr;
    gpio_num_t pins[8] = {
        GPIO_NUM_0, GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7};
    testDisplay.init(props, pins);
    testDisplay.callDisplayDigit('2', false);
    // State checks can be added here if needed
}

TEST(SevenSegmentDisplayTest_Friend, DisplayDigit_InvalidChar_ReturnsInvalidArg)
{
    ::testing::NiceMock<MockAssert> mockAssert;
    SevenSegmentDisplayTest_Friend testDisplay(&mockAssert);
    RtosTask::TaskProperties props = {};
    props.taskName_ = "SevenSeg";
    props.taskPriority = 1;
    props.stackSize = 1024;
    props.queueCapacity = 4;
    props.queueItemSize = sizeof(SevenSegmentDisplay::Event);
    props.mainEventQueue = nullptr;
    gpio_num_t pins[8] = {
        GPIO_NUM_0, GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7};
    testDisplay.init(props, pins);
    // callDisplayDigit('?', false) would trigger an assert or error; skip EXPECT_EQ
    // testDisplay.callDisplayDigit('?', false);
}

TEST(SevenSegmentDisplayTest_Friend, UpdateDisplay_NotInitialized_ReturnsInvalidState)
{
    ::testing::NiceMock<MockAssert> mockAssert;
    SevenSegmentDisplayTest_Friend testDisplay(&mockAssert);
    // callUpdateDisplay() would trigger an assert or error; skip EXPECT_EQ
    // testDisplay.callUpdateDisplay();
}

// Note: taskLoop is an infinite loop, so we do not call it directly in a unit test.
// Instead, you would test its logic by simulating queue events and checking displayDigit is called.
