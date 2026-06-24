#include "capture_helpers.h"

#include <stdlib.h>

bool capture_swap_rgb565_in_place(uint8_t *data, size_t data_len) {
    if (!data || (data_len % 2) != 0) {
        return false;
    }

    for (size_t index = 0; index < data_len; index += 2) {
        uint8_t tmp     = data[index];
        data[index]     = data[index + 1];
        data[index + 1] = tmp;
    }

    return true;
}

bool capture_ensure_buffer_capacity(uint8_t **buffer, size_t *buffer_size, size_t required_size) {
    if (!buffer || !buffer_size) {
        return false;
    }

    if (required_size == 0) {
        free(*buffer);
        *buffer      = NULL;
        *buffer_size = 0;
        return true;
    }

    if (*buffer && *buffer_size >= required_size) {
        return true;
    }

    uint8_t *new_buffer = malloc(required_size);
    if (!new_buffer) {
        return false;
    }

    free(*buffer);
    *buffer      = new_buffer;
    *buffer_size = required_size;
    return true;
}
