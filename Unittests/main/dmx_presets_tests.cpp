
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

TEST(DmxPresetsTest, SetPreset_CopiesPresetData)
{
    MockAssert mockAssert;
    DmxPresets presets(&mockAssert);
    uint8_t values1[NR_OF_DMX_CHANNELS] = {0};
    uint8_t values2[NR_OF_DMX_CHANNELS] = {0};
    values1[0] = 42;
    values2[0] = 99;
    presets.addPreset(0, "Original", values1);
    presets.setNumberOfFilledPresets(1);
    DmxPreset newPreset(&mockAssert);
    newPreset.setIndex(0);
    newPreset.setName("Updated");
    newPreset.setDmxValues(values2);
    presets.setPreset(0, newPreset);
    EXPECT_STREQ(presets.getPreset(0).getName(), "Updated");
    EXPECT_EQ(presets.getPreset(0).getDmxValue(0), 99);
}