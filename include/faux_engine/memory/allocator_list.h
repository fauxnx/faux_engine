#ifndef __NX_ALLOCATOR_LIST_H__
#define __NX_ALLOCATOR_LIST_H__

#include "faux_engine/memory/allocator.h"
#include <list>

namespace nx {

  class ListAllocator : public Allocator {

  public:
    ListAllocator(const std::size_t size);
    ListAllocator(const std::size_t size, void* address);
    ListAllocator(const std::size_t size, Allocator* allocator);
    virtual ~ListAllocator();

    virtual void* alloc(const std::size_t size, const std::size_t alignment = 0) override;
    virtual void free(void* ptr) override;
    virtual void reset() override;

    enum PlacementPolicy {
      FIND_FIRST,
      FIND_BEST
    };

  private:
    ListAllocator(ListAllocator& allocator);

    struct FreeHeader {
      std::size_t blockSize;
    };
    struct AllocationHeader {
      std::size_t blockSize;
      char padding;
    };

    std::list<FreeHeader> chunks;
  };

}

#endif