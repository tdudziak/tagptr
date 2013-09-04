#include <cstdio>
#include "Value.hpp"

int main(int argc, char** argv)
{
    Value x(5);
    printf("%d\n", x.asInt());
    x = Value((void*) &main);
    printf("%p\n", x.asPtr());
    return 0;
}
