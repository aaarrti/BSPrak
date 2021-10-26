//
// Created by Artem Sereda on 05.02.21.
//
#include <syscall.h>
#include <stdarg.h>


extern void _user_log_syscall_debug(const char *file, int line, const char *format, va_list *arg);

void user_log(int level, const char *file, int line, const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    if (level == USR_DEBUG) {
        _user_log_syscall_debug(file, line, format, &arg);
    }
    va_end(arg);
}

void __attribute__((used)) scheduler_thread_terminate() {
    _current_thread_end_syscall();
}
