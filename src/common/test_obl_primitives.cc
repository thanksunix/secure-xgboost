#include <iostream>
#include <obl_primitives.h>

/***************************************************************************************
 * Testing
 **************************************************************************************/

void test(const char* name, bool cond) {
    printf("%s : ", name);
    if (cond)
        printf("pass\n");
    else
        printf("fail\n");
}
void test_o_greater() {
    // Test generic cases
    test("4 > 5", !o_greater(4,5));
    test("5 > 4", o_greater(5,4));
    test("4 > 4", !o_greater(4,4));

    // Test negative cases
    test("-4 > 4", !o_greater(-4,4));
    test("4 > -4", o_greater(4, -4));
    test("-4 > -5", o_greater(-4, -5));
    test("-5 > -4", !o_greater(-5, -4));

    // Test floating point
    test("-4. > -3.", !o_greater(-4., -3.));
    test("-4.1 > -4.2", o_greater(-4.1, -4.2));
    test("-4.2 > -4.1", !o_greater(-4.2, -4.1));
    test("-4. > -4.", !o_greater(-4., -4.));
    test(".4 > .3", o_greater(.4, .3));
    test(".4 > .5", !o_greater(.4, .5));

    // Test integer overflow
    test("(int32_t) 2147483648 > 42", !o_greater((int32_t)2147483648, 42));
    test("2147483648 > 42", o_greater(2147483648, (int64_t) 42));
}
void test_o_less() {
    // Test generic cases
    test("4 < 5", o_less(4,5));
    test("5 < 4", !o_less(5,4));
    test("4 < 4", !o_less(4,4));

    // Test negative cases
    test("-4 < 4", o_less(-4,4));
    test("4 < -4", !o_less(4, -4));
    test("-4 < -5", !o_less(-4, -5));
    test("-5 < -4", o_less(-5, -4));

    // Test floating point
    test("-4. < -3.", o_less(-4., -3.));
    test("-4.1 < -4.2", !o_less(-4.1, -4.2));
    test("-4.2 < -4.1", o_less(-4.2, -4.1));
    test("-4. < -4.", !o_less(-4., -4.));
    test(".4 < .3", !o_less(.4, .3));
    test(".4 < .5", o_less(.4, .5));

    // Test integer overflow
    test("(int32_t) 2147483648 < 42", o_less((int32_t)2147483648, 42));
    test("2147483648 < 42", !o_less(2147483648, (int64_t) 42));
}
void test_o_equal() {
    // Test generic cases
    test("4 == 5", !o_equal(4,5));
    test("5 == 4", !o_equal(5,4));
    test("4 == 4", o_equal(4,4));

    // Test negative cases
    test("-4 == 4", !o_equal(-4,4));
    test("4 == -4", !o_equal(4, -4));
    test("-4 == -5", !o_equal(-4, -5));
    test("-5 == -4", !o_equal(-5, -4));
    test("-4 == -4", o_equal(-4,-4));

    // Test floating point
    test("-4. == -3.", !o_equal(-4., -3.));
    test("-4.1 == -4.2", !o_equal(-4.1, -4.2));
    test("-4.2 == -4.1", !o_equal(-4.2, -4.1));
    test(".4 == .3", !o_equal(.4, .3));
    test(".4 == .5", !o_equal(.4, .5));
    test(".4 == .400001", !o_equal(.4, .400001));
    test("-4. == -4.", o_equal(-4., -4.));
    test("4. == 4.", o_equal(4., 4.));
}

/***************************************************************************************
 * Main
 **************************************************************************************/

int main() {
    test_o_greater();
    test_o_less();
    test_o_equal();
}