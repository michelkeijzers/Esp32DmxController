#include "../../main/Tasks/web_server.hpp"
#include "Mocks/web_server_mock.hpp"

#include "../ThirdParty/nlohmann/json.hpp"
using nlohmann::json;
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Return;

namespace
{

#include "../../main/Data/dmx_preset.hpp"
#include "../../main/Data/dmx_presets.hpp"

class WebServerApiTest : public ::testing::Test
{
  protected:
    DmxPresets presets;
    std::unique_ptr<MockWebServer> server;
    void SetUp() override
    {
        presets.clearAll();
        server = std::make_unique<MockWebServer>(&presets);
        presets.setNumberOfFilledPresets(2);
        uint8_t vals0[NR_OF_DMX_CHANNELS] = {0, 1, 2};
        uint8_t vals1[NR_OF_DMX_CHANNELS] = {3, 4, 5};
        presets.addPreset(0, "Preset 1", vals0);
        presets.addPreset(1, "Preset 2", vals1);
    }
};

TEST_F(WebServerApiTest, AllDataGetResponseFormat)
{
    // Simulate the /all_data GET response
    std::string json = server->presets_to_json();
    nlohmann::json root = nlohmann::json::parse(json);
    ASSERT_TRUE(root.is_array());
    size_t num_presets = root.size();
    EXPECT_GE(num_presets, 2u);
    EXPECT_LE(num_presets, 20u);
    for (nlohmann::json::const_reference preset : root)
    {
        ASSERT_TRUE(preset.is_object());
        ASSERT_TRUE(preset.contains("index") && preset["index"].is_number());
        ASSERT_TRUE(preset.contains("name") && preset["name"].is_string());
        ASSERT_TRUE(preset.contains("dmxValues") && preset["dmxValues"].is_array());
    }
}

TEST_F(WebServerApiTest, AllDataPostRequestValidation)
{
    // Simulate a valid POST body
    const char *post_json = "{"
                            "\"configuration\": { \"footSwitchPolarity\": \"NORMAL\" },"
                            "\"presets\": ["
                            "{ \"index\": 0, \"name\": \"Preset 1\", \"dmxValues\": [0,1,2] },"
                            "{ \"index\": 1, \"name\": \"Preset 2\", \"dmxValues\": [3,4,5] }"
                            "]"
                            "}";
    json root = json::parse(post_json);
    ASSERT_TRUE(root.is_object());
    ASSERT_TRUE(root.contains("configuration") && root["configuration"].is_object());
    ASSERT_TRUE(root.contains("presets") && root["presets"].is_array());
    int num_presets = root["presets"].size();
    EXPECT_EQ(num_presets, 2);
    for (const auto &preset : root["presets"])
    {
        ASSERT_TRUE(preset.is_object());
        ASSERT_TRUE(preset.contains("index") && preset["index"].is_number());
        ASSERT_TRUE(preset.contains("name") && preset["name"].is_string());
        ASSERT_TRUE(preset.contains("dmxValues") && preset["dmxValues"].is_array());
    }
}

} // namespace
