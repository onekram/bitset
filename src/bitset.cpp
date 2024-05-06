#include "bitset.h"

#include "bitset-iterator.h"

#include <algorithm>
#include <cassert>
#include <sstream>

bitset::bitset()
    : _size(0)
    , _capacity(0)
    , _data(nullptr) {}

bitset::bitset(std::size_t size, bool value)
    : _size(size)
    , _capacity(get_capacity(size))
    , _data(nullptr) {
  if (_capacity > 0) {
    _data = new uint32_t[_capacity]{};
    if (value) {
      set_bit(value);
    }
  }
}

bitset::bitset(const bitset& other)
    : bitset(other.begin(), other.end()) {}

bitset::bitset(const_iterator first, const_iterator last)
    : bitset(first, last, last - first) {}

bitset::bitset(std::string_view str)
    : _size(str.size())
    , _capacity(get_capacity(_size))
    , _data(nullptr) {
  if (_capacity > 0) {
    _data = new uint32_t[_capacity]{};
    std::generate(begin(), end(), [oit = str.begin()]() mutable { return *(oit++) == '1'; });
  }
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
  return *(begin() + index);
}

bitset::const_reference bitset::operator[](std::size_t index) const {
  return *(begin() + index);
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
  return operation(other, [](bool l, bool r) { return l && r; });
}

bitset& bitset::operator|=(const const_view& other) & {
  return operation(other, [](bool l, bool r) { return l || r; });
}

bitset& bitset::operator^=(const const_view& other) & {
  return operation(other, [](bool l, bool r) { return l ^ r; });
}

bitset& bitset::operator<<=(std::size_t count) & {
  if (get_capacity(size() + count) > _capacity) {
    bitset bs(begin(), end(), size() + count);
    swap(bs);
    return *this;
  }
  _size += count;
  set_bit(end() - count, end(), false);
  return *this;
}

bitset& bitset::operator>>=(std::size_t count) & {
  if (size() >= count) {
    _size -= count;
  } else {
    _size = 0;
  }
  return *this;
}

bitset& bitset::flip() & {
  std::for_each(begin(), end(), [](reference el) { el.flip(); });
  return *this;
}

bitset& bitset::set() & {
  return set_bit(true);
}

bitset& bitset::reset() & {
  return set_bit(false);
}

bitset& bitset::set_bit(bool value) {
  return set_bit(begin(), end(), value);
}

bitset& bitset::set_bit(const iterator& first, const iterator& last, bool value) {
  std::fill(first, last, value);
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
  return std::all_of(begin(), end(), [](bool el) { return el; });
}

bool bitset::any() const {
  return std::any_of(begin(), end(), [](bool el) { return el; });
}

std::size_t bitset::count() const {
  return std::count(begin(), end(), true);
}

bitset& bitset::operation(const const_view& other, const std::function<bool(bool, bool)>& binary_op) {
  assert(size() == other.size());
  std::transform(begin(), end(), other.begin(), begin(), binary_op);
  return *this;
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

bitset::bitset(const_iterator first, const_iterator last, std::size_t size)
    : _size(size)
    , _capacity(get_capacity(size))
    , _data(nullptr) {
  if (_capacity > 0) {
    _data = new uint32_t[_capacity]{};
    std::generate(begin(), end(), [oit = first, &last]() mutable -> bool {
      if (oit >= last) {
        return false;
      }
      return *(oit++);
    });
  }
}

bool operator==(const bitset& left, const bitset& right) {
  return left.size() == right.size() && std::equal(left.begin(), left.end(), right.begin());
}

bool operator!=(const bitset& left, const bitset& right) {
  return !(left == right);
}

bitset operator&(const bitset::const_view& left, const bitset::const_view& right) {
  bitset bs(left);
  return bs &= right;
}

bitset operator|(const bitset::const_view& left, const bitset::const_view& right) {
  bitset bs(left);
  return bs |= right;
}

bitset operator^(const bitset::const_view& left, const bitset::const_view& right) {
  bitset bs(left);
  return bs ^= right;
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

std::ostream& operator<<(std::ostream& out, const bitset& bs) {
  for (auto el : bs) {
    out << el;
  }
  return out;
}

std::string to_string(const bitset& bs) {
  std::stringstream ss;
  for (auto b : bs) {
    ss << b;
  }
  return ss.str();
}

void swap(bitset& lhs, bitset& rhs) {
  lhs.swap(rhs);
}
