#pragma once

#include <cassert>
#include <functional>

#include "bitset-iterator.h"
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
  bitset_view();
  bitset_view(const bitset_view& other);
  ~bitset_view();

  operator bitset_view<const T>() const;

  bitset_view(iterator first, iterator last);
  explicit bitset_view(bitset& other);

  bitset_view& operator=(const bitset_view& other);

  iterator begin() const;
  iterator end() const;
  size_t size() const;

  bitset_view& operator&=(const bitset_view other);
  bitset_view& operator|=(const bitset_view other);
  bitset_view& operator^=(const bitset_view other);

  void flip();
  bitset_view& set();
  bitset_view& reset();

  bool all() const;
  bool any() const;
  size_t count() const;

  reference operator[](std::size_t index);
  const_reference operator[](std::size_t index) const;

private:
  iterator _begin;
  iterator _end;

  bitset_view& set_bit(bool value);
  bitset_view& operation(const bitset_view& other, const std::function<bool(bool, bool)>& binary_op);
};

template<typename T>
bool operator==(const bitset_view<T> lhs, const bitset_view<T> rhs);

template<typename T>
bool operator!=(const bitset_view<T> lhs, const bitset_view<T> rhs);

template<typename T>
bitset operator&(const bitset_view<T> left, const bitset_view<T> right);

template<typename T>
bitset operator|(const bitset_view<T> left, const bitset_view<T> right);

template<typename T>
bitset operator^(const bitset_view<T> left, const bitset_view<T> right);

template<typename T>
bitset operator~(const bitset_view<T> bs_view);
