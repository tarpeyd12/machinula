#ifndef ALLOC_H_INCLUDED
#define ALLOC_H_INCLUDED

#include "align.h"

#include "allocator.h"
#include "default_allocator.h"
#include "linear_allocator.h"
#include "stack_allocator.h"
#include "pool_allocator.h"
#include "object_pool_allocator.h"
#include "free_list_allocator.h"
#include "lock_allocator.h"

#include "proxy_allocator.h"

#include "safe_allocators.h"

#include "stl_adapter.h"
#include "stl_alias.h"

#include "ptr_alias.h"

namespace ptr
{
    using namespace alloc::stl;
    using namespace alloc::ptr;
    using namespace alloc;
}

#endif // ALLOC_H_INCLUDED
