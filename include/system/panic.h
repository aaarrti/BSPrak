#ifndef _PANIC_H_
#define _PANIC_H_

/**
 * \brief Funktionalitäten um das System kontrolliert zum Stoppen zu bringen.
 * \file panic.h
 */

/**
 * \brief Macro um kernel panic auszugeben
 */
#define PANIC(msg) _panic(__func__,msg)

/**
 * \brief Hält das System an
 * \param func_name Name der Funktion, die den Fehler verursacht hat
 * \param msg Optionale Nachricht die mit ausgegebn wird
 *
 * Blockiert Interrupts und initialisiert den
 * UART minimal um eine panic Nachricht auszugeben.
 * Anschließend wird das System angehalten.
 *
 */
void _panic(const char *func_name, char *msg) __attribute__((noreturn));

#endif
