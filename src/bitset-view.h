#pragma once

#include "bitset-iterator.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <sstream>

class bitset;

template <typename T>
class bitset_view {
public:
  using value_type = bool;

  using reference = T;
  using const_reference = bool;

  using iterator = bitset_iterator<T>;
  using const_iterator = bitset_iterator<bool>;

public:
  bitset_view() = default;

  bitset_view(const bitset_view& other) = default;

  operator bitset_view<bool>() const {
    return {begin(), end()};
  }

  bitset_view(iterator first, iterator last)
      : _begin(first)
      , _end(last) {}

  bitset_view& operator=(const bitset_view& other) {
    if (this != &other) {
      _begin = other._begin;
      _end = other._end;
    }
    return *this;
  }

  ~bitset_view() = default;

  void swap(bitset_view& other) {
    std::swap(begin(), other.begin());
    std::swap(end(), other.end());
  }

  std::size_t size() const {
    return _end - _begin;
  }

  bool empty() const {
    return size() == 0;
  }

  reference operator[](std::size_t index) {
    return *(begin() + index);
  }

  const_reference operator[](std::size_t index) const {
    return *(begin() + index);
  }

  iterator begin() {
    return _begin;
  }

  const_iterator begin() const {
    return _begin;
  }

  iterator end() {
    return _end;
  }

  const_iterator end() const {
    return _end;
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

  friend bool operator==(const bitset_view& lhs, const bitset_view& rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

  friend bool operator!=(const bitset_view& lhs, const bitset_view& rhs) {
    return !(lhs == rhs);
  }

  friend std::ostream& operator<<(std::ostream& out, const bitset_view& bs_view) {
    for (bitset_view::const_iterator it = bs_view.begin(); it != bs_view.end(); ++it) {
      out << (*it);
    }
    return out;
  }

  friend std::string to_string(const bitset_view& bs_view) {
    std::stringstream ss;
    for (auto b : bs_view) {
      ss << b;
    }
    return ss.str();
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
