#include "osd.h"

#include "combination.h"
#include "permutation.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <ranges>

namespace {

template <typename Container, std::ranges::range R>
auto to(R&& r) {
  Container v;

  // if we can get a size, reserve that much
  if constexpr (requires { std::ranges::size(r); }) {
    v.reserve(std::ranges::size(r));
  }

  // push all the elements
  for (auto&& e : r) {
    v.push_back(static_cast<decltype(e)&&>(e));
  }

  return v;
}

}

std::pair<vec2, std::vector<double>> decompose(const std::vector<double>& msg) {
  auto alpha = to<vec2>(msg | std::views::transform([](const auto& val) { return val < 0; }));
  auto beta = to<std::vector<double>>(msg | std::views::transform([](const auto& val) { return std::abs(val); }));
  return {alpha, beta};
}

double calc_metric(const vec2& alpha, const std::vector<double>& beta, const vec2& x) noexcept {
  assert(alpha.size() == beta.size() && beta.size() == x.size());
  double metric = 0;
  for (size_t i = 0; i < alpha.size(); ++i) {
    if (alpha[i] != x[i]) {
      metric += beta[i];
    }
  }
  return metric;
}

vec2 osd(const std::vector<double>& msg, const matrix& g, size_t w_max) {
  auto [alpha, beta] = decompose(msg);

  std::vector<std::pair<size_t, double>> indexed;
  indexed.reserve(beta.size());
  for (size_t i = 0; i < beta.size(); ++i) {
    indexed.emplace_back(i, beta[i]);
  }

  std::ranges::sort(indexed, [](auto&& fi, auto&& sec) {
    auto&& [i1, el1] = fi;
    auto&& [i2, el2] = sec;
    return el1 > el2;
  });

  auto inverted_pi = to<std::vector<size_t>>(indexed | std::views::transform([](auto&& p) { return std::get<0>(p); }));

  auto pi = invert_permutation(inverted_pi);

  alpha = make_permutation(pi, alpha);
  beta = make_permutation(pi, beta);
  auto shuffled_g = shuffle_cols(g, pi);

  auto [g_gamma, inform_indices] = gauss(shuffled_g);

  assert(inform_indices.size() == g.size());
  vec2 inf_set(g.size());
  for (int i = 0; i < g.size(); ++i) {
    inf_set[i] = true && alpha[inform_indices[i]];
  }

  double best_metric = std::numeric_limits<double>::max();
  vec2 best_code;

  for (size_t w = 0; w <= w_max; ++w) {
    auto c = combination(w, inf_set.size());
    do {
      auto flip_indices = c.get();

      for (auto&& i : flip_indices) {
        inf_set[i] = !inf_set[i];
      }
      auto x = mul(inf_set, g_gamma);
      auto cur_metric = calc_metric(alpha, beta, x);
      if (cur_metric < best_metric) {
        best_code = x;
        best_metric = cur_metric;
      }

      for (auto&& i : flip_indices) {
        inf_set[i] = !inf_set[i];
      }
    } while (c.next());
  }
  best_code = make_permutation(inverted_pi, best_code);
  return best_code;
}
