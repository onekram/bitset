#include "bitset-reference.h"

bitset_reference::bitset_reference(const bitset_reference& other) = default;

bitset_reference::~bitset_reference() = default;

bitset_reference& bitset_reference::operator=(bool value) {
  if (value) {
    *_p |= get_mask();
  } else {
    *_p &= ~get_mask();
  }
  return *this;
}

const bitset_reference& bitset_reference::operator=(bool value) const {
  if (value) {
    *_p |= get_mask();
  } else {
    *_p &= ~get_mask();
  }
  return *this;
}

bitset_reference::operator bool() const {
  return (*_p & get_mask()) != 0;
}

bitset_reference bitset_reference::flip() const {
  *_p ^= get_mask();
  return *this;
}

bitset_reference::bitset_reference(pointer p, std::size_t index)
    : _p(p)
    , _index(index) {}

uint32_t bitset_reference::get_mask() const {
  uint32_t ONE = 1;
  return ONE << _index;
}
