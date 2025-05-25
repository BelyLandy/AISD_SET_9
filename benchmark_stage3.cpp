// benchmark_stage3.cpp

#include "StringGenerator.hpp"
#include "StringSortTester.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

int main() {
  constexpr int REPEATS = 10;
  StringGenerator gen(42);
  StringSortTester tester;

  std::ofstream csv("results_stage3.csv");
  if (!csv) {
    std::cerr << "Не удалось открыть results_stage3.csv для записи\n";
    return 1;
  }
  csv << "type,n,algo,time_us,char_cmp\n";

  struct Case {
    const char* name;
    std::function<std::vector<std::string>(int)> make;
  };
  const std::vector<Case> cases = {
    { "random",  [&gen](int n) { return gen.randomArray(n); } },
    { "reverse", [&gen](int n) { return gen.reverseSortedArray(n); } },
    { "almost",  [&gen](int n) { return gen.almostSortedArray(n); } }
  };

  struct AlgoRec {
    const char* tag;
    StringSortTester::Algo id;
  };
  const std::vector<AlgoRec> algos = {
    { "std::sort", StringSortTester::Algo::StdQuick },
    { "stable_sort", StringSortTester::Algo::StdMerge },
    { "ternary_qs", StringSortTester::Algo::TernaryQuick },
    { "lcp_merge", StringSortTester::Algo::LcpMerge },
    { "msd_radix", StringSortTester::Algo::MsdRadix },
    { "radix+qs", StringSortTester::Algo::MsdRadixQuick }
  };

  for (const auto& c : cases) {
    for (int n = 100; n <= 3000; n += 100) {
      auto sample = c.make(n);

      for (const auto& a : algos) {
        auto m = tester.measure(sample, a.id, REPEATS);
        csv << c.name << ',' << n << ',' << a.tag << ',' << m.avgMicroSeconds << ',' << m.avgCharCompares << '\n';
      }
    }
  }

  std::cout << "Данные сохранены в results_stage3.csv\n";

  return 0;
}