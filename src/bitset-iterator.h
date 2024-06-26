#pragma once

#include "bitset-reference.h"

#include <cstddef>
#include <iterator>

template <typename T>
class bitset_iterator {
  template <typename S>
  friend class bitset_view;

  friend class bitset;

public:
  using value_type = bool;
  using word_type = uint64_t;

  using difference_type = std::ptrdiff_t;
  using pointer = void;

  using reference = bitset_reference<T>;
  using const_reference = bitset_reference<const T>;

  using iterator_category = std::random_access_iterator_tag;

  friend class bitset_iterator<word_type>;

public:
  bitset_iterator() = default;

  bitset_iterator(const bitset_iterator& other) = default;

  bitset_iterator& operator=(const bitset_iterator& other) = default;

  ~bitset_iterator() = default;

  operator bitset_iterator<const word_type>() const {
    return {_cur, _index};
  }

  // Element access

  reference operator*() const {
    return bitset_reference(_cur + _index / INT_SIZE, _index % INT_SIZE);
  }

  reference operator[](difference_type n) const {
    return bitset_reference(_cur + (_index + n) / INT_SIZE, (_index + n) % INT_SIZE);
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

  friend difference_type operator-(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return static_cast<difference_type>(lhs._index - rhs._index);
  }

private:
  word_type* _cur;
  std::size_t _index;
  static const std::size_t INT_SIZE = std::numeric_limits<word_type>::digits;

  bitset_iterator(word_type* cur, std::size_t index)
      : _cur(cur)
      , _index(index) {}
};
