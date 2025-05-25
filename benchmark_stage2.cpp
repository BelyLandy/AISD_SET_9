// benchmark_stage2.cpp

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

  std::ofstream csv("results.csv");
  
  if (!csv) {
    std::cerr << "Не удалось открыть results.csv для записи\n";
    return 1;
  }
  
  csv << "type,n,algo,time_us,char_cmp\n";

  struct Case {
    const char* name;
    std::function<std::vector<std::string>(int)> make;
  };

  const std::vector<Case> cases = {
    { "random",
      [&gen](int n) { return gen.randomArray(n); } },
    { "reverse",
      [&gen](int n) { return gen.reverseSortedArray(n); } },
    { "almost",
      [&gen](int n) { return gen.almostSortedArray(n); } }
  };

  for (const auto& c : cases) {
    for (int n = 100; n <= 3000; n += 100) {
      auto sample = c.make(n);

      auto quick = tester.measure(sample, StringSortTester::Algo::QuickSort, REPEATS);
      csv << c.name << ',' << n << ",std::sort," << quick.avgMicroSeconds << ',' << quick.avgCharCompares << '\n';

      auto merge = tester.measure(sample, StringSortTester::Algo::MergeSort, REPEATS);
      csv << c.name << ',' << n << ",stable_sort," << merge.avgMicroSeconds << ',' << merge.avgCharCompares << '\n';
    }
  }

  std::cout << "Данные сохранены в results.csv\n";
  
  return 0;
}