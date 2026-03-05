#include "../../main/Base/assert.hpp"
#include <stdio.h>
#include <stdlib.h>

void Assert::assertNotEspError(int v, const char *err)
{
    if (v != ESP_OK)
    {
        printf("ESP error: code %d, message: %s\n", v, err);
        Assert::Halt();
    }
}

void Assert::assertPdPass(int v, const char *err)
{
    if (v != pdPASS)
    {
        printf("pdPASS assertion failed: code %d\n", v);
        Assert::Halt();
    }
}

void Assert::assertNotNull(const void *v, const char *err)
{
    if (v == nullptr)
    {
        printf("Assertion failed: pointer is null, message: %s\n", err);
        Assert::Halt();
    }
}

void Assert::assertTrue(bool v, const char *err)
{
    if (!v)
    {
        printf("Assertion failed: condition is false, message: %s\n", err);
        Assert::Halt();
    }
}

void Assert::assertSoftwareError(const char *err)
{
    printf("Software error: %s\n", err);
    Assert::Halt();
}

void Assert::assertQueueHandle(void *v, const char *err)
{
    if (v == nullptr)
    {
        printf("Assertion failed: queue handle is null, message: %s\n", err);
        Assert::Halt();
    }
}

void Assert::assertNvsHandle(int v, const char *err)
{
    if (v != ESP_OK)
    {
        printf("Assertion failed: NVS handle error, code: %d, message: %s\n", v, err);
        Assert::Halt();
    }
}

void Assert::assertNot0(int v, const char *err)
{
    if (v == 0)
    {
        printf("Assertion failed: value is 0, message: %s\n", err);
        Assert::Halt();
    }
}

void Assert::Halt()
{
    printf("Halting execution.\n");
    while (true)
    {
        // Infinite loop to halt execution
    }
}