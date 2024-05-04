#pragma once

#include "bitset-iterator.h"
#include <cstddef>

class bitset_view {
  public:
    using value_type = bool;
    using reference = bitset_reference<uint32_t>;
    using const_reference = bitset_reference<const uint32_t>;
    using iterator = bitset_iterator<uint32_t>;
    using const_iterator = bitset_iterator<const uint32_t>;
    using word_type = uint32_t;

  public:


  private:
    size_t _size;
    size_t _capacity;
    word_type* _data;
};