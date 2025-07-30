#pragma once
#include "linalg.h"

#include <random>

template <typename Rng>
vec2 generate_vec(size_t size, Rng& rng) {
  std::uniform_int_distribution<> distr(0, 1);
  vec2 result;
  result.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    result.emplace_back(distr(rng) == 1);
  }
  return result;
}

template <typename Rng>
std::vector<double> generate_noise(double mean, double std, size_t size, Rng& rng) {
  std::normal_distribution<> distr(mean, std);
  std::vector<double> result;
  result.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    result.emplace_back(distr(rng));
  }
  return result;
}

template <typename Rng>
std::vector<double> simulate_translation(const vec2& vec, double mean, double std, Rng& rng) {
  auto noise = generate_noise(mean, std, vec.size(), rng);
  std::vector<double> msg(vec.size(), 0);
  for (int i = 0; i < msg.size(); ++i) {
    msg[i] = vec[i] == true ? -1 : 1;
    msg[i] += noise[i];
  }
  return msg;
}