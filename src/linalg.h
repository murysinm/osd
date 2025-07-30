#pragma once
#include <string>
#include <vector>

class vec2 : public std::vector<bool> {
public:
  using std::vector<bool>::vector;

  vec2& operator+=(const vec2& other) noexcept;

  friend vec2 operator+(const vec2& lhs, const vec2& rhs);

  static vec2 from_string(const std::string& s);
};

using matrix = std::vector<vec2>;

vec2 mul(const vec2& v, const matrix& m);

std::pair<matrix, std::vector<size_t>> gauss(matrix m);
