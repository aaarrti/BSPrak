#ifndef _KPRINT_H_
#define _KPRINT_H_

/**
 * \file kprintf.h
 * \brief Enthält Funktion für I/O über den UART des Raspi
 *
 * Hilfreiche Funktionen für formatierte String Ausgaben über den UART. Der
 * UART muss zuerst initialisiert werdem mit \p uart_init() aus \p uart.h
 */


/**
 * \brief Einfache printf ähnliche Funktion wie aus stdio.h
 * \param format Format String
 * \param ... Argumente
 *
 * | Vorhandene Format Parameter (%..)   | Vorhandene Feldbreitenparameter |
 * | :-----------------------------------|:--------------------------------|
 * | c: Einzelner char                   | [0-9]*: Feldbreite              |
 * | s: Null-Byte terminierter String    | 0: Null Padding                 |
 * | u: unsigned int in Dezimal          | #: Prefix                       |
 * | i: int in Dezimal                   | +: Vorzeichen                   |
 * | x: unsigned int in Hexadezimal      |                                 |
 * | p: Pointer                          |                                 |
 * | %: Prozent                          |                                 |
 */
void kprintf(const char *format, ...) __attribute__ ((format(printf, 1, 2)));

#endif /* _KPRINT_H_ */
