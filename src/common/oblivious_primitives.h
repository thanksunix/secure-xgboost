#include <algorithm>

/// TODO: change to generic oblivious implementation.
/// The assembly `cmovz` only handles 32/64 bit. How do we make this generic ?

template <class Iterator>
void BitonicSort(Iterator first, Iterator last) {
  return std::stable_sort(first, last);
}

template <class Iterator>
void BitonicMerge(Iterator first, Iterator last) {
  return std::stable_sort(first, last);
}

template <class T>
void ObliviousAssign(bool pred, const T& t_val, const T& f_val, T* out) {
  *out = pred ? t_val : f_val;
}

template <class T>
T ObliviousChoose(bool pred, const T& t_val, const T& f_val) {
  T result;
  ObliviousAssign(pred, t_val, f_val, &result);
  return result;
}

template <class T>
bool ObliviousGreater(const T& lhs, const T& rhs) {
  return lhs > rhs;
}

template <class T>
bool ObliviousGreaterOrEqual(const T& lhs, const T& rhs) {
  return lhs >= rhs;
}

template <class T>
bool ObliviousEqual(const T& lhs, const T& rhs) {
  return lhs == rhs;
}

template <class T>
bool ObliviousLess(const T& lhs, const T& rhs) {
  return lhs < rhs;
}
