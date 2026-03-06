#include "../../main/Base/assert.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <stdio.h>

// Unit test for Assert::Halt override
class TestAssert : public Assert
{
  public:
    void Halt() override { throw std::runtime_error("Halt called"); }
};

TEST(AssertUnit, HaltThrowsInsteadOfHangs)
{
    TestAssert assert;
    EXPECT_THROW(assert.Halt(), std::runtime_error);
}

TEST(AssertTest, AssertNotEspError_Ok)
{
    TestAssert assert;
    EXPECT_NO_THROW(assert.assertNotEspError(ESP_OK, "should not throw"));
}

TEST(AssertTest, AssertNotEspError_Fail)
{
    TestAssert assert;
    EXPECT_THROW(assert.assertNotEspError(ESP_FAIL, "fail"), std::runtime_error);
}

TEST(AssertTest, AssertPdPass_Ok)
{
    TestAssert assert;
    EXPECT_NO_THROW(assert.assertPdPass(pdPASS, "should not throw"));
}

TEST(AssertTest, AssertPdPass_Fail)
{
    TestAssert assert;
    EXPECT_THROW(assert.assertPdPass(!pdPASS, "fail"), std::runtime_error);
}

TEST(AssertTest, AssertQueueHandle_Ok)
{
    TestAssert assert;
    void *dummy = reinterpret_cast<void *>(0x1);
    EXPECT_NO_THROW(assert.assertQueueHandle(dummy, "queue"));
}

TEST(AssertTest, AssertQueueHandle_Fail)
{
    TestAssert assert;
    EXPECT_THROW(assert.assertQueueHandle(nullptr, "queue"), std::runtime_error);
}

TEST(AssertTest, AssertNvsHandle_Ok)
{
    TestAssert assert;
    nvs_handle_t dummy = 1;
    EXPECT_NO_THROW(assert.assertNvsHandle(dummy, "nvs"));
}

TEST(AssertTest, AssertNvsHandle_Fail)
{
    TestAssert assert;
    nvs_handle_t dummy = 0;
    EXPECT_THROW(assert.assertNvsHandle(dummy, "nvs"), std::runtime_error);
}

TEST(AssertTest, AssertNotNull_Ok)
{
    TestAssert assert;
    int x = 5;
    EXPECT_NO_THROW(assert.assertNotNull(&x, "x"));
}

TEST(AssertTest, AssertNotNull_Fail)
{
    TestAssert assert;
    EXPECT_THROW(assert.assertNotNull(nullptr, "x"), std::runtime_error);
}

TEST(AssertTest, AssertNot0_Ok)
{
    TestAssert assert;
    EXPECT_NO_THROW(assert.assertNot0(1, "var"));
}

TEST(AssertTest, AssertNot0_Fail)
{
    TestAssert assert;
    EXPECT_THROW(assert.assertNot0(0, "var"), std::runtime_error);
}

TEST(AssertTest, AssertTrue_Ok)
{
    TestAssert assert;
    EXPECT_NO_THROW(assert.assertTrue(true, "flag"));
}

TEST(AssertTest, AssertTrue_Fail)
{
    TestAssert assert;
    EXPECT_THROW(assert.assertTrue(false, "flag"), std::runtime_error);
}

TEST(AssertTest, AssertSoftwareError_AlwaysThrows)
{
    TestAssert assert;
    EXPECT_THROW(assert.assertSoftwareError("err"), std::runtime_error);
}
