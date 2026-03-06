#include "../../main/Base/lockable.hpp"
#include <freertos/semphr.h>
#include <gtest/gtest.h>

// Stub for FreeRTOS semaphore functions

class LockableTestClass : public Lockable
{
  public:
    void testLock() { lock(); }
    void testUnlock() { unlock(); }
    bool testTryLock(TickType_t timeout = 0) { return tryLock(timeout); }
};

TEST(LockableTest, LockUnlock)
{
    LockableTestClass obj;
    EXPECT_NO_THROW(obj.testLock());
    EXPECT_NO_THROW(obj.testUnlock());
}

TEST(LockableTest, TryLock)
{
    LockableTestClass obj;
    EXPECT_TRUE(obj.testTryLock());
}
