#pragma once

#include "bitset-iterator.h"
#include "bitset.h"
#include <cassert>
#include <cstddef>

class bitset;

template <typename T>
class bitset_view {
  public:
    using value_type = bool;
    using reference = bitset_reference<T>;
    using const_reference = bitset_reference<const T>;
    using iterator = bitset_iterator<T>;
    using const_iterator = bitset_iterator<const T>;

  public:
  bitset_view() = default;

  bitset_view(const bitset_view& other) = default;

  operator bitset_view<const T>() const {
    return {begin(), end()};
  }

  bitset_view(iterator first, iterator last)
      : _begin(first)
      , _end(last) {}

  explicit bitset_view(const bitset& other)
      : _begin(other.begin())
      , _end(other.end()) {}


  bitset_view& operator=(const bitset_view& other) {
    if (this != &other) {
      _begin = other._begin;
      _end = other._end;
    }
    return *this;
  }

  iterator begin() const {
    return _begin;
  }

  iterator end() const {
    return _end;
  }

  size_t size() const {
    return _end - _begin;
  }

  friend bitset operator&(const bitset_view& left, const bitset_view& right) {
    bitset bs(left);
    return bs &= right;
  }

  friend bitset operator|(const bitset_view& left, const bitset_view& right) {
    bitset bs(left);
    return bs |= right;
  }

  friend bitset operator^(const bitset_view& left, const bitset_view& right) {
    bitset bs(left);
    return bs ^= right;
  }

  private:
    iterator _begin;
    iterator _end;
};
