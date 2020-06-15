#ifndef TWISTERX_SRC_TWISTERX_CTX_MEMORY_POOL_H_
#define TWISTERX_SRC_TWISTERX_CTX_MEMORY_POOL_H_

#include <memory>
#include "../status.hpp"
namespace twisterx {

/**
 * This class is am exact copy of arrow memory pool
 */
class MemoryPool {
 public:
  virtual ~MemoryPool();

  /// \brief EXPERIMENTAL. Create a new instance of the default MemoryPool
  static std::unique_ptr<MemoryPool> CreateDefault();

  /// Allocate a new memory region of at least size bytes.
  ///
  /// The allocated region shall be 64-byte aligned.
  virtual Status Allocate(int64_t size, uint8_t **out) = 0;

  /// Resize an already allocated memory section.
  ///
  /// As by default most default allocators on a platform don't support aligned
  /// reallocation, this function can involve a copy of the underlying data.
  virtual Status Reallocate(int64_t old_size, int64_t new_size, uint8_t **ptr) = 0;

  /// Free an allocated region.
  ///
  /// @param buffer Pointer to the start of the allocated memory region
  /// @param size Allocated size located at buffer. An allocator implementation
  ///   may use this for tracking the amount of allocated bytes as well as for
  ///   faster deallocation if supported by its backend.
  virtual void Free(uint8_t *buffer, int64_t size) = 0;

  /// The number of bytes that were allocated and not yet free'd through
  /// this allocator.
  virtual int64_t bytes_allocated() const = 0;

  /// Return peak memory allocation in this memory pool
  ///
  /// \return Maximum bytes allocated. If not known (or not implemented),
  /// returns -1
  virtual int64_t max_memory() const;

  /// The name of the backend used by this MemoryPool (e.g. "system" or "jemalloc");
  virtual std::string backend_name() const = 0;

 protected:
  MemoryPool();
};
}

#endif //TWISTERX_SRC_TWISTERX_CTX_MEMORY_POOL_H_
