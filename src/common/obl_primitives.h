#ifndef OBL_PRIMITIVES_H_
#define OBL_PRIMITIVES_H_

template <typename T>
uint8_t ObliviousGreater(T x, T y);
uint8_t ObliviousGreater(double x, double y);

template <typename T>
uint8_t ObliviousGreaterOrEqual(T x, T y);
uint8_t ObliviousGreaterOrEqual(double x, double y);

template <typename T>
uint8_t ObliviousLess(T x, T y);
uint8_t ObliviousLess(double x, double y);

template <typename T>
uint8_t ObliviousLessOrEqual(T x, T y);
uint8_t ObliviousLessOrEqual(double x, double y);

template <typename T>
uint8_t ObliviousEqual(T x, T y);

template <typename T>
T ObliviousChoose(bool pred, const T& t_val, const T& f_val);

template <typename T>
void ObliviousAssign(bool pred, const T& t_val, const T& f_val, T* out);

#endif // OBL_PRIMITIVES_H_