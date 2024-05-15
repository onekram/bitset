#pragma once

#include "bitset-iterator.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <sstream>

template <typename T>
class bitset_view {
public:
  using value_type = bool;

  using reference = T;
  using const_reference = bitset_reference<const uint32_t>;

  using iterator = bitset_iterator<reference>;
  using const_iterator = bitset_iterator<const_reference>;

  static constexpr std::size_t npos = -1;

public:
  bitset_view() = default;

  bitset_view(iterator first, iterator last)
      : _begin(first)
      , _end(last) {}

  bitset_view(const bitset_view& other) = default;

  bitset_view& operator=(const bitset_view& other) = default;

  ~bitset_view() = default;

  operator bitset_view<const_reference>() const {
    return {begin(), end()};
  }

  void swap(bitset_view& other) {
    std::swap(_begin, other._begin);
    std::swap(_end, other._end);
  }

  std::size_t size() const {
    return _end - _begin;
  }

  bool empty() const {
    return size() == 0;
  }

  reference operator[](std::size_t index) const {
    return *(begin() + index);
  }

  iterator begin() const {
    return _begin;
  }

  iterator end() const {
    return _end;
  }

  bitset_view operator&=(const bitset_view<const_reference>& other) const {
    return operation(other, [](bool l, bool r) { return l && r; });
  }

  bitset_view operator|=(const bitset_view<const_reference>& other) const {
    return operation(other, [](bool l, bool r) { return l || r; });
  }

  bitset_view operator^=(const bitset_view<const_reference>& other) const {
    return operation(other, [](bool l, bool r) { return l ^ r; });
  }

  bitset_view flip() const {
    std::for_each(begin(), end(), [](reference el) { el.flip(); });
    return *this;
  }

  bitset_view set() const {
    return set_bit(true);
  }

  bitset_view reset() const {
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

  friend void swap(bitset_view& lhs, bitset_view& rhs) {
    lhs.swap(rhs);
  }

  friend std::string to_string(const bitset_view& bs_view) {
    std::stringstream ss;
    for (auto b : bs_view) {
      ss << b;
    }
    return ss.str();
  }

  bitset_view subview(std::size_t offset = 0, std::size_t count = npos) const {
    if (offset > size()) {
      return {end(), end()};
    }
    if (offset + count <= size() && count <= offset + count) {
      return {begin() + offset, begin() + offset + count};
    }

    return {begin() + offset, end()};
  }

private:
  iterator _begin;
  iterator _end;

  bitset_view set_bit(bool value) const {
    std::fill(begin(), end(), value);
    return *this;
  }

  bitset_view
  operation(const bitset_view<const_reference>& other, const std::function<bool(bool, bool)>& binary_op) const {
    assert(size() == other.size());
    std::transform(begin(), end(), other.begin(), begin(), binary_op);
    return *this;
  }
};
