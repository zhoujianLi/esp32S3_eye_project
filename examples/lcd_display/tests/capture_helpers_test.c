#include "capture_helpers.h"

// clang-format off
// cmocka requires these standard headers to be included BEFORE <cmocka.h>
// because it uses va_list, jmp_buf, etc. without including the headers
// itself. The project's .clang-format uses IncludeBlocks: Regroup, which
// would reorder these alphabetically and put <cmocka.h> first — breaking
// the build. Disabling clang-format for this block preserves the order.
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <cmocka.h>
// clang-format on

static void test_swap_rgb565_in_place(void **state) {
    (void)state;

    uint8_t data[] = {0x12, 0x34, 0xAB, 0xCD};

    assert_true(capture_swap_rgb565_in_place(data, sizeof(data)));
    assert_int_equal(data[0], 0x34);
    assert_int_equal(data[1], 0x12);
    assert_int_equal(data[2], 0xCD);
    assert_int_equal(data[3], 0xAB);
}

static void test_swap_rgb565_in_place_rejects_odd_length(void **state) {
    (void)state;

    uint8_t data[] = {0x12, 0x34, 0x56};

    assert_false(capture_swap_rgb565_in_place(data, sizeof(data)));
    assert_int_equal(data[0], 0x12);
    assert_int_equal(data[1], 0x34);
    assert_int_equal(data[2], 0x56);
}

static void test_ensure_buffer_capacity_allocates_and_reuses(void **state) {
    (void)state;

    uint8_t *buffer      = NULL;
    size_t   buffer_size = 0;

    assert_true(capture_ensure_buffer_capacity(&buffer, &buffer_size, 8));
    assert_non_null(buffer);
    assert_int_equal(buffer_size, 8);

    uint8_t *allocated = buffer;
    assert_true(capture_ensure_buffer_capacity(&buffer, &buffer_size, 4));
    assert_ptr_equal(buffer, allocated);
    assert_int_equal(buffer_size, 8);

    free(buffer);
}

static void test_ensure_buffer_capacity_grows_buffer(void **state) {
    (void)state;

    uint8_t *buffer      = malloc(4);
    size_t   buffer_size = buffer ? 4 : 0;

    assert_non_null(buffer);
    assert_true(capture_ensure_buffer_capacity(&buffer, &buffer_size, 12));
    assert_non_null(buffer);
    assert_int_equal(buffer_size, 12);

    free(buffer);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_swap_rgb565_in_place),
        cmocka_unit_test(test_swap_rgb565_in_place_rejects_odd_length),
        cmocka_unit_test(test_ensure_buffer_capacity_allocates_and_reuses),
        cmocka_unit_test(test_ensure_buffer_capacity_grows_buffer),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
