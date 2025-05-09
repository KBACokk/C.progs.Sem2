#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include "sort.h"


void test_insert_sort(void **state) {
    (void)state;
    
    int arr_inc[5] = {1, 2, 3, 4, 5};
    int expected_inc[5] = {1, 2, 3, 4, 5};
    int trud_inc = insertSort(5, arr_inc);
    assert_memory_equal(arr_inc, expected_inc, sizeof(arr_inc));
    

    int arr_dec[5] = {5, 4, 3, 2, 1};
    int expected_dec[5] = {1, 2, 3, 4, 5};
    int trud_dec = insertSort(5, arr_dec);
    assert_memory_equal(arr_dec, expected_dec, sizeof(arr_dec));
    
    
    int arr_rand[5] = {3, 1, 4, 5, 2};
    int expected_rand[5] = {1, 2, 3, 4, 5};
    int trud_rand = insertSort(5, arr_rand);
    assert_memory_equal(arr_rand, expected_rand, sizeof(arr_rand));
}

void test_fill_functions(void **state) {
    (void)state;
    
    int arr[10];
    
    FillInc(10, arr);
    for (int i = 0; i < 10; i++) {
        assert_int_equal(arr[i], i + 1);
    }
    
    FillDec(10, arr);
    for (int i = 0; i < 10; i++) {
        assert_int_equal(arr[i], 10 - i);
    }
    
    FillRand(10, arr);
    for (int i = 0; i < 10; i++) {
        assert_true(arr[i] >= 0 && arr[i] < 100);
    }
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_insert_sort),
        cmocka_unit_test(test_fill_functions),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}