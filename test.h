#ifndef TEST_H
#define TEST_H

#include <string>
#include <cassert>

#include "my_optional.h"

void test()
{
    static_assert(std::is_same_v<my_optional<std::string>::value_type, std::string>);
    static_assert(!std::is_same_v<my_optional<int>::value_type, std::string>);
    static_assert(std::is_same_v<my_optional<int>::value_type, int>);

    my_optional<std::string> testStr{};
    assert(!testStr.has_value());
    assert(!static_cast<bool>(testStr));

    testStr.emplace(5, 'a');
    assert(testStr.has_value());
    assert(static_cast<bool>(test));
    assert(testStr.value() == std::string(5, 'a'));

    assert(testStr->size() == 5);
    assert(*testStr == testStr.value());

    const my_optional<std::string> constTestStr = testStr;
    assert(constTestStr.has_value());
    assert(constTestStr == testStr);
    assert(constTestStr.value() == testStr.value());
    assert(constTestStr->size() == testStr->size());

    testStr.reset();
    assert(!testStr.has_value());

    my_optional<std::string> anotherTestStr(std::in_place, 2, 'b');

    assert(!(testStr == anotherTestStr));
    assert(testStr != anotherTestStr);
    assert(testStr < anotherTestStr);
    assert(testStr <= anotherTestStr);
    assert(!(testStr >= anotherTestStr));
    assert(!(testStr > anotherTestStr));
    assert(my_optional<std::string>("str") == my_optional<std::string>("str"));

    testStr.swap(anotherTestStr);
    assert(testStr.has_value());
    assert(!anotherTestStr.has_value());

    anotherTestStr.swap(testStr);
    assert(!testStr.has_value());
    assert(anotherTestStr.has_value());

    testStr = std::move(anotherTestStr);
    assert(testStr.has_value());
    assert(!anotherTestStr.has_value());

    testStr.reset();
    assert(testStr == anotherTestStr);
    assert(testStr <= anotherTestStr);
    assert(testStr >= anotherTestStr);
    assert(!(testStr < anotherTestStr));
    assert(!(testStr > anotherTestStr));

    testStr = std::string("new str");
    assert(testStr == std::string("new str"));
    testStr = anotherTestStr;
    assert(!testStr.has_value());

    my_optional<int> testInt(5);
    my_optional<int> anotherTestInt(6);

    assert(testInt < anotherTestInt);
    assert(testInt != anotherTestInt);
    assert(testInt != my_optional<int>{});

    testInt.swap(anotherTestInt);
    assert(testInt > anotherTestInt);

    testInt.emplace(-3);
    anotherTestInt = 1;
    assert(testInt < anotherTestInt);

    assert(testInt.value_or(10) == -3);
    testInt.reset();
    assert(testInt.value_or(10) == 10);
    assert(!testInt.has_value());
    const my_optional<int> constTestInt = testInt;
    assert(constTestInt.value_or(10) == 10);

    assert(!testInt.and_then([](int val) { return val * 10; }).has_value());
    testInt = 5;
    assert(testInt.and_then([](int val) { return val * 10; }) == 50);

    testInt.emplace(1);
    testInt.emplace(2);
    testInt.emplace(-5);
    assert(*testInt == -5);

    anotherTestInt.reset();
    testInt.reset();
    testInt.reset();
    assert(!testInt.has_value());

    testStr.reset();
    testStr.reset();
    testStr.reset();
    assert(!testStr.has_value());

    bool throwError = false;
    try
    {
        testInt.value();
    }
    catch (my_bad_optional_access&)
    {
        throwError = true;
    }
    assert(throwError);

    throwError = false;
    try
    {
        *testInt;
    }
    catch (my_bad_optional_access&)
    {
        throwError = true;
    }
    assert(throwError);


    throwError = false;
    try
    {
        testStr->clear();
    }
    catch (my_bad_optional_access&)
    {
        throwError = true;
    }
    assert(throwError);

    throwError = false;
    try
    {
        testInt.swap(anotherTestInt);
    }
    catch (my_bad_optional_access&)
    {
        throwError = true;
    }
    assert(!throwError);

    assert(testInt == anotherTestInt);
    assert(!testInt.has_value());
}

#endif
