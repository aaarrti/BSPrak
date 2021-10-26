#ifndef _CONFIG_H_
#define _CONFIG_H_

#define __QEMU__

#ifdef __QEMU__
/* Werte zum testen unter QEMU */
//#define BUSY_WAIT_COUNTER 30000

#else
/* Werte zum testen auf der Hardware */
//#define BUSY_WAIT_COUNTER 30000
#endif // __QEMU__

/*
 * Represents time unit = 1 for waiting
 */
extern const unsigned int WAIT_COUNTER;

#endif // _CONFIG_H_
