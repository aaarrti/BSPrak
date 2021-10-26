//
// Created by Artem Sereda on 26.01.21.
//
#include <stdint.h>
#include "stdbool.h"
#include "../include/types.h"

void set_nth_bit(uint32_t *value, u_int n) {
    uint32_t v = *value;
    v |= 1u << (n);
    *value = v;
}

void clear_nth_bit(uint32_t *value, u_int n) {
    uint32_t v = *value;
    v &= ~(1u << n);
    *value = v;
}

bool is_nth_bit_set(uint32_t value, u_int n) {
    return (value & (1u << n)) > 0;
}

void set_nth_to_mth_bit(uint32_t *value, u_int n, u_int m) {
    for (unsigned int i = n; i <= m; ++i) {
        set_nth_bit(value, i);
    }
}

void clear_nth_to_mth_bit(uint32_t *value, u_int n, u_int m) {
    for (unsigned int i = n; i <= m; ++i) {
        clear_nth_bit(value, i);
    }
}
