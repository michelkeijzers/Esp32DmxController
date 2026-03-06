#include "../../main/Data/dmx_presets.hpp"
#include <gtest/gtest.h>

// Use MockAssert for DI
#include "Mocks/mock_assert.hpp"

TEST(DmxPresetsTest, DefaultValues)
{
    MockAssert mockAssert;
    DmxPresets presets(&mockAssert);
    EXPECT_EQ(presets.getNumberOfFilledPresets(), 0);
    EXPECT_EQ(presets.getCurrentPresetIndex(), 0);
}

TEST(DmxPresetsTest, AddAndGetPreset)
{
    MockAssert mockAssert;
    DmxPresets presets(&mockAssert);
    uint8_t values[NR_OF_DMX_CHANNELS] = {0};
    values[0] = 55;
    presets.addPreset(0, "Test", values);
    EXPECT_EQ(presets.getNumberOfFilledPresets(), 1);
    EXPECT_STREQ(presets.getPreset(0).getName(), "Test");
    EXPECT_EQ(presets.getPreset(0).getDmxValue(0), 55);
}

TEST(DmxPresetsTest, SelectNextAndPreviousPreset)
{
    MockAssert mockAssert;
    DmxPresets presets(&mockAssert);
    uint8_t values[NR_OF_DMX_CHANNELS] = {0};
    presets.addPreset(0, "A", values);
    presets.addPreset(1, "B", values);
    presets.setCurrentPresetIndex(0);
    EXPECT_EQ(presets.selectNextPreset(), 1);
    EXPECT_EQ(presets.selectPreviousPreset(), 0);
}
