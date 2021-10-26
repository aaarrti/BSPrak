#ifndef _IRQ_H_
#define _IRQ_H_

/**
 * \file irq.h
 *
 * \brief Enthält Funktionen zum Konfigurieren des Irq Controllers des Raspi 2B
 */

/**
 * \brief Überprüft ob im ein uart irq am irq controller vorhanden ist
 * \return: 1 falls vorhanden.
 *          0 sonst.
 */
int irq_uart_pending(void);

/**
 * \brief Aktiviert irq's am irq controller für den uart.
 *
 * Damit uart interrupts auftreten muss der uart zusätlich
 * auf irq umgeschaltet werden
 */
void irq_uart_enable(void);

/**
 * \brief Deaktiviert irq's am irq controller für den uart.
 */
void irq_uart_disable(void);


#endif
