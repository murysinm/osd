#pragma once
#include <span>
#include <vector>

class combination {
public:
  combination(size_t k, size_t n);

  std::span<const size_t> get() const noexcept;

  bool next() noexcept;

private:
  std::vector<size_t> data;
};
