#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string_view>

#include "bitset-reference.h"
#include "bitset-view.h"

class bitset {
public:
  using value_type = bool;
  using reference = bitset_reference<uint32_t>;
  using const_reference = bitset_reference<const uint32_t>;
  using iterator = bitset_iterator<uint32_t>;
  using const_iterator = bitset_iterator<const uint32_t>;
  using view = bitset_view<uint32_t>;
  using const_view = bitset_view<const uint32_t>;
  using word_type = uint32_t;

  static constexpr std::size_t npos = -1;

  bitset();
  bitset(std::size_t size, bool value);
  bitset(const bitset& other);
  explicit bitset(std::string_view str);
  explicit bitset(const const_view& other);
  bitset(const const_iterator& first, const const_iterator& last);

  bitset& operator=(const bitset& other) &;
  bitset& operator=(std::string_view str) &;
  bitset& operator=(const const_view& other) &;

  ~bitset();

  void swap(bitset& other) noexcept;

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
  void flip() &;

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

  bitset(const const_iterator& first, const const_iterator& last, size_t size);

  bitset& set_bit(bool value);
  bitset& operation(const const_view& other, const std::function<bool(bool, bool)>& binary_op);

  static const size_t INT_SIZE = 32;
  static size_t get_capacity(size_t size);
};

bool operator==(const bitset& left, const bitset& right);
bool operator!=(const bitset& left, const bitset& right);

bitset operator&(const bitset& left, const bitset& right);
bitset operator|(const bitset& left, const bitset& right);
bitset operator^(const bitset& left, const bitset& right);
bitset operator~(const bitset& bs);
std::ostream& operator<<(std::ostream& out, const bitset& bs);

std::string to_string(const bitset& bs);
void swap(bitset& lhs, bitset& rhs) noexcept;
