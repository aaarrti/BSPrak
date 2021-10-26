#ifndef _MMU_H_
#define _MMU_H_

/**
 * \file mmu.h
 *
 * \brief Hilfreiche Funktionen in zusammenhang mit der MMU.
 */

/**
 * \brief Gibt den Wert des DFSR Register aus.
 * \return Wert des DFSR.
 */
unsigned int mmu_get_dfsr(void);

/**
 * \brief Gibt den Wert des IFSR Register aus.
 * \return Wert des IFSR.
 */
unsigned int mmu_get_ifsr(void);

/**
 * \brief Gibt den Wert des Fault Register aus.
 * \return Wert des Fault Register.
 */
unsigned int mmu_get_fault_address(void);

/**
 * \brief Gibt Eine Beschreibung des Fault Status zurück.
 * \param fsr Zu interpretierender Wert.
 * \return Beschreibung.
 */
char *mmu_get_fsr_info(unsigned int fsr);


void init_mmu(bool l2_enabled);


#endif
