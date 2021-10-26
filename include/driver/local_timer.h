#ifndef _TIMER_H_
#define _TIMER_H_

/**
 * \file local_timer.h
 *
 * \brief Implementierung des Local Timer des BCM2836
 */

/**
 * \brief Startet den Timer
 */
void local_timer_start();

/**
 * \brief Stoppt den timer
 */
void local_timer_stop();

/**
 * \brief Gibt zurück, ob ein unbehandelter Timer interrupt
 * aufgetreten ist.
 *
 * \return > 0 Interrupt ist aufgetreten
 */
unsigned int local_timer_pending_irq();

/**
 *\brief Setzt den Interrupt zurück
 *
 * Cleart den Interrupt. Diese Funktion sollte jedes mal,
 * nachdem ein Timer Interrupt behandelt wurde, aufgerufen
 * werden
 */
void local_timer_clear_irq();

/**
 * \brief Setzt den reload Wert des Timers
 *
 * Setzt den Timer reload value auf den entsprechenden
 * Wert in Mikro Sekunden
 * Der Timer wird neu geladen und fängt von vorne an zu zählen,
 * es wird jedoch kein Interrupt verursacht.
 *
 * \param us Wert in Mikrosekunden
 */
void local_timer_set_reload(unsigned int us);

/**
 * \brief Setzt den Timer zurück
 *
 * Setzt den Timer zurück ohne ein Interrupt zu triggern
 */
void local_timer_reload();

/**
 * \brief Initialisiert den Timer
 *
 * Initialisiert den Timer und setzt ihn auf ca 1 Sekunde
 * Der Timer ist nach dem initialisieren noch nicht gestartet.
 * Interrupts sind aktiv.
 */
void local_timer_init();

#endif /* _TIMER_H_ */
