/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>

#define LOG_USE_COLOR
#define MAX_CALLBACKS 32

typedef struct {
    va_list ap;
    const char *fmt;
    const char *file;
    void *udata;
    int line;
    int level;
} log_Event;

typedef void (*log_LogFn)(log_Event *ev);

typedef void (*log_LockFn)(bool lock, void *udata);

enum {
    USR_DEBUG = 0, SYS_DEBUG = 1, USR_INFO = 2, SYS_INFO = 3, USR_ERROR = 4, SYS_ERROR = 5
};

#define LOG_DEBUG(...) log_log(SYS_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  log_log(SYS_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_log(SYS_ERROR, __FILE__, __LINE__, __VA_ARGS__)

void log_set_level(int level);

void log_set_quiet(bool enable);

int log_add_callback(log_LogFn fn, void *udata, int level);

//int log_add_fp(FILE *fp, int level);

void log_log(int level, const char *file, int line, const char *fmt, ...);

void va_log(int level, const char *file, int line, const char *fmt, va_list *arg);

#endif
