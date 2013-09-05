#include <cstdio>
#include <cstdint>
#include "Value.hpp"

using tagptr::Value;

void test_int32(Value x, int64_t y)
{
    printf("%lld\n", x.asInt64());
    assert(x.asInt64() == y);
}

int main(int argc, char** argv)
{
    Value x = Value(12345678900);
    printf("%lld\n", x.asInt64());
    x = Value((void*) &main);
    printf("%p\n", x.asPtr());
    assert(&main == x.asPtr());
    x = Value(3.14);
    printf("%lf\n", x.asDouble());
    assert(x.asDouble() == 3.14);

    test_int32(Value(INT64_MIN), INT64_MIN);

    return 0;
}
