#include <cstdio>
#include "Value.hpp"

int main(int argc, char** argv)
{
    Value x(12345678900);
    printf("%lld\n", x.asInt64());
    Value y((void*) &main);
    printf("%p\n", y.asPtr());
    assert(&main == y.asPtr());
    Value z(3.14);
    printf("%lf\n", z.asDouble());
    assert(z.asDouble() == 3.14);
    Value t(INT32_MAX);
    assert(t.asInt32() == INT32_MAX);
    printf("%d\n", t.asInt32());
    return 0;
}
