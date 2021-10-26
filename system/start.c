#include <driver/uart.h>
#include <driver/local_timer.h>
#include <system/irq.h>
#include <system/exceptions.h>
#include <system/panic.h>
#include <system/scheduler.h>
#include <system/syscall_handler.h>
#include <lib/log.h>
#include <types.h>
#include <system/debug.h>
#include <kernel.h>
#include <driver/mmu.h>
#include <stdint.h>


extern char _user_code_start[];
extern char _kernel_code_start[];
const uint32_t ADDRESS_USER_CODE_S = (uint32_t) _user_code_start;
const uint32_t ADDRESS_KERNEL_CODE_S = ((uint32_t) _kernel_code_start) - kB;
uint32_t *NOT_ASSIGNED __attribute__((section ("user_data"))) = (uint32_t *) 0x1500000;


bool is_exception_from_kernel(reg_t *r) {
    unsigned int mode_i = 0x1F & r->psr;
    return mode_i != PSR_USR;
}

c_str ex_str[] = {"Undefined Instruction", "Supervisor Call", "Prefetch Abort", "Data Abort", "Interrupt"};
#define EX_TO_STR(e) (((e) == 0 || (e) > 6)?"INVALID":ex_str[(e)-1])


void handle_abort(exeption_t exception_mode) {
    if (exception_mode != E_DABT && exception_mode != E_PABT) {
        return;
    }
    u_int dfsr = mmu_get_dfsr();
    u_int ifsr = mmu_get_ifsr();
    if (dfsr != 0) {
        LOG_ERROR("Fault_addr = %x", mmu_get_fault_address());
        LOG_ERROR("DFSR = %x, desription: %s", dfsr, mmu_get_fsr_info(dfsr));
    } else {
        LOG_ERROR("Fault_addr = %x", mmu_get_fault_address());
        LOG_ERROR("IFSR = %x, desription: %s", ifsr, mmu_get_fsr_info(ifsr));
    }
}


void handle_fatal_exception(exeption_t exception_mode, reg_t *r) {
    handle_abort(exception_mode);
    if (is_exception_from_kernel(r)) {
        LOG_ERROR("%s in Kernel!", EX_TO_STR(exception_mode));
        debug_print_register(r);
        PANIC("Kernel halted");
    }
    LOG_ERROR("%s in thread %u", EX_TO_STR(exception_mode), scheduler_get_current_thread());
    if (scheduler_end_current() != 0) {
        LOG_ERROR("Failed to end thread");
    }
    scheduler_next(r);
}



//N: lesender Zugriff auf Null-Pointer.
//P: Sprung auf Null-Pointer.
//C: schreibender Zugriff auf eigenen Code.
//U: lesender Zugriff auf nicht zugeordnete Adresse.
//X: Sprung auf User Code
u_int keep_this __attribute__((used));
void handle_reserved_input() {
    if (uart_peekc() != 'N' && uart_peekc() != 'P' && uart_peekc() != 'C' && uart_peekc() != 'U' &&
        uart_peekc() != 'X') {
        return;
    }
    char c = uart_getc();
    switch (c) {
        case 'N': {
            LOG_INFO("Reading Data from NULLPTR -> data abort expected");
            keep_this = *(volatile u_int *) NULLPTR;
            break;
        }
        case 'P': {
            LOG_INFO("Jumping to NULLPTR -> prefetch abort expected");
            asm volatile("b 0x0");
            break;
        }
        case 'C': {
            LOG_INFO("Wring own code at %p -> data abort expected", ADDRESS_KERNEL_CODE_S);
            char *code_addr = (char *) ADDRESS_KERNEL_CODE_S;
            *code_addr = 'a';
            break;
        }
        case 'U': {
            LOG_INFO("Writing not assigned addres %p -> data abort expected", NOT_ASSIGNED);
            uint32_t *addr = (uint32_t *) NOT_ASSIGNED;
            *addr = 55;
            break;
        }
        case 'X': {
            LOG_INFO("Writing user code at %p -> data abort expected", ADDRESS_USER_CODE_S);
            char *code_addr = (char *) ADDRESS_USER_CODE_S;
            *code_addr = 'a';
            break;
        }
        default: {
            break;
        }
    }
}


void __attribute__((used)) exception(exeption_t e, reg_t *r) {
    if (e == E_UND || e == E_PABT || e == E_DABT) {
        handle_fatal_exception(e, r);
        return;
    }
    if (e == E_SVC) {
        handle_syscall(r);
        return;
    }
    if (e == E_IRQ) {
        if (irq_uart_pending()) {
            LOG_DEBUG("UART IRQ");
            uart_handle_irq();
            handle_reserved_input();
        }
        if (local_timer_pending_irq()) {
            LOG_DEBUG("Timer IRQ");
            scheduler_get_current_thread();
            scheduler_round_robin(r);
            local_timer_clear_irq();
        }
        return;
    }
    PANIC("Invalid exception mode %s");
}


__attribute__((weakref("user_idle_thread"))) static void _user_idle_thread();
void create_idle_user_thread() {
    if (_user_idle_thread == NULLPTR) {
        LOG_ERROR("_user_idle_thread() == NULLPTR");
        return;
    }
    if (scheduler_new_thread(_user_idle_thread, NULLPTR, 0) < 0) {
        LOG_ERROR("Failed to create user idle thread");
    }
}


c_str BANNER = {"\n\n \
#################################################################  \n \
Enter N/P/C/U/X to kill me in different ways                    #  \n \
You can also hurt user's threads with n/p/d/k/K/g/c/s/u/x       #  \n \
################################################################## \n  \
                                                 pls dont be ugly\n"};

void __attribute__((used)) start_kernel() {
    //log_set_level(SYS_INFO);
    set_vector_base();
    uart_init();
    init_mmu(true);
    local_timer_init();
    uart_irq_enable();
    irq_uart_enable();
    scheduler_init(USR_STACK_BASE, THREAD_STACK_SIZE, true);
    local_timer_start();
    asm volatile("cpsie i");
    create_idle_user_thread();
    LOG_INFO(BANNER);
}
