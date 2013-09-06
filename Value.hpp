#ifndef VALUE_HPP
#define VALUE_HPP

#include <cstdint>
#include <cassert>
#include <memory>

#include "Allocator.hpp"

namespace tagptr {

class Value {
private:
    static const uint32_t MASK = 0x3;
    static const uint32_t TAG_PTR = 0x0;
    static const uint32_t TAG_SMALLINT = 0x1;
    static const uint32_t TAG_BOXEDINT = 0x2;
    static const uint32_t TAG_DOUBLE = 0x3;
    static const uint32_t MASK_BOXED = 0x2;

    uint32_t val;

    static_assert(sizeof(void*) == sizeof(uint32_t), "only 32-bit pointers are supported");
    static_assert(sizeof(double) == sizeof(int64_t), "only 64-bit doubles are supported");

    inline void box(uint64_t x, uint32_t tag)
    {
        uint64_t* ptr = tagptr::global_allocator.allocate(1);
        *ptr = x;
        val = (uint32_t) ptr;
        assert((val&MASK) == 0x0 &&
               "boost::allocator returned unaligned address");
        val |= tag;
    }

    inline void box_copy(const Value& other)
    {
        assert(other.val&MASK_BOXED);
        uint64_t* other_ptr = (uint64_t*)(other.val&(~MASK));
        uint64_t* ptr = tagptr::global_allocator.allocate(1);
        *ptr = *other_ptr;
        val = (uint32_t) ptr;
        assert((val&MASK) == 0x0 &&
               "boost::allocator returned unaligned address");
        val |= other.val&MASK;
    }

    inline void release()
    {
        if (val&MASK_BOXED)
            tagptr::global_allocator.deallocate(reinterpret_cast<uint64_t*>(val&(~MASK)), 1);
    }

public:
    inline Value& operator=(const Value& other)
    {
        if (this == &other)
            return *this;

        release();

        if (other.val&MASK_BOXED)
            box_copy(other);
        else
            val = other.val;

        return *this;
    }

    inline Value(const Value& other)
    {
        if (other.val&MASK_BOXED)
            box_copy(other);
        else
            val = other.val;
    }

    inline explicit Value(void* aligned_ptr)
    {
        val = (uint32_t) aligned_ptr;
        assert((val&MASK) == TAG_PTR && "pointer is not properly aligned");
    }

    inline explicit Value(int32_t x)
    {
        if ((x&0xc0000000) == 0)
            val = (uint32_t(x) << 2) | TAG_SMALLINT;
        else
            box(x, TAG_BOXEDINT);
    }

    inline explicit Value(int64_t x)
    {
        if ((x&0xffffffffc0000000) == 0)
            val = (uint32_t(x) << 2) | TAG_SMALLINT;
        else
            box(x, TAG_BOXEDINT); // FIXME: reinterpret_cast?
    }

    inline explicit Value(double x)
    {
        box(*reinterpret_cast<uint64_t*>(&x), TAG_DOUBLE);
    }

    inline int64_t asInt64()
    {
        if ((val&MASK) == TAG_SMALLINT)
            return int64_t(val >> 2);
        else
        {
            assert((val&MASK) == TAG_BOXEDINT);
            return *(int64_t*)(val^TAG_BOXEDINT);
        }
    }

    // NOTE: this overflows if Value contains a 64-bit int
    inline int32_t asInt32()
    {
        return int(asInt64());
    }

    inline void* asPtr()
    {
        assert((val&MASK) == TAG_PTR);
        return (void*)(val);
    }

    inline double asDouble()
    {
        assert((val&MASK) == TAG_DOUBLE);
        return *reinterpret_cast<double*>(val^TAG_DOUBLE);
    }

    ~Value()
    {
        release();
    }
};

}

#endif
