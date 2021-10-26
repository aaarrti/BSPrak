/*
 * Copyright (c) 2020 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */


#include <lib/vprintf.h>
#include "../include/lib/log.h"


#define WHITE "\e[0m"
#define BHRED "\e[1;91m"
#define HBLU "\e[0;94m"
#define CYN "\e[0;36m"
#define HBLU "\e[0;94m"
#define RED "\e[0;31m"
#define BYEL "\e[1;33m"

typedef struct {
    log_LogFn fn;
    void *udata;
    int level;
} Callback;

static struct {
    void *udata;
    log_LockFn lock;
    int level;
    bool quiet;
    Callback callbacks[MAX_CALLBACKS];
} L;


static const char *level_strings[] = {
        "USR_DEBUG", "SYS_DEBUG", "USR_INFO", "SYS_INFO", "USR_ERROR", "SYS_ERROR"
};

#ifdef LOG_USE_COLOR
static const char *level_colors[] = {
        WHITE, CYN, BYEL, HBLU, RED, BHRED
};
#endif

static void stdout_callback(log_Event *ev) {

#ifdef TIME_T
    char buf[16];
    buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
    kprintf(
            "%s %s%-5s\033[0m %s%s%d:\033[0m ",
            buf, level_colors[ev->level], level_strings[ev->level],
            level_colors[ev->level], ev->file, ev->line);
#else
#ifdef LOG_USE_COLOR
    //vprintf("%s%s %s: %s:%i: ", level_colors[ev->level], __TIME__, level_strings[ev->level], ev->file, ev->line);
    vprintf("%s%s: %s:%i: ", level_colors[ev->level], level_strings[ev->level], ev->file, ev->line);
    _vprintf((char *const) ev->fmt, ev->ap);
    vprintf(WHITE);
    vprintf("\n");
#else
    kprintf("%-6s%s%d: ", level_strings[ev->level], ev->file, ev->line);
    kprintf((char *const) ev->fmt, ev->ap);
    kprintf("\n");
    reset_color();
    flush_uart();
#endif
#endif

}


static void lock(void) {
    if (L.lock) { L.lock(true, L.udata); }
}


static void unlock(void) {
    if (L.lock) { L.lock(false, L.udata); }
}


const char *log_level_string(int level) {
    return level_strings[level];
}


void log_set_lock(log_LockFn fn, void *udata) {
    L.lock = fn;
    L.udata = udata;
}


void log_set_level(int level) {
    L.level = level;
}


void log_set_quiet(bool enable) {
    L.quiet = enable;
}


int log_add_callback(log_LogFn fn, void *udata, int level) {
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        if (!L.callbacks[i].fn) {
            L.callbacks[i] = (Callback) {fn, udata, level};
            return 0;
        }
    }
    return -1;
}

/*
int log_add_fp(FILE *fp, int level) {
    return log_add_callback(file_callback, fp, level);
}
*/


static void init_event(__attribute__((unused)) log_Event *ev, __attribute__((unused)) void *udata) {
#ifdef TIME_T
    if (!ev->time) {
      time_t t = time(NULL);
      ev->time = localtime(&t);
    }
    ev->udata = udata;
#endif
}


void va_log(int level, const char *file, int line, const char *fmt, va_list *arg) {
    log_Event ev = {
            .fmt   = fmt,
            .file  = file,
            .line  = line,
            .level = level,
    };

    lock();

    if (!L.quiet && level >= L.level) {
        //init_event(&ev, stderr);
        ev.ap = *arg;
        stdout_callback(&ev);
    }

    for (int i = 0; i < MAX_CALLBACKS && L.callbacks[i].fn; i++) {
        Callback *cb = &L.callbacks[i];
        if (level >= cb->level) {
            init_event(&ev, cb->udata);
            ev.ap = *arg;
            cb->fn(&ev);
        }
    }
    unlock();
}


void log_log(int level, const char *file, int line, const char *fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    va_log(level, file, line, fmt, &arg);
    va_end(arg);
}


