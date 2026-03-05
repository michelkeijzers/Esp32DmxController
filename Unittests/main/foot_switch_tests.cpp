#include "../../main/Tasks/foot_switch.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::NiceMock;
using ::testing::Return;

// No need to stub gpio_get_level here; already stubbed in esp_idf_stubs/driver/gpio.h

static Configuration _unit_test_dummy_configuration_foot_switch_test;
class FootSwitchTest : public ::testing::Test
{
  protected:
    FootSwitch footSwitch{_unit_test_dummy_configuration_foot_switch_test};
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FootSwitchTest, DefaultConstructor_InitializesMembers)
{
    EXPECT_EQ(footSwitch.getPin(), GPIO_NUM_NC);
    // No isPressed() method available
}

// Add more tests as needed, e.g., for event handling, long press, etc.
