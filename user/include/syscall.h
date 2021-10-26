//
// Created by Artem Sereda on 08.01.21.
//

#ifndef KERNEL_E3_SYSCALL_USER_H
#define KERNEL_E3_SYSCALL_USER_H

#define PRINT_COUNT  10u
#define WAIT 1u
#define NULLPTR ((void *)0)

enum {
    USR_DEBUG = 0, USR_INFO = 2, USR_ERROR = 4
};

#define USR_LOG(...) user_log(USR_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

/**
 * Ein syscal fuer zeichen ausgeben
 * @param c zeichen zum ausgeben
 */
extern void _print_char_syscall(char c) __attribute__((used));


/**
 * Ein syscall fuer zeichen einlesen
 * Blockiert aktuellle thread
 * @return eingelesen zeichen
 */
extern char _read_char_syscall() __attribute__((used));

/**
 * Syscall um ein neue thread zu erzuegen
 * @param func pointer auf thread startup funktion
 * @param arg ppinter auf argumnete von funktion
 * @param arg_size groesse von arg
 */
extern void _thread_create_syscall(void (*func)(void *), const void *arg, unsigned int arg_size) __attribute__((used));

/**
 * Syscall um aktuelle thread zu beenden
 */
extern void _current_thread_end_syscall() __attribute__((used));

/**
 * Syscall um aktuelle thread für s*WAIT_COUNTER blockierend zu verzögern
 * @param s
 */
extern void _current_thread_sleep_syscall(unsigned int s) __attribute__((used));

/**
 * Unbekannte syscall, gilt als absturz von thread
 */
extern void _unknown_syscall() __attribute__((used));


void user_log(int level, const char *file, int line, const char *format, ...);

#endif //KERNEL_E3_SYSCALL_USER_H
