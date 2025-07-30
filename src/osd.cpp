#include "osd.h"

#include "combination.h"
#include "permutation.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <ranges>

std::pair<vec2, std::vector<double>> decompose(const std::vector<double>& msg) {
  auto alpha = msg | std::views::transform([](const auto& val) { return val < 0; }) | std::ranges::to<vec2>();
  auto beta = msg | std::views::transform([](const auto& val) { return std::abs(val); }) |
              std::ranges::to<std::vector<double>>();
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

  auto indexed = std::ranges::views::enumerate(beta) | std::ranges::to<std::vector>();

  std::ranges::sort(indexed, [](auto&& fi, auto&& sec) {
    auto&& [i1, el1] = fi;
    auto&& [i2, el2] = sec;
    return el1 > el2;
  });

  auto inverted_pi = indexed | std::views::transform([](auto&& p) { return static_cast<size_t>(std::get<0>(p)); }) |
                     std::ranges::to<std::vector>();

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
