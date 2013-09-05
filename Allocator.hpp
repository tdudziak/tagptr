#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#ifdef POOL_ALLOCATOR
#include <boost/pool/pool_alloc.hpp>
namespace tagptr {
    typedef boost::pool_allocator<uint64_t> allocator_t;
}
#else
#include <memory>
namespace tagptr {
    typedef std::allocator<uint64_t> allocator_t;
}
#endif

namespace tagptr {
    extern allocator_t global_allocator;
}

#endif
