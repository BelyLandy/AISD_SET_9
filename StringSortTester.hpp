#include "StringSpecialSorts.hpp"

#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <numeric>

class StringSortTester {
public:
  enum class Algo {
    StdQuick,    // std::sort.
    StdMerge,    // std::stable_sort.
    TernaryQuick,  // трёхсторонний QS.
    LcpMerge,    // merge-sort с LCP.
    MsdRadix,    // MSD-radix.
    MsdRadixQuick  // MSD + QS.
  };

  struct Metrics {
    double avgMicroSeconds;
    std::size_t avgCharCompares;
  };

  Metrics measure(const std::vector<std::string>& input, Algo algo, int repeats = 10) const
  {
    std::vector<double> times;
    std::vector<std::size_t> comps;

    times.reserve(repeats);
    comps.reserve(repeats);

    for (int r = 0; r < repeats; ++r) {
      std::vector<std::string> data = input;
      std::size_t counter = 0;

      auto t0 = Clock::now();
      switch (algo)
      {
      case Algo::StdQuick:
        std::sort(data.begin(), data.end(),
          [&counter](const std::string& a,
            const std::string& b) {
              return lexCmp(a, b, counter);
          });

        break;

      case Algo::StdMerge:
        std::stable_sort(data.begin(), data.end(),
          [&counter](const std::string& a,
            const std::string& b) {
              return lexCmp(a, b, counter);
          });

        break;

      case Algo::TernaryQuick:
        sps::quickSort3(data, 0, static_cast<int>(data.size()) - 1, 0, &counter);

        break;

      case Algo::LcpMerge: {
        std::vector<std::string> buf(data.size());
        sps::mergeSort(data, buf, 0, static_cast<int>(data.size()), &counter);

        break;
      }

      case Algo::MsdRadix: {
        std::vector<std::string> aux(data.size());
        sps::msdRadix(data, aux, 0, static_cast<int>(data.size()) - 1, 0, &counter);

        break;
      }

      case Algo::MsdRadixQuick: {
        std::vector<std::string> aux(data.size());
        sps::msdRadixQS(data, aux, 0, static_cast<int>(data.size()) - 1, 0, &counter);

        break;
      }
      }

      auto t1 = Clock::now();

      double us = std::chrono::duration<double, std::micro>(t1 - t0).count();

      times.push_back(us);
      comps.push_back(counter);
    }

    return { mean(times), mean(comps) };
  }

private:
  using Clock = std::chrono::high_resolution_clock;

  static bool lexCmp(const std::string& a, const std::string& b, std::size_t& counter)
  {
    std::size_t len = std::min(a.size(), b.size());

    for (std::size_t i = 0; i < len; ++i) {
      ++counter;
      if (a[i] < b[i]) {
        return true;
      }

      if (a[i] > b[i]) {
        return false;
      }
    }

    return a.size() < b.size();
  }

  static double mean(const std::vector<double>& v) {
    return v.empty() ? 0.0 : std::accumulate(v.begin(), v.end(), 0.0) / v.size();
  }
  static std::size_t mean(const std::vector<std::size_t>& v) {
    if (v.empty()) {
      return 0;
    }

    std::size_t sum = std::accumulate(v.begin(), v.end(), std::size_t{ 0 });

    return (sum + v.size() / 2) / v.size();
  }
};
