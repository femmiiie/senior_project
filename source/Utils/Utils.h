#include <cstdint>

namespace utils
{
  template <typename T>
  uint32_t aligned_size(const T& value, uint32_t alignment = 256)
  {
    value; //removes unused parameter warning
    uint32_t size = sizeof(T);
    return ((size + alignment - 1) / alignment) * alignment;
  }
}
