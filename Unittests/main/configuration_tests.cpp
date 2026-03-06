#include <gtest/gtest.h>
#include "../../main/Data/configuration.hpp"

// Use MockAssert for DI
#include "Mocks/mock_assert.hpp"

TEST(ConfigurationTest, DefaultValues)
{
    MockAssert mockAssert;
    Configuration config(&mockAssert);
    EXPECT_FALSE(config.getFootSwitchPolarityNormallyOpen());
    EXPECT_EQ(config.getFootSwitchLongPressTime(), 0);
    EXPECT_FALSE(config.getCircularPresetNavigation());
    EXPECT_EQ(config.getNumberOfFilledPresets(), 0);
}

TEST(ConfigurationTest, SettersAndGetters)
{
    MockAssert mockAssert;
    Configuration config(&mockAssert);
    config.setFootSwitchPolarityNormallyOpen(true);
    EXPECT_TRUE(config.getFootSwitchPolarityNormallyOpen());
    config.setFootSwitchLongPressTime(1234);
    EXPECT_EQ(config.getFootSwitchLongPressTime(), 1234);
    config.setCircularPresetNavigation(true);
    EXPECT_TRUE(config.getCircularPresetNavigation());
    config.setNumberOfFilledPresets(5);
    EXPECT_EQ(config.getNumberOfFilledPresets(), 5);
}
