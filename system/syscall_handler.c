//
// Created by Artem Sereda on 08.01.21.
//

#include <lib/log.h>
#include <driver/uart.h>
#include <system/panic.h>
#include <system/debug.h>
#include <system/scheduler.h>
#include <system/syscall_handler.h>
#include <config.h>
#include <types.h>

enum svc_mode {
    SVC_PRINT = 101, SVC_READ = 102, SVC_THREAD_END = 103,
    SVC_THREAD_CREATE = 104, SVC_THREAD_SLEEP = 105, SVC_DEBUG = 106
};

#pragma GCC push_options
#pragma GCC optimize ("O0")
void sleep_blocking(u_int to_wait) {
    volatile unsigned long long i = 0;
    for (; i < to_wait * WAIT_COUNTER; i++) {
        asm volatile("nop");
    }
}
#pragma GCC pop_options

char wait_for_char() {
    char c;
    do {
        _unmask_interrupts();
        while (!uart_read_available()) {
        }
        _mask_interrupts();
        c = uart_getc();
    } while (c == 'S');
    return c;
}


extern bool is_exception_from_kernel();

void handle_syscall(reg_t *r) {
    if (is_exception_from_kernel(r)) {
        debug_print_register(r);
        PANIC("SVC in kernel");
    }
    enum svc_mode mode = r->r7;
    switch (mode) {
        case SVC_PRINT: {
            LOG_DEBUG("SVC print");
            char c = r->r0;
            uart_putc(c);
            break;
        }
        case SVC_READ: {
            LOG_DEBUG("SVC read");
            char c = wait_for_char();
            r->r0 = c;
            break;
        }
        case SVC_THREAD_END: {
            LOG_DEBUG("SVC kill");
            if (scheduler_end_current() != 0) {
                LOG_ERROR("failed to end thread");
            }
            scheduler_next(r);
            break;
        }
        case SVC_THREAD_CREATE: {
            LOG_DEBUG("SVC create");
            void (*func)(void *) = (void (*)(void *)) r->r0;
            const void *arg = (void *) r->r1;
            unsigned int arg_size = r->r2;
            int id = scheduler_new_thread(func, arg, arg_size);
            if (id < 0) {
                LOG_ERROR("Failed to create thread");
            } else {
                LOG_INFO("Created thread with id %u", id);
            }
            break;

        }
        case SVC_THREAD_SLEEP: {
            LOG_DEBUG("SVC sleep");
            unsigned int s = r->r0;
            sleep_blocking(s);
            break;
        }

        case SVC_DEBUG: {
            va_log(USR_DEBUG, (const char *) r->r0, r->r1, (const char *) r->r2, (va_list *) r->r3);
            break;
        }
        default: {
            LOG_ERROR("Unknown SVC %u in thread %u", mode, scheduler_get_current_thread());
            if (scheduler_end_current() != 0) {
                LOG_ERROR("failed to end thread");
            }
            scheduler_next(r);
            break;
        }

    }
}
