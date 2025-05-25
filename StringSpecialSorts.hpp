#include <vector>
#include <string>
#include <algorithm>
#include <cstddef>

namespace sps {

  inline int charAt(const std::string& s, int d, std::size_t* cmp = nullptr)
  {
    if (cmp) ++(*cmp);
    return d < static_cast<int>(s.size())
      ? static_cast<unsigned char>(s[d])
      : -1;
  }

  inline void insertionSuffix(std::vector<std::string>& a,
    int lo, int hi, int d, std::size_t* cmp)
  {
    for (int i = lo + 1; i <= hi; ++i) {
      std::string key = std::move(a[i]);
      int j = i - 1;
      while (j >= lo && a[j].compare(d, std::string::npos,
        key, d, std::string::npos) > 0)
      {
        a[j + 1] = std::move(a[j]);
        --j;
      }
      a[j + 1] = std::move(key);
    }
  }

  // =====================================================
  // 1. String QuickSort
  // =====================================================
  inline void quickSort3(std::vector<std::string>& a,
    int lo, int hi, int d,
    std::size_t* cmp = nullptr)
  {
    if (hi <= lo) return;
    if (hi - lo < 10) { insertionSuffix(a, lo, hi, d, cmp); return; }

    int lt = lo, gt = hi;
    int v = charAt(a[lo], d, cmp);
    int i = lo + 1;
    while (i <= gt) {
      int t = charAt(a[i], d, cmp);
      if (t < v)    std::swap(a[lt++], a[i++]);
      else if (t > v)   std::swap(a[i], a[gt--]);
      else        ++i;
    }
    quickSort3(a, lo, lt - 1, d, cmp);
    if (v >= 0) quickSort3(a, lt, gt, d + 1, cmp);
    quickSort3(a, gt + 1, hi, d, cmp);
  }

  // =====================================================
  // 2. String MergeSort
  // =====================================================
  inline int lcpCompare(const std::string& a, const std::string& b,
    std::size_t* cmp)
  {
    std::size_t len = std::min(a.size(), b.size());
    std::size_t i = 0;
    while (i < len && a[i] == b[i]) { if (cmp) ++(*cmp); ++i; }
    if (i == len) return (a.size() < b.size()) ? -1 : (a.size() > b.size());
    if (cmp) ++(*cmp);
    return (a[i] < b[i]) ? -1 : 1;
  }
  inline bool lcpLess(const std::string& a, const std::string& b,
    std::size_t* cmp) {
    return lcpCompare(a, b, cmp) < 0;
  }

  inline void mergeSort(std::vector<std::string>& v, std::vector<std::string>& buf,
    int L, int R, std::size_t* cmp = nullptr)
  {
    if (R - L <= 1) return;
    int M = (L + R) >> 1;
    mergeSort(v, buf, L, M, cmp);
    mergeSort(v, buf, M, R, cmp);
    int i = L, j = M, k = L;
    while (i < M && j < R)
      buf[k++] = lcpLess(v[i], v[j], cmp) ? std::move(v[i++]) : std::move(v[j++]);
    while (i < M) buf[k++] = std::move(v[i++]);
    while (j < R) buf[k++] = std::move(v[j++]);
    for (int t = L; t < R; ++t) v[t] = std::move(buf[t]);
  }

  // =====================================================
  // 3. MSD Radix Sort
  // =====================================================
  inline void msdRadix(std::vector<std::string>& a,
    std::vector<std::string>& aux,
    int lo, int hi, int d,
    std::size_t* cmp = nullptr)
  {
    constexpr int R = 256;
    if (hi <= lo) return;
    if (hi - lo < 15) { insertionSuffix(a, lo, hi, d, cmp); return; }

    int freq[R + 2] = { 0 };
    for (int i = lo; i <= hi; ++i) ++freq[charAt(a[i], d, cmp) + 2];
    for (int r = 0; r < R + 1; ++r) freq[r + 1] += freq[r];
    for (int i = lo; i <= hi; ++i)
      aux[lo + freq[charAt(a[i], d, cmp) + 1]++] = std::move(a[i]);
    for (int i = lo; i <= hi; ++i) a[i] = std::move(aux[i]);

    int start = 0;
    for (int r = 0; r < R; ++r) {
      int cntR = freq[r + 1] - freq[r];
      if (cntR > 1) msdRadix(a, aux, lo + start, lo + start + cntR - 1, d + 1, cmp);
      start += cntR;
    }
  }

  // =====================================================
  // 4. MSD Radix + QuickSort
  // =====================================================
  inline void msdRadixQS(std::vector<std::string>& a,
    std::vector<std::string>& aux,
    int lo, int hi, int d,
    std::size_t* cmp = nullptr)
  {
    constexpr int R = 256;
    constexpr int SWITCH_THRESHOLD = 74;

    if (hi <= lo) return;

    if (hi - lo + 1 < SWITCH_THRESHOLD) {
      quickSort3(a, lo, hi, d, cmp);
      return;
    }

    int freq[R + 2] = { 0 };
    for (int i = lo; i <= hi; ++i) ++freq[charAt(a[i], d, cmp) + 2];
    int pos[R + 2]; pos[0] = 0;
    for (int r = 1; r < R + 2; ++r) pos[r] = pos[r - 1] + freq[r - 1];
    for (int i = lo; i <= hi; ++i) {
      int idx = charAt(a[i], d, cmp) + 2;
      aux[lo + pos[idx]] = std::move(a[i]);
      ++pos[idx];
    }
    for (int i = lo; i <= hi; ++i) a[i] = std::move(aux[i]);

    int start = 0;
    for (int r = 1; r < R + 2; ++r) {
      int cnt = freq[r];
      if (cnt > 1 && r > 1)
        msdRadixQS(a, aux, lo + start, lo + start + cnt - 1, d + 1, cmp);
      start += cnt;
    }
  }

}
