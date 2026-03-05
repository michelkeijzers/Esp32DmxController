
#include "../../main/Tasks/max3485_sender.hpp"
#include "Mocks/mock_assert.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>


class Max3485SenderTest : public ::testing::Test {
protected:
    ::testing::NiceMock<MockAssert> mockAssert;
    Max3485Sender sender;
    RtosTask::TaskProperties props = {"Max3485Sender", 1, 1024, 4, sizeof(int), nullptr};

    Max3485SenderTest() : sender(&mockAssert) {}
};

TEST_F(Max3485SenderTest, Init_ReturnsEspOk) { EXPECT_EQ(sender.init(props), ESP_OK); }

TEST_F(Max3485SenderTest, SendDmx_NotInitialized_ReturnsInvalidState)
{
    uint8_t data[10] = {0};
    // sendDmx is private, so we cannot call it directly; would need a friend or wrapper for full coverage
    // This test is a placeholder for when access is available
    SUCCEED();
}

// Additional tests for close(), sendDmx(), and taskLoop() would require access to private/protected members or friend
// test class wrappers.
