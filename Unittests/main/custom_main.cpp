
#include <gmock/gmock.h>
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    ::testing::GMOCK_FLAG(verbose) = "error";
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
