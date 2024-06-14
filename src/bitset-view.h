#pragma once

#include "bitset-iterator.h"

#include <algorithm>
#include <bit>
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
    return begin()[index];
  }

  iterator begin() const {
    return _begin;
  }

  iterator end() const {
    return _end;
  }

  bitset_view operator&=(const const_view& other) const {
    return operation(other, [](word_type lhs, word_type rhs) { return lhs & rhs; });
  }

  bitset_view operator|=(const const_view& other) const {
    return operation(other, [](word_type lhs, word_type rhs) { return lhs | rhs; });
  }

  bitset_view operator^=(const const_view& other) const {
    return operation(other, [](word_type lhs, word_type rhs) { return lhs ^ rhs; });
  }

  bitset_view flip() const {
    apply_unary([](word_type& num, std::size_t offset, std::size_t count) {
      word_type des = sub_bits(num, offset, count);
      apply_bits(num, offset, count, ~des);
      return true;
    });
    return *this;
  }

  bitset_view set() const {
    return set_bits(true);
  }

  bitset_view reset() const {
    return set_bits(false);
  }

  bool all() const {
    return apply_unary([](word_type num, std::size_t offset, std::size_t count) {
      return count_bits(sub_bits(num, offset, count)) == count;
    });
  }

  bool any() const {
    return !apply_unary([](word_type num, std::size_t offset, std::size_t count) {
      return sub_bits(num, offset, count) == 0;
    });
  }

  std::size_t count() const {
    std::size_t c = 0;
    apply_unary([&c](word_type num, std::size_t offset, std::size_t count) {
      c += count_bits(sub_bits(num, offset, count));
      return true;
    });
    return c;
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

  friend bool operator==(const const_view& lhs, const const_view& rhs);

private:
  iterator _begin;
  iterator _end;

  static const std::size_t INT_SIZE = std::numeric_limits<word_type>::digits;
  static constexpr word_type ALL_ONE = -1;

  bitset_view set_bits(bool value) const {
    word_type mask = value ? mask_ones(INT_SIZE) : 0;
    apply_unary([mask](word_type& num, std::size_t offset, std::size_t count) {
      apply_bits(num, offset, count, first_bits(mask, count));
      return true;
    });
    return *this;
  }

  static std::size_t count_bits(word_type num) {
    return std::popcount(num);
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
    if (offset == 0 && count == INT_SIZE) {
      num = 0;
    } else {
      word_type mask = mask_ones(count) << (INT_SIZE - offset - count);
      num &= ~mask;
    }
  }

  static void apply_bits(word_type& num, std::size_t offset, std::size_t count, word_type source) {
    if (offset == 0 && count == INT_SIZE) {
      num = source;
    } else {
      clear_bits(num, offset, count);
      num |= first_bits(source, count) << (INT_SIZE - offset - count);
    }
  }

  static word_type& get_element(word_type* data, std::size_t idx) {
    return data[idx / INT_SIZE];
  }

  template <class Function>
  bool apply_binary(const const_view& other, Function binary_op) const {
    assert(size() == other.size());

    word_type* data = begin()._cur;
    word_type* other_data = other.begin()._cur;
    std::size_t idx = begin()._index;
    std::size_t other_idx = other.begin()._index;
    std::size_t border = end()._index;

    while (idx < border) {
      std::size_t count = border - idx;
      std::size_t i = idx % INT_SIZE;
      std::size_t j = other_idx % INT_SIZE;

      if (i <= j) {
        count = std::min(count, INT_SIZE - j);
      } else {
        count = std::min(count, INT_SIZE - i);
      }

      word_type& cur = get_element(data, idx);
      word_type other_cur = get_element(other_data, other_idx);
      word_type source = sub_bits(other_cur, j, count);

      if (!binary_op(cur, i, count, source)) {
        return false;
      }
      idx += count;
      other_idx += count;
    }
    return true;
  }

  template <class Function>
  bool apply_unary(Function unary_op) const {
    word_type* data = begin()._cur;

    std::size_t idx = begin()._index;
    std::size_t border = end()._index;

    while (idx < border) {
      std::size_t i = idx % INT_SIZE;
      std::size_t count = std::min(border - idx, INT_SIZE - i);
      word_type& cur = get_element(data, idx);

      if (!unary_op(cur, i, count)) {
        return false;
      }
      idx += count;
    }
    return true;
  }

  template <class Function>
  bitset_view operation(const const_view& other, Function binary_op) const {
    apply_binary(other, [&binary_op](word_type& num, std::size_t offset, std::size_t count, word_type source) {
      word_type des = sub_bits(num, offset, count);
      apply_bits(num, offset, count, binary_op(des, source));
      return true;
    });
    return *this;
  }
};
