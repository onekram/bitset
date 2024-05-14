#pragma once

#include <cstddef>
#include <cstdint>

template <typename T>
class bitset_reference {
public:
  using pointer = T*;
  using word_type = uint32_t;

public:
  bitset_reference() = delete;

  bitset_reference(const bitset_reference& other) = default;

  bitset_reference& operator=(const bitset_reference& other) = default;

  ~bitset_reference() = default;

  bitset_reference& operator=(bool value) {
    if (value) {
      *_p |= get_mask();
    } else {
      *_p &= ~get_mask();
    }
    return *this;
  }

  const bitset_reference& operator=(bool value) const {
    if (value) {
      *_p |= get_mask();
    } else {
      *_p &= ~get_mask();
    }
    return *this;
  }

  operator bool() const {
    return (*_p & get_mask()) != 0;
  }

  operator bitset_reference<const uint32_t>() const {
    return {_p, _index};
  }

  bitset_reference flip() const {
    *_p ^= get_mask();
    return *this;
  }

  bitset_reference(pointer p, std::size_t index)
      : _p(p)
      , _index(index) {}

private:
  pointer _p;
  std::size_t _index;

  uint32_t get_mask() const {
    uint32_t ONE = 1;
    return ONE << _index;
  }
};
