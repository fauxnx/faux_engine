#include "faux_engine/memory/memory_manager.h"

nx::MemoryManager::MemoryManager() {
  
}

nx::MemoryManager::~MemoryManager() {
  delete linear_;
  delete stack_;
  delete pool_;
  delete heap_;
}

nx::Result nx::MemoryManager::init() {

  heap_ = new LinearAllocator(2048);
  linear_ = new LinearAllocator(512, heap_);
  stack_ = new StackAllocator(512, heap_);
  pool_ = new PoolAllocator(1024, 512, heap_);

  void* ptr1 = pool_->alloc();
  void* ptr2 = pool_->alloc();
  void* ptr3 = pool_->alloc();

  return nx::Result::Success;
}
