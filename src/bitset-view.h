#pragma once

#include "bitset-iterator.h"
#include "bitset.h"

#include <algorithm>
#include <cassert>
#include <functional>

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

  ~bitset_view() = default;

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

  bitset_view& operator&=(const bitset_view& other) {
    return operation(other, [](bool l, bool r) { return l && r; });
  }

  bitset_view& operator|=(const bitset_view other) {
    return operation(other, [](bool l, bool r) { return l || r; });
  }

  bitset_view& operator^=(const bitset_view& other) {
    return operation(other, [](bool l, bool r) { return l ^ r; });
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

  friend bitset operator~(const bitset_view& bs_view) {
    bitset bs(bs_view);
    return ~bs;
  }

  void flip() {
    std::for_each(begin(), end(), [](reference el) { el.flip(); });
  }

  bitset_view& set() {
    return set_bit(true);
  }

  bitset_view& reset() {
    return set_bit(false);
  }

  bool all() const {
    return std::all_of(begin(), end(), [](bool el) { return el; });
  }

  bool any() const {
    return std::any_of(begin(), end(), [](bool el) { return el; });
  }

  std::size_t count() const {
    return std::count(begin(), end(), true);
  }

  reference operator[](std::size_t index) {
    return *(begin() + index);
  }

  const_reference operator[](std::size_t index) const {
    return *(begin() + index);
  }

  friend bool operator==(const bitset_view& lhs, const bitset_view& rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

  friend bool operator!=(const bitset_view& lhs, const bitset_view& rhs) {
    return !(lhs == rhs);
  }

private:
  iterator _begin;
  iterator _end;

  bitset_view& set_bit(bool value) {
    std::fill(begin(), end(), value);
    return *this;
  }

  bitset_view& operation(const bitset_view& other, const std::function<bool(bool, bool)>& binary_op) {
    assert(size() == other.size());
    std::transform(begin(), end(), other.begin(), begin(), binary_op);
    return *this;
  }
};
