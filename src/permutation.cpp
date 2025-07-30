#include "permutation.h"

std::vector<size_t> invert_permutation(const perm_t& perm) {
  std::vector<size_t> result(perm.size());
  for (size_t i = 0; i < perm.size(); ++i) {
    result[perm[i]] = i;
  }
  return result;
}

matrix shuffle_cols(const matrix& m, const perm_t& perm) {
  if (m.empty()) {
    return m;
  }
  matrix result(m.size());
  for (int i = 0; i < m.size(); ++i) {
    result[i] = make_permutation(perm, m[i]);
  }
  return result;
}
