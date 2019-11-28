#include <iostream>

/***************************************************************************************
 * Helper functions
 **************************************************************************************/

inline uint32_t greatest_power_of_two_less_than(uint32_t n) {
    uint32_t k = 1;
    while (k < n)
        k = k << 1;
    return k >> 1;
}

inline uint32_t log2_ceil(uint32_t n) {
    uint32_t k = 0;
    uint32_t _n = n;
    while (n > 1) {
        k++;
        n /= 2;
    }
    if ((1 << k) < _n)
        k++;
    return k;
}

/***************************************************************************************
 * Oblivious primitives 
 **************************************************************************************/

// Returns true if t_val > f_val else f_val
uint8_t o_greater(uint32_t t_val, uint32_t f_val) {
    uint8_t result;
    __asm__ volatile (
            "cmp %2, %1;"
            "setg %0;"
            : "=r" (result)
            : "r" (t_val), "r" (f_val)
            : "cc"
            );
    return result;
}

// Returns true if t_val == f_val else f_val
uint8_t o_equal(uint32_t t_val, uint32_t f_val) {
    uint8_t result;
    __asm__ volatile (
            "cmp %2, %1;"
            "sete %0;"
            : "=r" (result)
            : "r" (t_val), "r" (f_val)
            : "cc"
            );
    return result;
}

// Returns t_val if pred is true else f_val
uint32_t o_assign(bool pred, uint32_t t_val, uint32_t f_val) {
    uint32_t result;
    __asm__ volatile (
            "mov %2, %0;"
            "test %1, %1;"
            "cmovz %3, %0;"
            : "=r" (result)
            : "r" (pred), "r" (t_val), "r" (f_val)
            : "cc"
            );
    return result;
}

/***************************************************************************************
 * Oblivious bitonic sort (imperative version)
 **************************************************************************************/

// Imperative implementation of bitonic merge network
inline void imperative_o_merge(uint32_t* arr, uint32_t low, uint32_t len, bool ascending) {
    uint32_t i, j, k;
    uint32_t l = log2_ceil(len);
    uint32_t n = 1 << l;
    for (i = 0; i < l; i++) {
        for (j = 0; j < n; j += n >> i) {
            for (k = 0; k < (n >> i) / 2; k++) {
                uint32_t i1 = low + k + j;
                uint32_t i2 = i1 + (n >> i) / 2;
                if (i2 >= low + len) 
                    break;
                bool to_swap = ((arr[i1] > arr[i2]) == ascending);
                bool pred = o_greater(arr[i1], arr[i2]);
                pred = o_equal(pred, ascending);
                uint32_t tmp = arr[i1];
                arr[i1] = o_assign(pred, arr[i2], arr[i1]);
                arr[i2] = o_assign(pred, tmp, arr[i2]);
            }
        }
    } 
}

// Imperative implementation of bitonic sorting network -- works only for powers of 2
inline void imperative_o_sort(uint32_t* arr, uint32_t n, bool ascending) {
    uint32_t i, j, k;
    for (k = 2; k <= n; k = 2 * k) {
        for (j = k >> 1; j > 0; j = j >> 1) {
            for (i = 0; i < n; i++) {
                uint32_t ij = i ^ j;
                if (ij > i) {
                    if ((i & k) == 0) {
                        bool pred = o_greater(arr[i], arr[ij]);
                        pred = o_equal(pred, ascending);
                        uint32_t tmp = arr[i];
                        arr[i] = o_assign(pred, arr[ij], arr[i]);
                        arr[ij] = o_assign(pred, tmp, arr[ij]);
                    } else {
                        bool pred = o_greater(arr[ij], arr[i]);
                        pred = o_equal(pred, ascending);
                        uint32_t tmp = arr[i];
                        arr[i] = o_assign(pred, arr[ij], arr[i]);
                        arr[ij] = o_assign(pred, tmp, arr[ij]);
                    }
                }
            }
        }
    }
}

// Sort <len> elements in arr -- starting from index arr[low]
void o_sort(uint32_t* arr, uint32_t low, uint32_t len, bool ascending) {
    if (len > 1) {
        uint32_t m = greatest_power_of_two_less_than(len);
        if (m * 2 == len) {
            imperative_o_sort(arr + low, len, ascending);
        } else {
            imperative_o_sort(arr + low, m, !ascending);
            o_sort(arr, low + m, len - m, ascending);
            imperative_o_merge(arr, low, len, ascending);
        }
    }
}

/***************************************************************************************
 * Main
 **************************************************************************************/

//int main() {
//    int x = o_assign(false, 4, 5);
//    std::cout << x << std::endl;
//
//    bool y = o_greater(4, 5);
//    std::cout << y << std::endl;
//
//    y = o_equal(4, 4);
//    std::cout << y << std::endl;
//
//    uint32_t arr[] = {5, 2, 10, 4, 2, 1, 2, 7};
//    o_sort(arr, 0, 8, true);
//    for (int i = 0; i < 8; i++) 
//        std::cout << arr[i] << std::endl;
//}
