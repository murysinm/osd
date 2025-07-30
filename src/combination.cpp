#include "combination.h"

combination::combination(size_t k, size_t n) {
  data.reserve(k + 1);
  for (int i = 0; i < k; ++i) {
    data.push_back(i);
  }
  data.push_back(n);
}

std::span<const size_t> combination::get() const noexcept {
  return {data.begin(), std::prev(data.end())};
}

bool combination::next() noexcept {
  size_t increment_index = data.size() - 2;
  while (increment_index != -1 && data[increment_index] + 1 == data[increment_index + 1]) {
    --increment_index;
  }
  if (increment_index == -1) {
    return false;
  }
  ++data[increment_index];
  for (size_t i = increment_index + 1; i < data.size() - 1; ++i) {
    data[i] = data[i - 1] + 1;
  }
  return true;
}
