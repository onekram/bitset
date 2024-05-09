#pragma once

#include "bitset-iterator.h"
#include "bitset-view.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string_view>

class bitset {
public:
  using value_type = bool;

  using reference = bitset_reference<uint32_t>;
  using const_reference = bitset_reference<const uint32_t>;

  using iterator = bitset_iterator<reference>;
  using const_iterator = bitset_iterator<const_reference>;

  using view = bitset_view<reference>;
  using const_view = bitset_view<const_reference>;

  using word_type = uint32_t;

  static constexpr std::size_t npos = -1;

  bitset();
  bitset(std::size_t size, bool value);
  bitset(const bitset& other);
  explicit bitset(std::string_view str);
  explicit bitset(const const_view& other);
  bitset(const_iterator first, const_iterator last);

  bitset& operator=(const bitset& other) &;
  bitset& operator=(std::string_view str) &;
  bitset& operator=(const const_view& other) &;

  ~bitset();

  void swap(bitset& other);

  std::size_t size() const;
  bool empty() const;

  reference operator[](std::size_t index);
  const_reference operator[](std::size_t index) const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  bitset& operator&=(const const_view& other) &;
  bitset& operator|=(const const_view& other) &;
  bitset& operator^=(const const_view& other) &;
  bitset& operator<<=(std::size_t count) &;
  bitset& operator>>=(std::size_t count) &;
  bitset& flip() &;

  bitset& set() &;
  bitset& reset() &;

  bool all() const;
  bool any() const;
  std::size_t count() const;

  operator const_view() const;
  operator view();

  view subview(std::size_t offset = 0, std::size_t count = npos);
  const_view subview(std::size_t offset = 0, std::size_t count = npos) const;

private:
  size_t _size;
  size_t _capacity;
  word_type* _data;

  bitset(const_iterator first, const_iterator last, std::size_t size);

  bitset& set_bit(bool value);
  bitset& set_bit(const iterator& first, const iterator& last, bool value);

  bitset& operation(const const_view& other, const std::function<bool(bool, bool)>& binary_op);

  static const std::size_t INT_SIZE = 32;
  static std::size_t get_capacity(std::size_t size);
};

bool operator==(const bitset::const_view& lhs, const bitset::const_view& rhs);
bool operator!=(const bitset::const_view& lhs, const bitset::const_view& rhs);

bitset operator&(const bitset::const_view& left, const bitset::const_view& right);
bitset operator|(const bitset::const_view& left, const bitset::const_view& right);
bitset operator^(const bitset::const_view& left, const bitset::const_view& right);
bitset operator~(const bitset::const_view& bs_view);
bitset operator<<(const bitset::const_view& bs_view, std::size_t count);
bitset operator>>(const bitset::const_view& bs_view, std::size_t count);

std::ostream& operator<<(std::ostream& out, const bitset::const_view bs);

std::string to_string(const bitset& bs);

void swap(bitset& lhs, bitset& rhs);
