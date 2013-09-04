#ifndef VALUE_HPP
#define VALUE_HPP

#include <cstdint>
#include <cassert>

class Value {
private:
    static const uint32_t MASK = 0x3;
    static const uint32_t TAG_PTR = 0x0;
    static const uint32_t TAG_SMALLINT = 0x1;

    uint32_t val;

    static_assert(sizeof(void*) == sizeof(uint32_t), "only 32-bit pointers are supported");
    static_assert(sizeof(int) == sizeof(uint32_t), "only 32-bit ints are supported");

public:
    inline Value(void* aligned_ptr) {
        assert(sizeof(ptr) == sizeof(val));
        val = (uint32_t) aligned_ptr;
        assert((val&MASK) == TAG_PTR && "pointer is not properly aligned");
    }

    inline Value(int x) {
        if ((x&0xc0000000) == 0)
            val = (uint32_t(x) << 2) | TAG_SMALLINT;
        else
            assert(false && "int boxing not implemented");
    }

    inline int asInt() {
        assert((val&MASK) == TAG_SMALLINT);
        return int(val >> 2);
    }

    inline void* asPtr() {
        assert((val&MASK) == TAG_PTR);
        return (void*)(val);
    }
};

#endif
