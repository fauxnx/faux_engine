#include "faux_engine/memory/allocator_linear.h"
#include "easylogging++.h"

nx::LinearAllocator::LinearAllocator(const std::size_t size) : Allocator(size) { }

nx::LinearAllocator::LinearAllocator(const std::size_t size, void* address) : Allocator(size, address) { }

nx::LinearAllocator::LinearAllocator(const std::size_t size, Allocator* allocator) : Allocator(size, allocator) { }

nx::LinearAllocator::~LinearAllocator() { }

void* nx::LinearAllocator::alloc(const std::size_t size, const std::size_t alignment) {

  const std::size_t address = (std::size_t)start_ + used_;
  std::size_t padding = 0;
  std::size_t alignedAddress = address;
  
  if (alignment != 0 && used_ % alignment != 0) {
    alignedAddress = ((address / alignment) + 1) * alignment;
    padding = alignedAddress - address;
  }
  
  if ((alignedAddress + size) - (std::size_t)start_ > size_) {
    LOG(ERROR) << "Failed to allocate memory. Requested " << size + padding << " bytes but only " << size_ - used_ << " are available";
    return nullptr;
  }

  used_ += size;
  used_ += padding;

  LOG(TRACE) << "Succesfully allocated " << size << " bytes + " << padding << " padding";
  return (void*)alignedAddress;
}

void nx::LinearAllocator::free(void* ptr) {
  
  LOG(WARNING) << "Attempting to free a linear allocator";
}

void nx::LinearAllocator::reset() {

  used_ = 0;
}
