#include "Allocator.hpp"

// FIXME: Make it thread-safe? Use thread-local allocators and deal with Values
// passed from different threads with some synchronization?
namespace tagptr
{
    allocator_t global_allocator;
}
