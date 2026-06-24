#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool capture_swap_rgb565_in_place(uint8_t *data, size_t data_len);
bool capture_ensure_buffer_capacity(uint8_t **buffer, size_t *buffer_size, size_t required_size);
