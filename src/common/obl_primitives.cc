#include "obl_primitives.h"

#define SIMULATED_OBL_ASSIGN

namespace obl {

// TODO: define this as inline cause segment fault. Need to know why. Is it due
// to |inline| does not work well with |assembly| impl?
bool LessImplDouble(double x, double y) {
  bool result;
  __asm__ volatile(
      "movsd %1, %%xmm0;"
      "movsd %2, %%xmm1;"
      "comisd %%xmm1, %%xmm0;"
      "setb %0;"
      : "=r"(result)
      : "m"(x), "m"(y)
      : "cc");
  return result;
}

template <typename T,
          typename std::enable_if<!std::is_same<T, uint8_t>::value &&
                                      std::is_scalar<T>::value,
                                  int>::type = 0>
inline void ObliviousAssignHelper(bool pred, T t_val, T f_val, T *out) {
#ifdef SIMULATED_OBL_ASSIGN
  *out = pred ? t_val : f_val;
#else
  T result;
  __asm__ volatile(
      "mov %2, %0;"
      "test %1, %1;"
      "cmovz %3, %0;"
      : "=&r"(result)
      : "r"(pred), "r"(t_val), "r"(f_val), "m"(out)
      : "cc");
  *out = result;
#endif
}

template <typename T, typename std::enable_if<std::is_same<T, uint8_t>::value,
                                              int>::type = 0>
inline void ObliviousAssignHelper(bool pred, T t_val, T f_val, T *out) {
#ifdef SIMULATED_OBL_ASSIGN
  *out = pred ? t_val : f_val;
#else
  uint16_t result;
  uint16_t t = t_val;
  uint16_t f = f_val;
  __asm__ volatile(
      "mov %2, %0;"
      "test %1, %1;"
      "cmovz %3, %0;"
      : "=&r"(result)
      : "r"(pred), "r"(t), "r"(f), "m"(out)
      : "cc");
  *out = static_cast<uint8_t>(result);
#endif
}

}  // namespace obl

void ObliviousBytesAssign(bool pred, size_t nbytes, const void *t_val,
                          const void *f_val, void *out) {
#ifdef SIMULATED_OBL_ASSIGN
  if (pred)
    std::memcpy(out, t_val, nbytes);
  else
    std::memcpy(out, f_val, nbytes);
#else

  const size_t bytes = nbytes;
  char *res = (char *)out;
  char *t = (char *)t_val;
  char *f = (char *)f_val;

  // Obliviously assign 8 bytes at a time
  size_t num_8_iter = bytes / 8;
  // #pragma omp simd
  for (int i = 0; i < num_8_iter; i++) {
    ObliviousAssignHelper(pred, *((uint64_t *)t), *((uint64_t *)f),
                          (uint64_t *)res);
    res += 8;
    t += 8;
    f += 8;
  }

  // Obliviously assign 4 bytes
  if ((bytes % 8) / 4) {
    ObliviousAssignHelper(pred, *((uint32_t *)t), *((uint32_t *)f),
                          (uint32_t *)res);
    res += 4;
    t += 4;
    f += 4;
  }

  // Obliviously assign 2 bytes
  if ((bytes % 4) / 2) {
    ObliviousAssignHelper(pred, *((uint16_t *)t), *((uint16_t *)f),
                          (uint16_t *)res);
    res += 2;
    t += 2;
    f += 2;
  }

  if ((bytes % 2)) {
    ObliviousAssignHelper(pred, *((uint8_t *)t), *((uint8_t *)f),
                          (uint8_t *)res);
  }
#endif
}
