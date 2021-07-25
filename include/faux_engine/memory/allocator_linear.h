#ifndef __NX_ALLOCATOR_LINEAR_H__
#define __NX_ALLOCATOR_LINEAR_H__

#include "faux_engine/memory/allocator.h"

namespace nx {

  class LinearAllocator : public Allocator {

  public:
    LinearAllocator(const std::size_t size);
    LinearAllocator(const std::size_t size, void* address);
    LinearAllocator(const std::size_t size, Allocator* allocator);
    virtual ~LinearAllocator();

    virtual void* alloc(const std::size_t size, const std::size_t alignment = 0) override;
    virtual void free(void* ptr) override;
    virtual void reset() override;

  private:
    LinearAllocator(LinearAllocator& allocator);
  };

}

#endif