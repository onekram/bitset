#pragma once

#include "bitset-reference.h"

#include <iterator>

template <typename T>
class bitset_iterator {
public:
  using value_type = bool;
  using difference_type = std::ptrdiff_t;
  using pointer = void;
  using reference = bitset_reference<T>;
  using iterator_category = std::random_access_iterator_tag;

public:
  bitset_iterator() = default;

  bitset_iterator(const bitset_iterator& other) = default;

  ~bitset_iterator() = default;

  bitset_iterator(uint32_t* cur, size_t index)
      : _cur(cur)
      , _index(index) {}

  operator bitset_iterator<const T>() const {
    return {_cur, _index};
  }

  // Element access

  reference operator*() const {
    return {_cur + _index / INT_BITS, _index % INT_BITS};
  }

  reference operator[](difference_type n) const {
    return {_cur + (_index + n) / INT_BITS, (_index + n) % INT_BITS};
  }

  // Comparison

  friend bool operator==(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return rhs._index == lhs._index;
  }

  friend bool operator!=(const bitset_iterator& rhs, const bitset_iterator& lhs) {
    return !(rhs == lhs);
  }

  friend bool operator<(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return lhs._index < rhs._index;
  }

  friend bool operator>(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return rhs < lhs;
  }

  friend bool operator<=(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return !(lhs > rhs);
  }

  friend bool operator>=(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return !(lhs < rhs);
  }

  // Operation

  bitset_iterator& operator++() {
    ++_index;
    return *this;
  }

  bitset_iterator operator++(int) {
    bitset_iterator copy(*this);
    ++_index;
    return copy;
  }

  bitset_iterator& operator--() {
    --_index;
    return *this;
  }

  bitset_iterator operator--(int) {
    bitset_iterator copy(*this);
    --_index;
    return copy;
  }

  bitset_iterator& operator+=(difference_type n) {
    _index += n;
    return *this;
  }

  bitset_iterator& operator-=(difference_type n) {
    return *this += -n;
  }

  bitset_iterator operator+(difference_type n) const {
    bitset_iterator copy(*this);
    copy += n;
    return copy;
  }

  bitset_iterator operator-(difference_type n) const {
    return *this + -n;
  }

  friend bitset_iterator operator+(difference_type lhs, const bitset_iterator& rhs) {
    return rhs + lhs;
  }

  friend bitset_iterator operator-(difference_type lhs, const bitset_iterator& rhs) {
    return rhs - lhs;
  }

  friend difference_type operator-(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return static_cast<difference_type>(lhs._index - rhs._index);
  }

private:
  uint32_t* _cur;
  std::size_t _index;
  static const size_t INT_BITS = 32;
};
