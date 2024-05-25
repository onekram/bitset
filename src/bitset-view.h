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
  using word_type = uint64_t;

  using reference = T;
  using const_reference = bitset_reference<const word_type>;

  using iterator = bitset_iterator<reference>;
  using const_iterator = bitset_iterator<const_reference>;

  using view = bitset_view<reference>;
  using const_view = bitset_view<const_reference>;

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

  bitset_view operator&=(const const_view& other) const {
    return operation(other, [](word_type l, word_type r) { return l & r; });
  }

  bitset_view operator|=(const const_view& other) const {
    return operation(other, [](word_type l, word_type r) { return l | r; });
  }

  bitset_view operator^=(const const_view& other) const {
    return operation(other, [](word_type l, word_type r) { return l ^ r; });
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
    return std::all_of(begin(), end(), std::identity());
  }

  bool any() const {
    return std::any_of(begin(), end(), std::identity());
  }

  std::size_t count() const {
    return std::count(begin(), end(), true);
  }

  friend void swap(bitset_view& lhs, bitset_view& rhs) {
    lhs.swap(rhs);
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

  static const std::size_t INT_SIZE = std::numeric_limits<word_type>::digits;
  static constexpr word_type ALL_ONE = -1;

  bitset_view set_bit(bool value) const {
    std::fill(begin(), end(), value);
    return *this;
  }

  static word_type first_bits(word_type num, std::size_t count) {
    if (count == 0) {
      return 0;
    }
    if (count == INT_SIZE) {
      return num;
    }
    return num & (ALL_ONE >> (INT_SIZE - count));
  }

  static word_type sub_bits(word_type num, std::size_t offset, std::size_t count) {
    word_type res = first_bits(num, INT_SIZE - offset);
    res >>= (INT_SIZE - offset - count);
    return res;
  }

  static word_type mask_ones(std::size_t count) {
    if (count == 0) {
      return 0;
    }
    return ALL_ONE >> (INT_SIZE - count);
  }

  static void clear_bits(word_type& num, std::size_t offset, std::size_t count) {
    word_type mask = mask_ones(count) << (INT_SIZE - offset - count);
    num &= ~mask;
  }

  static void apply_bits(word_type& num, std::size_t offset, std::size_t count, word_type source) {
    clear_bits(num, offset, count);
    num |= source << (INT_SIZE - offset - count);
  }

  static word_type& get_element(word_type* data, std::size_t idx) {
    return data[idx / INT_SIZE];
  }

  template <class Function>
  bitset_view operation(const const_view& other, Function binary_op) const {
    assert(size() == other.size());
    word_type* data = begin()._cur;
    word_type* other_data = other.begin()._cur;

    std::size_t idx = begin()._index;
    std::size_t other_idx = other.begin()._index;
    std::size_t border = end()._index;

    while (idx < border) {
      std::size_t c = border - idx;
      std::size_t i = idx % INT_SIZE;
      std::size_t j = other_idx % INT_SIZE;

      if (i <= j) {
        c = std::min(c, INT_SIZE - j);
      } else {
        c = std::min(c, INT_SIZE - i);
      }

      word_type source = sub_bits(get_element(other_data, other_idx), j, c);
      word_type des = sub_bits(get_element(data, idx), i, c);

      apply_bits(get_element(data, idx), i, c, binary_op(des, source));
      idx += c;
      other_idx += c;
    }
    return *this;
  }
};
