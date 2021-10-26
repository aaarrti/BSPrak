#include <stdbool.h>

#ifndef _KERNEL_H_
#define _KERNEL_H_

/**
 * \file kernel.h
 *
 * Enthält feste Werte zur Initialisierung des kernels
 */

#define PSR_USR (0x10)
#define PSR_IRQ (0x12)
#define PSR_SVC (0x13)
#define PSR_ABT (0x17)
#define PSR_UND (0x1b)
#define PSR_SYS (0x1f)
#define SECTION_SIZE (1024 * 1024)
#define MAX_ADDR (8 * (1024 * 1024)) // 128MB
#define KERNEL_STACK_BASE  0x400000
#define MIN_ADR (0x100000)
#define EXCEPTION_MODE_STACK_SIZE (4 * ( 1024 )) // 4KB
#define THREAD_STACK_SIZE (4 * ( 1024 )) // 4KB
#define SVC_STACK_BASE ( KERNEL_STACK_BASE - ( 0 * EXCEPTION_MODE_STACK_SIZE) )
#define ABT_STACK_BASE ( KERNEL_STACK_BASE - ( 1 * EXCEPTION_MODE_STACK_SIZE) )
#define UDF_STACK_BASE ( KERNEL_STACK_BASE - ( 2 * EXCEPTION_MODE_STACK_SIZE) )
#define IRQ_STACK_BASE ( KERNEL_STACK_BASE - ( 3 * EXCEPTION_MODE_STACK_SIZE) )
#define SYS_STACK_BASE ( KERNEL_STACK_BASE - ( 4 * EXCEPTION_MODE_STACK_SIZE) )
#define USR_STACK_BASE (MAX_ADDR)
#endif // _KERNEL_H_
