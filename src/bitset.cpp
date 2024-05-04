#include "bitset.h"

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
    _data = new uint32_t[size];
    set_bit(value);
  }
}

bitset::bitset(const bitset& other)
    : _size(other._size)
    , _capacity(other._capacity)
    , _data(nullptr) {
  if (_capacity > 0) {
    _data = new uint32_t[_size];
     std::generate(begin(),
                   end(),
                   [oit = other.begin()]() mutable {return *(oit++);});
  }
}

bitset::bitset(const_iterator first, const_iterator last)
    : _size(last - first)
    , _capacity(get_capacity(_size))
    , _data(nullptr) {
  if (_capacity > 0) {
    _data = new uint32_t[_capacity];
    std::generate(begin(),
                  end(),
                  [oit = first] () mutable {return *(oit++);});
  }
}

bitset::bitset(std::string_view str)
    : _size(str.size())
    , _capacity(get_capacity(_size))
    , _data(nullptr) {
  if (_capacity > 0) {
    _data = new uint32_t[_capacity];
    std::generate(begin(),
                  end(),
                  [oit = str.begin()] () mutable {return *(oit++) == '1';});
  }
}

bitset& bitset::operator=(const bitset& other) & {
  if (this != &other) {
    bitset copy(other);
    swap(copy);
  }
  return *this;
}

bitset& bitset::operator=(std::string_view str) & {
  _size = str.size();
  _capacity = get_capacity(_size);
  _data = nullptr;
  if (_capacity > 0) {
    _data = new uint32_t[_capacity];
    std::generate(begin(),
                  end(),
                  [oit = str.begin()] () mutable {return *(oit++) == '1';});
  }
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

bitset::reference bitset::operator[] (std::size_t index) {
  return *(begin() + index);
}

bitset::const_reference bitset::operator[] (std::size_t index) const {
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

void bitset::flip() & {
  std::for_each(begin(), end(), [](reference el) {el.flip();});
}

bitset& bitset::set() & {
  return set_bit(true);
}

bitset& bitset::reset() & {
  return set_bit(false);
}

bitset& bitset::set_bit(bool value) {
  std::fill(begin(), end(), value);
  return *this;
}

size_t bitset::get_capacity(size_t size) {
  return (size + bitset::INT_SIZE - 1) / bitset::INT_SIZE;
}

void bitset::swap(bitset& other) noexcept {
  std::swap(_size, other._size);
  std::swap(_capacity, other._capacity);
  std::swap(_data, other._data);
}

bool bitset::all() const {
  return std::all_of(begin(), end(), [](bool el) {return el;});
}

bool bitset::any() const {
  return std::any_of(begin(), end(), [](bool el) {return el;});
}

std::size_t bitset::count() const {
  return std::count(begin(), end(), true);
}

bool operator==(const bitset& left, const bitset& right) {
  return left.size() == right.size() && std::equal(left.begin(), left.end(), right.begin());
}

bool operator!=(const bitset& left, const bitset& right) {
  return !(left == right);
}

void swap(bitset& lhs, bitset& rhs) noexcept {
  lhs.swap(rhs);
}

std::string to_string(const bitset& bs) {
  std::stringstream ss;
  for (auto b : bs) {
    ss << b;
  }
  return ss.str();
}
