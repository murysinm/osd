#include "linalg.h"

#include <cassert>
#include <stdexcept>

vec2& vec2::operator+=(const vec2& other) noexcept {
  assert(size() == other.size());
  for (int i = 0; i < size(); ++i) {
    (*this)[i] = (*this)[i] ^ other[i];
  }
  return *this;
}

vec2 operator+(const vec2& lhs, const vec2& rhs) {
  vec2 res = lhs;
  res += rhs;
  return res;
}

vec2 vec2::from_string(const std::string& s) {
  vec2 res;
  res.reserve(s.size());
  for (char c : s) {
    if (c != '0' && c != '1') {
      throw std::runtime_error("String must contain only zeros and ones");
    }
    res.push_back(c == '1');
  }
  return res;
}

vec2 mul(const vec2& v, const matrix& m) {
  assert(v.size() == m.size());
  assert(v.size() != 0);
  vec2 result(m[0].size(), false);

  for (int i = 0; i < m.size(); ++i) {
    if (v[i] == true) {
      result += m[i];
    }
  }
  return result;
}

std::pair<matrix, std::vector<size_t>> gauss(matrix m) {
  assert(m.size() != 0);
  std::vector<size_t> inf_set;
  for (size_t j = 0; j < m[0].size(); ++j) {
    size_t ein_index = -1;
    for (size_t i = inf_set.size(); i < m.size(); ++i) {
      if (m[i][j] == true) {
        ein_index = i;
        break;
      }
    }
    if (ein_index == -1) {
      continue;
    }
    if (ein_index != inf_set.size()) {
      m[inf_set.size()] += m[ein_index];
    }
    assert(m[inf_set.size()][j] == true);
    for (size_t i = 0; i < m.size(); ++i) {
      if (i == inf_set.size() || m[i][j] == false) {
        continue;
      }
      m[i] += m[inf_set.size()];
      assert(m[i][j] == false);
    }
    inf_set.push_back(j);
  }
  assert(inf_set.size() == m.size());
  return {std::move(m), std::move(inf_set)};
}
