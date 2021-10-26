//
// Created by Artem Sereda on 08.01.21.
//

#ifndef KERNEL_E3_SYSCALL_HANDLER_H
#define KERNEL_E3_SYSCALL_HANDLER_H

/**
 * Blocks active thread, but kernel still recieves IRQs,
 * current thread will not be paused by scheduler until done here
 * @param to_wait time inuts to sleep
 */
void sleep_blocking(unsigned int to_wait);

/**
 * Blocks active thread, but kernel still recieves IRQs,
 * current thread will not be paused by scheduler until done here
 * @return readed from uart char
 */
char wait_for_char();


void handle_syscall(reg_t *r);

extern void _unmask_interrupts();

extern void _mask_interrupts();

#endif //KERNEL_E3_SYSCALL_HANDLER_H
