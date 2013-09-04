#ifndef VALUE_HPP
#define VALUE_HPP

#include <cstdint>
#include <cassert>

#include <boost/pool/pool_alloc.hpp>

// TODO: implement full value semantics (copy constructor, assignment operator, ...)
class Value {
private:
    static const uint32_t MASK = 0x3;
    static const uint32_t TAG_PTR = 0x0;
    static const uint32_t TAG_SMALLINT = 0x1;
    static const uint32_t TAG_BOXEDINT = 0x2;

    // XXX: this isn't thread safe
    // XXX: does boost::pool_allocator guarantee alignment?
    static boost::pool_allocator<int64_t> pool;

    uint32_t val;

    static_assert(sizeof(void*) == sizeof(uint32_t), "only 32-bit pointers are supported");

public:
    inline Value(void* aligned_ptr)
    {
        val = (uint32_t) aligned_ptr;
        assert((val&MASK) == TAG_PTR && "pointer is not properly aligned");
    }

    inline Value(int64_t x)
    {
        if ((x&0xc0000000) == 0)
            val = (uint32_t(x) << 2) | TAG_SMALLINT;
        else {
            int64_t* ptr = pool.allocate(1);
            *ptr = x;
            val = (uint32_t) ptr;
            assert((val&MASK) == 0x0 &&
                   "boost::pool_allocator returned unaligned address");
            val |= TAG_BOXEDINT;
        }
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

    inline int asInt()
    {
        return int(asInt64());
    }

    inline void* asPtr()
    {
        assert((val&MASK) == TAG_PTR);
        return (void*)(val);
    }

    ~Value()
    {
        switch (val&MASK)
        {
        case TAG_BOXEDINT:
            pool.deallocate((int64_t*)(val^TAG_BOXEDINT), 1);
            break;
        }
    }
};

#endif
