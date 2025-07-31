#include "linalg.h"
#include "osd.h"
#include "simulation_utils.h"

#include <cxxopts.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {

double run_experiment(const matrix& g, double std, size_t w_max, size_t needed_errors, size_t count, size_t limit, bool verbose = false) {
  std::mt19937 rng(42);
  size_t k = g.size();
  size_t errors_count = 0;
  std::size_t i = 0;
  for (; i < limit && (i < count || errors_count < needed_errors) ; ++i) {
    auto original_message = generate_vec(k, rng);
    auto original_code = mul(original_message, g);

    auto noisy = simulate_translation(original_code, 0, std, rng);
    auto decoded = osd(noisy, g, w_max);
    if (decoded != original_code) {
      ++errors_count;
    }
    if (verbose && (i + 1) % 10000 == 0) {
      std::cout << std << ' ' << i + 1 << ' ' << errors_count << std::endl;
      std::cout << std::endl;
    }
  }
  return 1. * errors_count / i;
}

double calc_stddev(double ebno, size_t k, size_t n) {
    return std::sqrt(0.5 * n / k * std::pow(10, -ebno / 10));
}

matrix read_matrix_file(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }

  size_t cols, rows;
  file >> cols >> rows;

  matrix result;

  for (size_t i = 0; i < rows; ++i) {
    std::string line;
    file >> line;
    result.push_back(vec2::from_string(line));
  }

  return result;
}

}
int main(int argc, char* argv[]) {
  cxxopts::Options options("osd");
  options.set_width(80);
  options.add_options()
      ("f,file", "Place where generating matrix is stored", cxxopts::value<std::string>())
      ("w,wmax", "Maximum weight of error correcting combination", cxxopts::value<size_t>()->default_value("2"))
      ("n,EbN0", "E_b / N_0 ratio. This option can be an array: \"-n 1, 2, 3\" would run an experiment for each parameter", cxxopts::value<std::vector<double>>())
      ("e,errors", "Minimal count of errors to wait for", cxxopts::value<size_t>())
      ("c,count", "Count of words to translate for each experiment. If both \"errors\" and \"count\" are provided, experiment runs until both constraints are satisfied or limit is reached", cxxopts::value<size_t>())
      ("L,tries-limit", "Hard limit of tries. If limit is reached, experiment is stopped even if errors count isn't reached", cxxopts::value<size_t>())
      ("h,help", "Print usage");

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  if (result.count("file") == 0) {
    std::cout << "Generating matrix must be specified, but it isn't." << std::endl;
    std::cout << options.help() << std::endl;
    return 0;
  }

  if (result.count("EbN0") == 0) {
    std::cout << "Channel noise (EbN0 parameter) must be specified, but it isn't." << std::endl;
    std::cout << options.help() << std::endl;
    return 0;
  }

  if (result.count("errors") == 0 && result.count("count") == 0) {
    std::cout << "At least one of -e and -c must be provided." << std::endl;
    std::cout << options.help() << std::endl;
    return 0;
  }

  std::string filename = result["file"].as<std::string>();
  size_t w_max = result["wmax"].as<size_t>();
  std::vector<double> ebn0_vals = result["EbN0"].as<std::vector<double>>();

  size_t needed_errors = 0;
  size_t count = 0;

  if (result.count("errors") != 0) {
    needed_errors = result["errors"].as<size_t>();
  }

  if (result.count("count") != 0) {
    count = result["count"].as<size_t>();
  }

  std::size_t limit = -1;
  if (result.count("limit") != 0) {
    limit = result["limit"].as<size_t>();
  }

  matrix m = read_matrix_file(filename);

  auto local_calc_stddev = [&m](double ebn0) {
    return calc_stddev(ebn0, m.size(), m[0].size());
  };

  std::vector<double> wers(ebn0_vals.size(), -1);

  std::vector<std::thread> threads;
  threads.reserve(ebn0_vals.size());
  for (size_t i = 0; i < ebn0_vals.size(); ++i) {
    threads.emplace_back([i, &ebn0_vals, &local_calc_stddev, &m, &w_max, &needed_errors, &count, &limit, &wers] {
      auto std = local_calc_stddev(ebn0_vals[i]);
      double wer = run_experiment(m, std, w_max, needed_errors, count, limit);
      wers[i] = wer;
    });
  }
  for (auto& t: threads) {
    t.join();
  }
  std::cout << "E_b / N_0 (db) | Word/error rate" << std::endl;
  for (size_t i = 0; i < ebn0_vals.size(); ++i) {
    std::cout << std::left << std::setw(16) << ebn0_vals[i] << " " << wers[i] << '\n';
  }

}
