#include "faux_engine/memory/allocator_list.h"
#include "easylogging++.h"

nx::ListAllocator::ListAllocator(const std::size_t size) : Allocator(size) { }

nx::ListAllocator::ListAllocator(const std::size_t size, void* address) : Allocator(size, address) { }

nx::ListAllocator::ListAllocator(const std::size_t size, Allocator* allocator) : Allocator(size, allocator) { }

nx::ListAllocator::~ListAllocator() { }

void* nx::ListAllocator::alloc(const std::size_t size, const std::size_t alignment) {

  return nullptr;
}

void nx::ListAllocator::free(void* ptr) {

}

void nx::ListAllocator::reset() {

  used_ = 0;
}
