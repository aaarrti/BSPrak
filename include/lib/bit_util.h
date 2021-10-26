//
// Created by Artem Sereda on 21.01.21.
//

#ifndef KERNEL_E3_BIT_UTIL_H
#define KERNEL_E3_BIT_UTIL_H

#include <stdint.h>
#include <types.h>

#define ASSERT(expr) if (!(expr))  LOG_ERROR("Assertion Error")
#define LEN_BITS(value) (sizeof(value)*8)

/**
 * n in [0, LEN_BITS(value)-1]
 */
#define SET_NTH_BIT(value, n) set_nth_bit(&value, n)
#define CLEAR_NTH_BIT(value, n) clear_nth_bit(&value, n)

//#define IS_NTH_BIT_SET(value, n) ( ((value) & (1u << (n))) > 0)

#define SET_NTH_TO_MTH_BIT(value, n, m) set_nth_to_mth_bit(&value, n, m)

#define CLEAR_NTH_TO_MTH_BIT(value, n, m)  clear_nth_to_mth_bit(&value, n, m)


void set_nth_bit(uint32_t *value, u_int n);

void clear_nth_bit(uint32_t *value, u_int n);

bool is_nth_bit_set(uint32_t value, u_int n);

void set_nth_to_mth_bit(uint32_t *value, u_int n, u_int m);

void clear_nth_to_mth_bit(uint32_t *value, u_int n, u_int m);

#endif //KERNEL_E3_BIT_UTIL_H
