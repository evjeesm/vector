#include <check.h>
#include <stdlib.h>
#include "memory/memswap.h"

#include "memswap.h"

START_TEST(memswap_size_aligned_test)
{
    int src[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int dst[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    memswap((char*)src, (char*)dst, sizeof(src));
}
END_TEST

START_TEST(memswap_size_missaligned_test)
{
    int src[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int dst[11] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    memswap((char*)src, (char*)dst, sizeof(src));
}
END_TEST

Suite *memswap_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Memswap");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, memswap_size_aligned_test);
    tcase_add_test(tc_core, memswap_size_missaligned_test);

    suite_add_tcase(s, tc_core);

    return s;
}


int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = memswap_suite();
    sr = srunner_create(s);

    /* srunner_set_fork_status(sr, CK_NOFORK); */
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
