#pragma once
#include "linalg.h"

using perm_t = std::vector<size_t>;

template <typename Container>
Container make_permutation(const perm_t& perm, const Container& v) {
  Container res(v.size());
  for (size_t i = 0; i < v.size(); ++i) {
    res[perm[i]] = v[i];
  }
  return res;
}

std::vector<size_t> invert_permutation(const perm_t& perm);

matrix shuffle_cols(const matrix& m, const perm_t& perm);
