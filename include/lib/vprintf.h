//
// Created by Artem Sereda on 08.01.21.
//

#ifndef KERNEL_E3_VPRINTF_H
#define KERNEL_E3_VPRINTF_H

#include <stdarg.h>

/*
 * Just for the sake of being able to use pretty loggger
 */
void _vprintf(char *const format, va_list arg);

void vprintf(char *format, ...)
__attribute__ ((__format__ (__printf__, 1, 2)));


#endif //KERNEL_E3_VPRINTF_H
