#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdbool.h>


// useless tests, TODO: REPLACE WITH REAL TESTS
void test_assert_true(void) {
    CU_ASSERT(true);
}

void test_assert_2_not_equal_minus_1(void) {
    CU_ASSERT_NOT_EQUAL(2, -1);
}

int setup(void) { return 0; }

int teardown(void) { return 0; }

int main(void) {
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();


    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("test_suite", setup, teardown);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }


    if ((NULL == CU_add_test(pSuite, "basic_test_1", test_assert_true)) ||
        (NULL == CU_add_test(pSuite, "Test assert 2 not equal -1", test_assert_2_not_equal_minus_1))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());

    CU_cleanup_registry();
    return CU_get_error();
}
