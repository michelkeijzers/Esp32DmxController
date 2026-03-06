#include <gtest/gtest.h>
#include "../../main/Data/dmx_preset.hpp"

// Use MockAssert for DI
#include "Mocks/mock_assert.hpp"

TEST(DmxPresetTest, DefaultValues)
{
    MockAssert mockAssert;
    DmxPreset preset(&mockAssert);
    EXPECT_EQ(preset.getIndex(), 0);
    EXPECT_STREQ(preset.getName(), "");
    EXPECT_FALSE(preset.isInitialized());
    for (int i = 0; i < NR_OF_DMX_CHANNELS; ++i) {
        EXPECT_EQ(preset.getDmxValue(i), 0);
    }
}

TEST(DmxPresetTest, SettersAndGetters)
{
    MockAssert mockAssert;
    DmxPreset preset(&mockAssert);
    preset.setIndex(42);
    EXPECT_EQ(preset.getIndex(), 42);
    preset.setName("TestPreset");
    EXPECT_STREQ(preset.getName(), "TestPreset");
    EXPECT_TRUE(preset.isInitialized());
    preset.setDmxValue(10, 123);
    EXPECT_EQ(preset.getDmxValue(10), 123);
    uint8_t values[NR_OF_DMX_CHANNELS] = {0};
    values[5] = 77;
    preset.setDmxValues(values);
    EXPECT_EQ(preset.getDmxValue(5), 77);
}

TEST(DmxPresetTest, ClearAndCopy)
{
    MockAssert mockAssert;
    DmxPreset preset1(&mockAssert);
    preset1.setIndex(1);
    preset1.setName("A");
    preset1.setDmxValue(0, 99);
    DmxPreset preset2(&mockAssert);
    preset2.copyFrom(preset1);
    EXPECT_EQ(preset2.getIndex(), 1);
    EXPECT_STREQ(preset2.getName(), "A");
    EXPECT_EQ(preset2.getDmxValue(0), 99);
    preset2.clear();
    EXPECT_EQ(preset2.getIndex(), 0);
    EXPECT_STREQ(preset2.getName(), "");
    EXPECT_EQ(preset2.getDmxValue(0), 0);
}
