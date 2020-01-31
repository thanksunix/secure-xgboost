#pragma once

#include <xgboost/logging.h>
#include <cstddef>
#include <cstdint>
#include <type_traits>

//----------------------------------------------------------------------------
// Interface

constexpr size_t CACHE_LINE_SIZE = 64;

template <typename T>
inline bool ObliviousLess(const T &x, const T &y);

template <typename T>
inline bool ObliviousLessOrEqual(const T &x, const T &y);

template <typename T>
inline bool ObliviousGreater(const T &x, const T &y);

template <typename T>
inline bool ObliviousGreaterOrEqual(const T &x, const T &y);

template <typename T,
          typename std::enable_if<std::is_scalar<T>::value, int>::type = 0>
inline bool ObliviousEqual(T x, T y);

template <typename T, typename std::enable_if<std::is_standard_layout<T>::value,
                                              int>::type = 0>
inline void ObliviousAssign(bool pred, const T &t_val, const T &f_val, T *out);

template <typename T>
inline T ObliviousChoose(bool pred, const T &t_val, const T &f_val);

template <typename Iter>
inline void ObliviousMerge(Iter begin, Iter end);

template <typename Iter>
inline void ObliviousSort(Iter begin, Iter end);

template <typename Iter, typename Comparator>
inline void ObliviousMerge(Iter begin, Iter end, Comparator cmp);

template <typename Iter, typename Comparator>
inline void ObliviousSort(Iter begin, Iter end, Comparator cmp);

template <typename T>
inline T ObliviousArrayAccess(const T *arr, size_t i, size_t n);

template <typename T>
inline void ObliviousArrayAssign(T *arr, size_t i, size_t n, const T &val);

void ObliviousBytesAssign(bool pred, size_t nbytes, const void *t_val,
                          const void *f_val, void *out);

// Impl.

namespace obl {

template <typename T,
          typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
inline bool LessImpl(T x, T y) {
  bool result;
  __asm__ volatile(
      "cmp %2, %1;"
      "setl %0;"
      : "=r"(result)
      : "r"(x), "r"(y)
      : "cc");
  return result;
}

bool LessImplDouble(double x, double y);

template <typename T, typename std::enable_if<std::is_floating_point<T>::value,
                                              int>::type = 0>
inline bool LessImpl(T x, T y) {
  return LessImplDouble(x, y);
}

template <typename T>
struct less {
  bool operator()(const T &a, const T &b) { return LessImpl<T>(a, b); }
};

template <typename T, typename Comparator>
struct reverse_cmp {
  bool operator()(const T &a, const T &b) {
    Comparator cmp;
    return cmp(b, a);
  }
};

template <typename T>
struct greater {
  bool operator()(const T &a, const T &b) { return less<T>()(b, a); }
};

template <typename T>
struct greater_equal {
  bool operator()(const T &a, const T &b) { return !less<T>()(a, b); }
};

template <typename T>
struct less_equal {
  bool operator()(const T &a, const T &b) { return !greater<T>()(a, b); }
};

}  // namespace obl

template <typename T>
inline bool ObliviousLess(const T &x, const T &y) {
  return obl::less<T>()(x, y);
}

template <typename T>
inline bool ObliviousGreater(const T &x, const T &y) {
  return obl::greater<T>()(x, y);
}

template <typename T>
inline bool ObliviousGreaterOrEqual(const T &x, const T &y) {
  return obl::greater_equal<T>()(x, y);
}

template <typename T>
inline bool ObliviousLessOrEqual(const T &x, const T &y) {
  return obl::less_equal<T>()(x, y);
}

template <typename T,
          typename std::enable_if<std::is_scalar<T>::value, int>::type>
inline bool ObliviousEqual(T x, T y) {
  bool result;
  __asm__ volatile(
      "cmp %2, %1;"
      "sete %0;"
      : "=r"(result)
      : "r"(x), "r"(y)
      : "cc");
  return result;
}

template <typename T,
          typename std::enable_if<std::is_standard_layout<T>::value, int>::type>
inline void ObliviousAssign(bool pred, const T &t_val, const T &f_val, T *out) {
  return ObliviousBytesAssign(pred, sizeof(T), &t_val, &f_val, out);
}

template <typename T>
inline T ObliviousChoose(bool pred, const T &t_val, const T &f_val) {
  T result;
  ObliviousAssign(pred, t_val, f_val, &result);
  return result;
}

template <typename Iter, typename Comparator>
inline void ObliviousMerge(Iter begin, Iter end, Comparator cmp);

template <typename Iter, typename Comparator>
inline void ObliviousSort(Iter begin, Iter end, Comparator cmp);

template <typename Iter>
inline void ObliviousMerge(Iter begin, Iter end) {
  using value_type = typename std::remove_reference<decltype(*begin)>::type;
  return ObliviousMerge(begin, end, ::obl::less<value_type>());
}

template <typename Iter>
inline void ObliviousSort(Iter begin, Iter end) {
  using value_type = typename std::remove_reference<decltype(*begin)>::type;
  return ObliviousSort(begin, end, ::obl::less<value_type>());
}

// Return arr[i]
template <typename T>
inline T ObliviousArrayAccess(const T *arr, size_t i, size_t n) {
  T result = arr[0];
  size_t step = sizeof(T) < CACHE_LINE_SIZE ? CACHE_LINE_SIZE / sizeof(T) : 1;
  for (size_t j = 0; j < n; j += step) {
    bool cond = ObliviousEqual(j / step, i / step);
    int pos = ObliviousChoose(cond, i, j);
    result = ObliviousChoose(cond, arr[pos], result);
  }
  return result;
}

// Set arr[i] = val
template <typename T>
inline void ObliviousArrayAssign(T *arr, size_t i, size_t n, const T& val) {
  size_t step = sizeof(T) < CACHE_LINE_SIZE ? CACHE_LINE_SIZE / sizeof(T) : 1;
  for (size_t j = 0; j < n; j += step) {
    bool cond = ObliviousEqual(j / step, i / step);
    int pos = ObliviousChoose(cond, i, j);
    arr[pos] = ObliviousChoose(cond, val, arr[pos]);
  }
}

namespace detail {

inline uint32_t greatest_power_of_two_less_than(uint32_t n) {
  uint32_t k = 1;
  while (k < n) k = k << 1;
  return k >> 1;
}

inline uint32_t log2_ceil(uint32_t n) {
  uint32_t k = 0;
  uint32_t _n = n;
  while (n > 1) {
    k++;
    n /= 2;
  }
  if ((1 << k) < _n) k++;
  return k;
}

// Imperative implementation of bitonic merge network
template <typename T, typename Comparator>
inline void imperative_o_merge(T *arr, uint32_t low, uint32_t len,
                               Comparator cmp) {
  uint32_t i, j, k;
  uint32_t l = log2_ceil(len);
  uint32_t n = 1 << l;
  for (i = 0; i < l; i++) {
    for (j = 0; j<n; j += n>> i) {
      for (k = 0; k < (n >> i) / 2; k++) {
        uint32_t i1 = low + k + j;
        uint32_t i2 = i1 + (n >> i) / 2;
        if (i2 >= low + len) break;
        bool pred = cmp(arr[i2], arr[i1]);
        // These array accesses are oblivious because the indices are
        // deterministic
        T tmp = arr[i1];
        arr[i1] = ObliviousChoose(pred, arr[i2], arr[i1]);
        arr[i2] = ObliviousChoose(pred, tmp, arr[i2]);
      }
    }
  }
}

// Imperative implementation of bitonic sorting network -- works only for powers
// of 2
template <typename T, typename Comparator>
inline void imperative_o_sort(T *arr, size_t n, Comparator cmp) {
  uint32_t i, j, k;
  for (k = 2; k <= n; k = 2 * k) {
    for (j = k >> 1; j > 0; j = j >> 1) {
      for (i = 0; i < n; i++) {
        uint32_t ij = i ^ j;
        if (ij > i) {
          if ((i & k) == 0) {
            bool pred = cmp(arr[ij], arr[i]);
            // These array accesses are oblivious because the indices are
            // deterministic
            T tmp = arr[i];
            arr[i] = ObliviousChoose(pred, arr[ij], arr[i]);
            arr[ij] = ObliviousChoose(pred, tmp, arr[ij]);
          } else {
            bool pred = cmp(arr[i], arr[ij]);
            // These array accesses are oblivious because the indices are
            // deterministic
            T tmp = arr[i];
            arr[i] = ObliviousChoose(pred, arr[ij], arr[i]);
            arr[ij] = ObliviousChoose(pred, tmp, arr[ij]);
          }
        }
      }
    }
  }
}

// Sort <len> elements in arr -- starting from index arr[low]
template <typename T, typename Comparator>
inline void o_sort(T *arr, uint32_t low, uint32_t len, Comparator cmp) {
  if (len > 1) {
    uint32_t m = greatest_power_of_two_less_than(len);
    if (m * 2 == len) {
      imperative_o_sort(arr + low, len, cmp);
    } else {
      imperative_o_sort(arr + low, m, obl::reverse_cmp<T, Comparator>());
      o_sort(arr, low + m, len - m, cmp);
      imperative_o_merge(arr, low, len, cmp);
    }
  }
}

}  // namespace detail

template <typename Iter, typename Comparator>
inline void ObliviousMerge(Iter begin, Iter end, Comparator cmp) {
  using value_type = typename std::remove_reference<decltype(*begin)>::type;
  value_type *array = &(*begin);
  return detail::imperative_o_merge<value_type, Comparator>(array, 0,
                                                            end - begin, cmp);
}

template <typename Iter, typename Comparator>
inline void ObliviousSort(Iter begin, Iter end, Comparator cmp) {
  using value_type = typename std::remove_reference<decltype(*begin)>::type;
  value_type *array = &(*begin);
  return detail::o_sort<value_type, Comparator>(array, 0, end - begin, cmp);
}
