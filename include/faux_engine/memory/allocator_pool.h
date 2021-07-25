#ifndef __NX_ALLOCATOR_POOL_H__
#define __NX_ALLOCATOR_POOL_H__

#include "faux_engine/memory/allocator.h"
#include <stack>

namespace nx {

  class PoolAllocator : public Allocator {

  public:
    PoolAllocator(const std::size_t size, const size_t chunkSize);
    PoolAllocator(const std::size_t size, const size_t chunkSize, void* address);
    PoolAllocator(const std::size_t size, const size_t chunkSize, Allocator* allocator);
    virtual ~PoolAllocator();

    void* alloc();
    virtual void* alloc(const std::size_t size, const std::size_t alignment = 0) override;
    virtual void free(void* ptr) override;
    virtual void reset() override;

  protected:
    std::size_t chunkSize_;

  private:
    PoolAllocator(PoolAllocator& allocator);

    std::stack<void*> chunks_;
  };
}

#endif