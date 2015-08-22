/**
 * @file Scg/Memory/AutoDeleteAllocator.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __AutoDeleteAllocator_h__
#define __AutoDeleteAllocator_h__

// STL header files
#include <set>

// Scg header files
#include <Basic/exceptions.h>
#include <exceptions.h>

namespace Scg
{
/**
 * Simple memory allocator that keeps tracks of all allocated memory blocks
 * and deallocates them when it is destructed.
 */
class AutoDeleteAllocator
{
  std::set<void*> allocatedBuffers;

public:
  AutoDeleteAllocator()
  {
  }

  ~AutoDeleteAllocator()
  {
    for (auto it : allocatedBuffers) {
      free(it);
    }
  }

public:
  /**
   * Allocates a memory block with the given size.
   * @param[in] size  The size in bytes of the memory block to allocate.
   * @return A pointer to the memory block.
   */
  void *allocateMem(size_t size)
  {
    void *ptr = malloc(size);
    allocatedBuffers.insert(ptr);
    return ptr;
  }

  /**
   * freeMem a memory block.
   * @param[in] A pointer to the memory block.
   */
  void freeMem(void *ptr)
  {
    auto it = allocatedBuffers.find(ptr);

    if (it == allocatedBuffers.end()) {
      throw EXCEPTION(Core::Basic::MemoryException, CHR('f'),
                      STR("The given pointer doesn't exist "
                          "in this AutoDeleteAllocater."));
    }

    free(ptr);
    allocatedBuffers.erase(it);
  }

private:
  static AutoDeleteAllocator sSingleton;

public:
  static AutoDeleteAllocator &getSingleton()
  {
    return sSingleton;
  }
};
}

#endif // __AutoDeleteAllocator_h__
