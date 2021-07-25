#ifndef __NX_ALLOCATOR_H__
#define __NX_ALLOCATOR_H__

#include "faux_engine/types.h"
#include <cstddef>

namespace nx {

  class Allocator {

  public: 
    Allocator(const std::size_t size);
    Allocator(const std::size_t size, void* address);
    Allocator(const std::size_t size, Allocator* allocator);
    virtual ~Allocator();

    virtual void* alloc(const std::size_t size, const std::size_t alignment = 0) = 0;
    virtual void free(void* ptr) = 0;
    virtual void reset() = 0;

  protected:
    std::size_t size_;
    std::size_t used_;
    void* start_;
    bool preallocated_;

  };

}

#endif