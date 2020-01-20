#ifndef OBL_PRIMITIVES_H_
#define OBL_PRIMITIVES_H_

#define CACHE_LINE_SIZE 64

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
void ObliviousAssign(bool pred, uint8_t t_val, uint8_t f_val, uint8_t* out);

template <typename T>
inline void ObliviousMerge(T* arr, uint32_t low, uint32_t len, bool ascending);
template <typename T>
inline void ObliviousMergePOD(T* arr, uint32_t low, uint32_t len, bool ascending);

template <typename T>
inline void ObliviousSort(T* arr, uint32_t low, uint32_t len, bool ascending);
template <typename T>
inline void ObliviousSortPOD(T* arr, uint32_t low, uint32_t len, bool ascending);

template <typename T>
T ObliviousArrayAccess(T *arr, int i, size_t n);
    
template <typename T>
void ObliviousArrayAssign(T *arr, int i, size_t n, T val);

#endif // OBL_PRIMITIVES_H_
