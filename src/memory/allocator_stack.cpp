#include "faux_engine/memory/allocator_stack.h"
#include "easylogging++.h"

nx::StackAllocator::StackAllocator(const std::size_t size) : Allocator(size) { }

nx::StackAllocator::StackAllocator(const std::size_t size, void* address) : Allocator(size, address) { }

nx::StackAllocator::StackAllocator(const std::size_t size, Allocator* allocator) : Allocator(size, allocator) { }

nx::StackAllocator::~StackAllocator() { }

void* nx::StackAllocator::alloc(const std::size_t size, const std::size_t alignment) {

  const std::size_t address = (std::size_t)start_ + used_;
  std::size_t padding = 0;
  std::size_t alignedAddress = address;

  if (alignment != 0 && used_ % alignment != 0) {
    alignedAddress = ((address / alignment) + 1) * alignment;
    padding = alignedAddress - address;

    std::size_t headerSize = sizeof(AllocationHeader);

    if (padding < headerSize) {
      headerSize -= padding;

      if (headerSize % alignment > 0) {
        padding += alignment * (1 + (headerSize / alignment));
      }
      else {
        padding += alignment * (headerSize / alignment);
      }
    }
  }

  if ((alignedAddress + size) - (std::size_t)start_ > size_) {
    LOG(ERROR) << "Failed to allocate memory. Requested " << size + padding << " bytes but only " << size_ - used_ << " are available";
    return nullptr;
  }

  const std::size_t headerAddress = alignedAddress - sizeof(AllocationHeader);
  AllocationHeader allocationHeader{ (u8)padding };
  AllocationHeader* headerPtr = (AllocationHeader*)headerAddress;
  headerPtr = &allocationHeader;

  used_ += size;
  used_ += padding;

  LOG(TRACE) << "Succesfully allocated " << size << " bytes + " << padding << " padding";
  return (void*)alignedAddress;
}

void nx::StackAllocator::free(void* ptr) {

  const std::size_t currentAddress = (std::size_t)ptr;
  const std::size_t headerAddress = currentAddress - sizeof(AllocationHeader);
  const AllocationHeader* allocationHeader{ (AllocationHeader*)headerAddress };

  used_ = currentAddress - allocationHeader->padding - (std::size_t)start_;
}

void nx::StackAllocator::reset() {

  used_ = 0;
}