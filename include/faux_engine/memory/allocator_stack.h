#ifndef __NX_ALLOCATOR_STACK_H__
#define __NX_ALLOCATOR_STACK_H__

#include "faux_engine/memory/allocator.h"

namespace nx {

  class StackAllocator : public Allocator {

  public:
    StackAllocator(const std::size_t size);
    StackAllocator(const std::size_t size, void* address);
    StackAllocator(const std::size_t size, Allocator* allocator);
    virtual ~StackAllocator();

    virtual void* alloc(const std::size_t size, const std::size_t alignment = 0) override;
    virtual void free(void* ptr) override;
    virtual void reset() override;

  private:
    StackAllocator(StackAllocator& allocator);

    struct AllocationHeader {
      u8 padding;
    };
  };

}

#endif