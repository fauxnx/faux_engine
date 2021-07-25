#include "faux_engine/memory/allocator.h"
#include "easylogging++.h"


nx::Allocator::Allocator(const std::size_t size) {

  size_ = size;
  used_ = 0;
  start_ = malloc(size_);
  preallocated_ = false;

  if (start_ == nullptr) {
    while (start_ == nullptr) {
      size_ >>= 1;
      start_ = malloc(size_);
    }
    LOG(ERROR) << "Could not allocate the requested amount. Allocated " << size_ << " instead";
  }

  LOG(TRACE) << "Created new allocator with size " << size_;
}

nx::Allocator::Allocator(const std::size_t size, void* address) {

  size_ = size;
  used_ = 0;
  start_ = address;
  preallocated_ = true;
}

nx::Allocator::Allocator(const std::size_t size, Allocator* allocator) {

  size_ = size;
  used_ = 0;
  start_ = allocator->alloc(size_, alignof(Allocator));
  preallocated_ = true;

  if (start_ == nullptr) {
    while (start_ == nullptr) {
      size_ >>= 1;
      start_ = allocator->alloc(size_, alignof(Allocator));
    }
    LOG(ERROR) << "Could not allocate the requested amount. Allocated " << size_ << " instead";
  }

  LOG(TRACE) << "Created new allocator with size " << size_;
}


nx::Allocator::~Allocator() {

  if (start_ != nullptr && !preallocated_) {
    LOG(TRACE) << "Freed linear allocator at address " << start_;
    ::free(start_);
    start_ = nullptr;
  }
}
