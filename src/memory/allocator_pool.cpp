#include "faux_engine/memory/allocator_pool.h"
#include "easylogging++.h"

nx::PoolAllocator::PoolAllocator(const std::size_t size, const size_t chunkSize) : Allocator(size), chunkSize_{ chunkSize } {
  
  reset();
}

nx::PoolAllocator::PoolAllocator(const std::size_t size, const size_t chunkSize, void* address) : Allocator(size, address), chunkSize_{ chunkSize } {

  reset();
}

nx::PoolAllocator::PoolAllocator(const std::size_t size, const size_t chunkSize, Allocator* allocator) : Allocator(size, allocator), chunkSize_{ chunkSize } {
  
  reset();
}

nx::PoolAllocator::~PoolAllocator() { }

void* nx::PoolAllocator::alloc() {

  return alloc(chunkSize_);
}

void* nx::PoolAllocator::alloc(const std::size_t size, const std::size_t alignment) {

  if (size != chunkSize_) {
    LOG(WARNING) << "Attempting to allocate " << size << " bytes in a pool with chunks of " << chunkSize_ << " bytes";
  }
  if (chunks_.empty()) {
    LOG(ERROR) << "No more chunks available";
    return nullptr;
  }
  void* address = chunks_.top();
  chunks_.pop();

  used_ += size;

  LOG(TRACE) << "Succesfully allocated " << size << " bytes";
  return address;
}

void nx::PoolAllocator::free(void* ptr) {

  used_ -= chunkSize_;
  chunks_.push(ptr);
}

void nx::PoolAllocator::reset() {

  used_ = 0;
  while (!chunks_.empty()) {
    chunks_.pop();
  }
  const std::size_t nChunks = size_ / chunkSize_;
  for (std::size_t i = 0; i < nChunks; ++i) {
    std::size_t address = (std::size_t)start_ + i * chunkSize_;
    chunks_.push((void*)address);
  }
}
