#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <system/exceptions.h>
#include <system/cpu.h>

/**
 * \file debug.h
 *
 * \brief Hilfreiche Funktionen zum debuggen.
 */

/**
 * \brief Gibt die übergebenen Register aus.
 *
 * \param registers Registerkontext.
 */
void debug_print_register(struct regs *registers);

/**
 * \brief Gibt Informationen über das gegebene psr aus.
 *
 * \param psr Wert der als PSR Register interpretiert werden soll.
 */
void debug_print_psr(unsigned int psr);

/**
 * \brief Gibt die banked Register des angegebenen Modus aus.
 *
 * \param mode Der Modus der ausgegeben werden soll.
 */
void debug_print_banked(enum PSR_MODE mode);

#endif
