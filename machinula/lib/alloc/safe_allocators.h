#ifndef SAFE_ALLOCATORS_H_INCLUDED
#define SAFE_ALLOCATORS_H_INCLUDED

#include "lock_allocator.h"

#include "global_allocator.h"
#include "default_allocator.h"
#include "linear_allocator.h"
#include "stack_allocator.h"
#include "pool_allocator.h"
#include "object_pool_allocator.h"
#include "free_list_allocator.h"

#include "proxy_allocator.h"

namespace alloc
{
    using SafeGlobalAllocator = LockAllocator< GlobalAllocator >;

    using SafeDefaultAllocator = LockAllocator< DefaultAllocator >;

    using SafeLinearAllocator = LockAllocator< LinearAllocator >;

    using SafeStackAllocator = LockAllocator< StackAllocator >;

    using SafePoolAllocator = LockAllocator< PoolAllocator >;

    template < typename Type >
    using SafeObjectPoolAllocator = LockAllocator< ObjectPoolAllocator< Type > >;

    using SafeFreeListAllocator = LockAllocator< FreeListAllocator >;

    using SafeProxyAllocator = LockAllocator< ProxyAllocator >;
}

#endif // SAFE_ALLOCATORS_H_INCLUDED
