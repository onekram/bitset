#include "bitset.h"

#include "bitset-iterator.h"

#include <algorithm>
#include <cassert>
#include <sstream>

bitset::bitset()
    : bitset(0) {}

bitset::bitset(std::size_t size, bool value)
    : bitset(size) {
  set_bit(value);
}

bitset::bitset(const bitset& other)
    : bitset(other.begin(), other.end()) {}

bitset::bitset(const_iterator first, const_iterator last)
    : bitset(first, last, 0) {}

bitset::bitset(std::string_view str)
    : bitset(str.size()) {
  std::transform(str.begin(), str.end(), begin(), [](char c) { return c == '1'; });
}

bitset::bitset(const const_view& other)
    : bitset(other.begin(), other.end()) {}

bitset& bitset::operator=(const bitset& other) & {
  if (this != &other) {
    bitset copy(other);
    swap(copy);
  }
  return *this;
}

bitset& bitset::operator=(std::string_view str) & {
  bitset copy(str);
  swap(copy);
  return *this;
}

bitset& bitset::operator=(const const_view& other) & {
  bitset copy(other);
  swap(copy);
  return *this;
}

bitset::~bitset() {
  delete[] _data;
}

std::size_t bitset::size() const {
  return _size;
}

bool bitset::empty() const {
  return size() == 0;
}

bitset::reference bitset::operator[](std::size_t index) {
  return begin()[index];
}

bitset::const_reference bitset::operator[](std::size_t index) const {
  return begin()[index];
}

bitset::iterator bitset::begin() {
  return {_data, 0};
}

bitset::const_iterator bitset::begin() const {
  return {_data, 0};
}

bitset::iterator bitset::end() {
  return {_data, _size};
}

bitset::const_iterator bitset::end() const {
  return {_data, _size};
}

bitset& bitset::operator&=(const const_view& other) & {
  view(*this) &= other;
  return *this;
}

bitset& bitset::operator|=(const const_view& other) & {
  view(*this) |= other;
  return *this;
}

bitset& bitset::operator^=(const const_view& other) & {
  view(*this) ^= other;
  return *this;
}

bitset& bitset::operator<<=(std::size_t count) & {
  if (get_capacity(size() + count) > _capacity) {
    bitset bs(begin(), end(), count);
    swap(bs);
    return *this;
  }
  _size += count;
  set_bit(end() - count, end(), false);
  return *this;
}

bitset& bitset::operator>>=(std::size_t count) & {
  if (count >= size()) {
    bitset bs;
    swap(bs);
    return *this;
  }
  if (get_capacity(size() - count) < _capacity) {
    bitset bs(begin(), end() - count);
    swap(bs);
    return *this;
  }
  _size -= count;
  return *this;
}

bitset& bitset::flip() & {
  subview().flip();
  return *this;
}

bitset& bitset::set() & {
  subview().set();
  return *this;
}

bitset& bitset::reset() & {
  subview().reset();
  return *this;
}

bitset& bitset::set_bit(bool value) {
  return set_bit(begin(), end(), value);
}

bitset& bitset::set_bit(const iterator& first, const iterator& last, bool value) {
  if (value) {
    view(first, last).set();
  } else {
    view(first, last).reset();
  }
  return *this;
}

std::size_t bitset::get_capacity(std::size_t size) {
  return (size + bitset::INT_SIZE - 1) / bitset::INT_SIZE;
}

void bitset::swap(bitset& other) {
  std::swap(_size, other._size);
  std::swap(_capacity, other._capacity);
  std::swap(_data, other._data);
}

bool bitset::all() const {
  return subview().all();
}

bool bitset::any() const {
  return subview().any();
}

std::size_t bitset::count() const {
  return subview().count();
}

bitset::operator const_view() const {
  return {begin(), end()};
}

bitset::operator view() {
  return {begin(), end()};
}

bitset::view bitset::subview(std::size_t offset, std::size_t count) {
  if (offset > size()) {
    return {end(), end()};
  }
  if (offset + count <= size() && count <= offset + count) {
    return {begin() + offset, begin() + offset + count};
  }

  return {begin() + offset, end()};
}

bitset::const_view bitset::subview(std::size_t offset, std::size_t count) const {
  if (offset > size()) {
    return {end(), end()};
  }
  if (offset + count <= size() && count <= offset + count) {
    return {begin() + offset, begin() + offset + count};
  }

  return {begin() + offset, end()};
}

bitset::bitset(const_iterator first, const_iterator last, std::size_t extra_size)
    : bitset(last - first + extra_size) {
  std::transform(first, last, begin(), std::identity());
  set_bit(end() - extra_size, end(), false);
}

bitset::bitset(std::size_t size)
    : _size(size)
    , _capacity(get_capacity(_size))
    , _data(nullptr) {
  if (_capacity > 0) {
    _data = new bitset::word_type[_capacity];
  }
}

bool operator==(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  return lhs.size() == rhs.size() &&
         lhs.apply_binary(
             rhs,
             [](bitset::word_type& num, std::size_t offset, std::size_t count, bitset::word_type source) {
               bitset::word_type des = bitset::const_view::sub_bits(num, offset, count);
               return des == source;
             }
         );
}

bool operator!=(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  return !(lhs == rhs);
}

bitset operator&(const bitset::const_view& left, const bitset::const_view& right) {
  bitset bs(left);
  bs &= right;
  return bs;
}

bitset operator|(const bitset::const_view& left, const bitset::const_view& right) {
  bitset bs(left);
  bs |= right;
  return bs;
}

bitset operator^(const bitset::const_view& left, const bitset::const_view& right) {
  bitset bs(left);
  bs ^= right;
  return bs;
}

bitset operator~(const bitset::const_view& bs_view) {
  bitset bs(bs_view);
  bs.flip();
  return bs;
}

bitset operator<<(const bitset::const_view& bs_view, std::size_t count) {
  bitset bs(bs_view);
  bs <<= count;
  return bs;
}

bitset operator>>(const bitset::const_view& bs_view, std::size_t count) {
  bitset bs(bs_view);
  bs >>= count;
  return bs;
}

std::ostream& operator<<(std::ostream& out, const bitset::const_view& bs) {
  for (auto el : bs) {
    out << el;
  }
  return out;
}

std::string to_string(const bitset::const_view& bs_view) {
  std::string s;
  s.reserve(bs_view.size());
  for (auto el : bs_view) {
    s.push_back('0' + el);
  }
  return s;
}

void swap(bitset& lhs, bitset& rhs) {
  lhs.swap(rhs);
}
