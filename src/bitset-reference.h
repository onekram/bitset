#pragma once

#include <cstdint>

template <typename T>
class bitset_reference {

  template <typename S>
  friend class bitset_iterator;

public:
  using pointer = T*;

public:
  bitset_reference() = default;

  bitset_reference(const bitset_reference& other) = default;

  ~bitset_reference() = default;

  operator bitset_reference<const T> () const {
    return {_p, _index};
  }

  bitset_reference& operator=(bool value) {
    uint32_t mask = 1UL << _index;
    if (value) {
      *_p |= mask;
    } else {
      *_p &= ~mask;
    }
    return *this;
  }

  operator bool() const {
    return (*_p & (1UL << _index)) != 0;
  }

  bitset_reference& flip() {
    *_p ^= 1UL << _index;
    return *this;
  }

private:
  pointer _p;
  std::size_t _index{};

  bitset_reference(pointer p, std::size_t index)
      : _p(p)
      , _index(index) {}
};
