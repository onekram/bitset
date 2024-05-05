#include "bitset-reference.h"

bitset_reference::bitset_reference() = default;

bitset_reference::bitset_reference(const bitset_reference& other) = default;

bitset_reference::~bitset_reference() = default;

bitset_reference& bitset_reference::operator=(bool value) {
  uint32_t mask = 1UL << _index;
  if (value) {
    *_p |= mask;
  } else {
    *_p &= ~mask;
  }
  return *this;
}

bitset_reference::operator bool() const {
  return (*_p & (1UL << _index)) != 0;
}

bitset_reference& bitset_reference::flip() {
  *_p ^= (1UL << _index);
  return *this;
}

bitset_reference::bitset_reference(pointer p, std::size_t index)
    : _p(p)
    , _index(index) {}
