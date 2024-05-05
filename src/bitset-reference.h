#pragma once

#include <cstddef>
#include <cstdint>

class bitset_reference {
  template <typename S>
  friend class bitset_iterator;

public:
  using pointer = uint32_t*;

public:
  bitset_reference();

  bitset_reference(const bitset_reference& other);

  ~bitset_reference();

  bitset_reference& operator=(bool value);

  operator bool() const;

  bitset_reference& flip();

private:
  pointer _p;
  std::size_t _index;

  uint32_t get_mask() const;

  bitset_reference(pointer p, std::size_t index);
};
