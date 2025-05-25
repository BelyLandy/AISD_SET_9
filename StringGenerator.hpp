#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <chrono>
#include <cstdint>

class StringGenerator {
public:
  explicit StringGenerator(std::uint32_t seed =
    static_cast<std::uint32_t>(
      std::chrono::steady_clock::now().time_since_epoch().count()))
    : _rng(seed),
    _lenDist(10, 200),
    _charDist(0, AlphabetSize - 1)
  {
    buildBaseVectors();
  }

  std::vector<std::string> randomArray(int size) const { 
    return slice(_randomBase, size);
  }

  std::vector<std::string> reverseSortedArray(int size) const { 
    return slice(_reverseBase, size);
  }

  std::vector<std::string> almostSortedArray(int size, std::size_t swapPairs = 0) const
  {
    std::vector<std::string> result = slice(_sortedBase, size);
    if (swapPairs == 0) {
      swapPairs = static_cast<std::size_t>(result.size() * 0.02);
    }

    std::uniform_int_distribution<int> idxDist(0, static_cast<int>(result.size() - 1));

    for (std::size_t k = 0; k < swapPairs; ++k) {
      int i = idxDist(_rng), j = idxDist(_rng);

      if (i != j) {
        std::swap(result[i], result[j]);
      }
    }

    return result;
  }

  std::vector<std::string> randomArrayWithPrefix(int size, int prefixLen) const
  {
    std::vector<std::string> out;

    out.reserve(size);
    std::string common = randomString(prefixLen);

    for (int i = 0; i < size; ++i) {
      std::size_t len = std::max<std::size_t>(prefixLen + 1, _lenDist(_rng));
      std::string tail = randomString(len - prefixLen);

      out.push_back(common + tail);
    }

    return out;
  }

private:
  static constexpr char kAlphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "!@#%:;^&*()-.";

  static constexpr int AlphabetSize = static_cast<int>(sizeof(kAlphabet) - 1);

  mutable std::mt19937 _rng;
  mutable std::uniform_int_distribution<std::size_t> _lenDist;
  mutable std::uniform_int_distribution<int>  _charDist;

  std::vector<std::string> _randomBase;
  std::vector<std::string> _sortedBase;
  std::vector<std::string> _reverseBase;

  std::string randomString(std::size_t len) const {
    std::string s;

    s.reserve(len);

    for (std::size_t i = 0; i < len; ++i) {
      s.push_back(kAlphabet[_charDist(_rng)]);
    }

    return s;
  }

  static std::vector<std::string> slice(const std::vector<std::string>& src, int size) {
    return { src.begin(), src.begin() + size };
  }

  void buildBaseVectors() {
    const int N = 3000;

    _randomBase.reserve(N);

    for (int i = 0; i < N; ++i) {
      _randomBase.push_back(randomString(_lenDist(_rng)));
    }

    _sortedBase = _randomBase;
    std::sort(_sortedBase.begin(), _sortedBase.end());

    _reverseBase = _sortedBase;
    std::reverse(_reverseBase.begin(), _reverseBase.end());
  }
};