#ifndef __NX_MEMORY_MANAGER_H__
#define __NX_MEMORY_MANAGER_H__

#include "faux_engine/memory/allocator_linear.h"
#include "faux_engine/memory/allocator_pool.h"
#include "faux_engine/memory/allocator_stack.h"
#include "faux_engine/memory/allocator_list.h"

namespace nx {
  class MemoryManager {

  public:
    MemoryManager();
    ~MemoryManager();

    nx::Result init();

    LinearAllocator* linear_;
    StackAllocator* stack_;
    PoolAllocator* pool_;

  private:
    LinearAllocator* heap_;
  };
}

#endif