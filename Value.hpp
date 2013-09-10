#ifndef VALUE_HPP
#define VALUE_HPP

#include <cstdint>
#include <climits>
#include <limits>
#include <cassert>
#include <memory>

#include "Allocator.hpp"

namespace tagptr {

template<typename T_int, typename T_real>
class ValueTemplate {
private:
    static const uint8_t MASK = 0x3;
    static const uint8_t TAG_PTR = 0x0;
    static const uint8_t TAG_SMALLINT = 0x1;
    static const uint8_t TAG_BOXEDINT = 0x2;
    static const uint8_t TAG_REAL = 0x3;

    // val&MASK_BOXED iff ((val&TAG_REAL) || (val&TAG_BOXEDINT))
    static const uint8_t MASK_BOXED = 0x2;

    typedef allocator_t::value_type slot_t;

    // An integer can be stored in unboxed form if it can be represented using
    // 2 bits less than uintptr_t (they're needed for the tag). This can be
    // checked by anding the integer with this mask.
    static const slot_t SLOT_MASK_SMALLINT =
        ~((slot_t(1) << (CHAR_BIT*sizeof(uintptr_t)-2)) - 1);

    // Two lowest bits (i.e., val&MASK) of the value store the tag. Interpre-
    // -tation of the remaining bits depends on this tag.
    //    TAG_PTR (=0x0): ValueTemplate stores a pointer equal to val
    //                    (including the tag bits)
    //    TAG_SMALLINT: ValueTemplate stores an integer equal to (val>>2)
    //    TAG_REAL: ValueTemplate stores a boxed real equal to:
    //              *(real_t*)(val^TAG_REAL)
    //    TAG_BOXEDINT: ValueTemplate stores a boxed int equal to:
    //                  *(int_t*)(val^TAG_BOXEDINT)
    uintptr_t val;

    static_assert(sizeof(void*) <= sizeof(slot_t),
                  "allocator slot is too small to for a pointer value");
    static_assert(sizeof(T_int) <= sizeof(slot_t),
                  "given integer type doesn't fit in one allocator slot");
    static_assert(sizeof(T_real) <= sizeof(slot_t),
                  "given real type doesn't fit in one allocator slot");
    static_assert(std::numeric_limits<slot_t>::is_integer &&
                  !std::numeric_limits<slot_t>::is_signed,
                  "allocator slot type has to be an unsigned integer");

    inline void box(slot_t x, uint8_t tag)
    {
        slot_t* ptr = tagptr::global_allocator.allocate(1);
        *ptr = x;
        val = (uintptr_t) ptr;
        assert((val&MASK) == 0x0 &&
               "allocator returned unaligned address");
        val |= tag;
    }

    inline void box_copy(const ValueTemplate& other)
    {
        assert(other.val&MASK_BOXED);
        slot_t* other_ptr = (slot_t*)(other.val&(~MASK));
        slot_t* ptr = tagptr::global_allocator.allocate(1);
        *ptr = *other_ptr;
        val = (uintptr_t) ptr;
        assert((val&MASK) == 0x0 &&
               "allocator returned unaligned address");
        val |= other.val&MASK;
    }

    inline void release()
    {
        if (val&MASK_BOXED)
            tagptr::global_allocator.deallocate(reinterpret_cast<slot_t*>(val&(~MASK)), 1);
    }

public:
    typedef T_int int_t;
    typedef T_real real_t;

    inline ValueTemplate& operator=(const ValueTemplate& other)
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

    inline ValueTemplate(const ValueTemplate& other)
    {
        if (other.val&MASK_BOXED)
            box_copy(other);
        else
            val = other.val;
    }

    inline explicit ValueTemplate(void* aligned_ptr)
    {
        val = (uintptr_t) aligned_ptr;
        assert((val&MASK) == TAG_PTR && "pointer is not properly aligned");
    }

    inline explicit ValueTemplate(int_t x)
    {
        slot_t slot = slot_t(x);

        if ((slot&SLOT_MASK_SMALLINT) == 0)
            val = uintptr_t(slot<<2) | TAG_SMALLINT;
        else
            box(x, TAG_BOXEDINT);
    }

    inline explicit ValueTemplate(real_t x)
    {
        box(*reinterpret_cast<slot_t*>(&x), TAG_REAL);
    }

    inline int_t asInt()
    {
        if ((val&MASK) == TAG_SMALLINT)
            return int_t(val >> 2);
        else
        {
            assert((val&MASK) == TAG_BOXEDINT);
            return *(int_t*)(val^TAG_BOXEDINT);
        }
    }

    inline void* asPtr()
    {
        assert((val&MASK) == TAG_PTR);
        return (void*)(val);
    }

    inline double_t asReal()
    {
        assert((val&MASK) == TAG_REAL);
        return *reinterpret_cast<real_t*>(val^TAG_REAL);
    }

    inline bool isInt()
    {
        return (val&MASK) == TAG_SMALLINT || (val&MASK) == TAG_BOXEDINT;
    }

    inline bool isReal()
    {
        return (val&MASK) == TAG_REAL;
    }

    inline bool isPtr()
    {
        return (val&MASK) == TAG_PTR;
    }

    ~ValueTemplate()
    {
        release();
    }
};

}

#endif
