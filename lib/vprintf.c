//
// Created by Artem Sereda on 08.01.21.
//

#include <driver/uart.h>
#include <stdbool.h>
#include <stdint.h>
#include "../include/lib/vprintf.h"

#define INT_LENGTH 128
enum padding_side {
    undefined = 0,
    left = 1,
    right = 2
};
typedef enum padding_side side;

struct padding_properties {
    bool needs_p;
    side side_p;
    int field_width;
};
typedef struct padding_properties padding_p;

bool is_number(char c) {
    return c >= '0' && c <= '9';
}

int get_next_number(char *const str) {
    int res = 0;
    for (char *i = str; is_number(*i); i++) {
        res = res * 10 + (*i) - '0';
    }
    return res;
}

char *scroll_till_char(char *str) {
    int i = 0;
    for (; is_number(str[i]); ++i) {
    }
    return str + i;
}

enum int_base_ {
    decimal = 10,
    hexadecimal = 16,
};
typedef enum int_base_ int_base;

char *detect_padding(char *i, padding_p *p) {
    if (*i == '-') {
        p->side_p = left;
        i++;
    } else {
        p->side_p = right;
    }
    if (is_number(*i)) {
        p->needs_p = true;
        p->field_width = get_next_number(i);
        i = scroll_till_char(i);
    }
    return i;
}

void put_str(char *str) {
    for (char *i = str; *i != '\0'; ++i) {
        uart_putc(*i);
    }
}

int str_len(const char *str) {
    const char *s;
    for (s = str; *s != '\0'; ++s) {
    }
    return (int) (s - str);
}

void insert_spaces(int len) {
    for (int i = 0; i < len; ++i) {
        uart_putc(' ');
    }
}

void insert_padding_str(char *str, padding_p *p) {
    int len = str_len(str);
    int spaces_to_insert = p->field_width - len;
    if (p->side_p == left) {
        put_str(str);
        insert_spaces(spaces_to_insert);
    } else {
        insert_spaces(spaces_to_insert);
        put_str(str);
    }
    p->side_p = undefined;
}

void handle_str(char *str, padding_p *p) {
    if (!(p->needs_p)) {
        put_str(str);
        return;
    }
    insert_padding_str(str, p);
    p->needs_p = false;
    p->field_width = 0;
}


struct int_s_ {
    int value;
    int_base base;
};
typedef struct int_s_ int_s;

void swap(char *a, char *b) {
    char temp = *a;
    *a = *b;
    *b = temp;
}

void reverse(char str[], int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        swap((str + start), (str + end));
        start++;
        end--;
    }
}

void itoa(int num, char *str, int_base base) {
    int i = 0;
    bool isNegative = false;
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
    }
    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == decimal) {
        isNegative = true;
        num = -num;
    }
    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
    str[i] = '\0'; // Append string terminator
    // Reverse the string
    reverse(str, i);
}

void put_int(int_s num) {
    if (num.value == 0) {
        uart_putc('0');
        return;
    }
    char str[INT_LENGTH];
    itoa(num.value, str, num.base);
    if (num.base == hexadecimal) {
        put_str("0x");
    }
    put_str(str);
}

void insert_zeros(int len) {
    for (int i = 0; i < len; ++i) {
        uart_putc('0');
    }
}

void insert_padding_int(int_s num, padding_p *p) {
    char buf[INT_LENGTH];
    itoa(num.value, buf, num.base);
    int spaces_to_insert = p->field_width - str_len(buf);
    if (p->side_p == left) {
        put_int(num);
        insert_spaces(spaces_to_insert);
    } else {
        if (num.base == hexadecimal) {
            put_str("0x");
            insert_zeros(spaces_to_insert);
        } else {
            insert_spaces(spaces_to_insert);
        }
        put_int(num);
    }
    p->side_p = undefined;
}

void handle_int(int_s num, padding_p *p) {
    if (!(p->needs_p)) {
        if (num.base == hexadecimal) {
            //put_str("0x");
        }
        put_int(num);
        return;
    }
    insert_padding_int(num, p);
    p->field_width = 0;
    p->needs_p = false;
}

void print_binary(uint32_t n) {
    unsigned int i;
    for (i = 1u << 31u; i > 0; i = i / 2) {
        (n & i) ? uart_putc('1') : uart_putc('0');
    }
}

void _vprintf(char *const format, va_list arg) {
    padding_p pp = {false, undefined, 0};
    for (char *i = format; *i != '\0'; i++) {
        if (*i != '%') {
            uart_putc(*i);
            continue;
        } else {
            i++;
        }
        i = detect_padding(i, &pp);
        switch (*i) {
            case 'c' :
                uart_putc(va_arg(arg,
                int));
                break;
            case 's': {
                char *str = va_arg(arg,
                char*);
                handle_str(str, &pp);
                break;
            }
            case 'i': {
                int_s num = {va_arg(arg, int), decimal};
                handle_int(num, &pp);
                break;
            }
            case 'u': {
                int_s num = {(uint32_t) va_arg(arg, int), decimal};
                handle_int(num, &pp);
            }
                break;
            case 'x': {
                int_s num = {va_arg(arg, uint32_t), hexadecimal};
                handle_int(num, &pp);
                break;
            }
            case 'p': {
                int_s num = {va_arg(arg, uint32_t), hexadecimal};
                handle_int(num, &pp);
                break;
            }
            case '%':
                uart_putc(*i);
                break;
            case 'b': {
                uint32_t u = va_arg(arg, uint32_t);
                //binary representation of uint32
                print_binary(u);
                break;
            }
        }
    }
}

void vprintf(char *const format, ...) {
    va_list arg;
    va_start(arg, format);
    _vprintf(format, arg);
    va_end(arg);
}
