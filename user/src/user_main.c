//
// Created by Artem Sereda on 08.01.21.
//



#include <stdint.h>
#include <stdbool.h>
#include <syscall.h>


#define B_STR(b) ( ((b) == 0)? "false" : "true")
const unsigned int WAIT_COUNTER = 10000000;


#pragma GCC push_options
#pragma GCC optimize ("O0")
void wait_actively(unsigned int w) {
    unsigned long long i = 0;
    for (; i < w * WAIT_COUNTER; ++i) {
        asm volatile("nop");
    }
}
#pragma GCC pop_options

void printer(void *v) {
    char c = *((char *) v);
    if (c >= 'a' && c <= 'z') {
        for (unsigned int n = 0; n < PRINT_COUNT; n++) {
            _print_char_syscall(c);
            _current_thread_sleep_syscall(WAIT);
        }
        return;
    }
    if (c >= 'A' && c <= 'Z') {
        for (unsigned int n = 0; n < PRINT_COUNT; n++) {
            _print_char_syscall(c);
            wait_actively(WAIT);
        }
    }
}

extern uint32_t STACK_BASE;
extern uint32_t STACK_SIZE;
extern char _kernel_code_end[];
extern char _kernel_data_start[];
static const uint32_t *ADDRESS_KERNEL_CODE = (uint32_t *) _kernel_code_end;
static const uint32_t *ADDRESS_KERNEL_DATA = (uint32_t *) _kernel_data_start;
extern uint32_t *ADR_KERNEL_STACK;
extern uint32_t ADR_UART;
extern char _user_code_start[];
static const uint32_t *ADDRESS_USER_CODE = (uint32_t *) _user_code_start;
extern uint32_t *NOT_ASSIGNED;

uint32_t keep_this_for_user __attribute__((used));
#pragma GCC push_options
#pragma GCC optimize ("O0")
void do_stack_overflow() {unsigned int sp = (unsigned int) __builtin_frame_address(0);
    unsigned int stack_size = STACK_BASE - sp;
    bool overflow = stack_size > STACK_SIZE;
    USR_LOG("SP = %x, stack size = %u, overflow = %s", sp, stack_size, B_STR(overflow));
    do_stack_overflow();
}
#pragma GCC pop_options


//n: lesender Zugriff auf Null-Pointer.
//p: Sprung auf Null-Pointer.
//d: lesender Zugriff auf Kernel-Daten.
//k: lesender Zugriff auf Kernel-Code.
//K: lesender Zugriff auf Kernel-Stack.
//g: lesender Zugriff auf Peripherie-Gerät, z.B. UART.
//c: schreibender Zugriff auf eigenen Code.
//s: Stack-Overflow.
//u: lesender Zugriff auf nicht zugeordnete Adresse.
//x: Sprung auf eigene Daten oder Stack.
void misbehave(void *v) {
    char c = *((char *) v);
    switch (c) {
        case 'n': {
            USR_LOG("Reading Data from NULLPTR -> data abort expected");
            keep_this_for_user = *(volatile uint32_t *) NULLPTR;
            break;
        }
        case 'p': {
            USR_LOG("Jumping to NULLPTR -> prefetch abort expected");
            asm volatile("b 0x0");
            break;
        }
        case 'd': {
            USR_LOG("Trying to read kernel data on %p -> data abort expected", ADDRESS_KERNEL_DATA);
            keep_this_for_user = *ADDRESS_KERNEL_DATA;
            break;
        }
        case 'k': {
            //SYS_ERROR: system/start.c:35: Fault_addr = 0x3f201000
            //SYS_ERROR: system/start.c:36: DFSR = 0xd, desription: Permission Section fault
            //SYS_ERROR: system/start.c:46: Data Abort in thread 1
            USR_LOG("Trying to read kernel code on %p -> data abort expected", ADDRESS_KERNEL_CODE);
            keep_this_for_user = *ADDRESS_KERNEL_CODE;
            break;
        }
        case 'K': {
            USR_LOG("Trying to read kernel stack on %p -> data abort expected", ADR_KERNEL_STACK);
            keep_this_for_user = *ADR_KERNEL_STACK;
            break;
        }
        case 'g': {
            USR_LOG("Reading from UART on %x -> data abort expected", ADR_UART);
            keep_this_for_user = *(uint32_t *) ADR_UART;
            break;
        }

        case 'c': {
            USR_LOG("Writing own code on %p -> data abort expected", ADDRESS_USER_CODE);
            char *code_addr = (char *) ADDRESS_USER_CODE;
            *code_addr = 'a';
            break;
        }
        case 's': {
            USR_LOG("Stackoverflow");
            do_stack_overflow();
            break;
        }
        case 'u': {
            USR_LOG("Writing not assigned address %p -> data abort expected", NOT_ASSIGNED);
            *NOT_ASSIGNED = 55;
            break;
        }
        case 'x': {
            USR_LOG("Jumping to own stack on 0x3fffff -> prefetch abort expected");
            asm volatile("b 0x3fffff");
            break;
        }
        default: {
            USR_LOG("%c is ok, nothing special");
            break;
        }
    }
}


_Noreturn void __attribute__((used)) user_idle_thread() {
    USR_LOG("user_idle_thread is running");
    for (;;) {
        asm("WFI");
        char c = _read_char_syscall();
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            _thread_create_syscall(misbehave, &c, sizeof(c));
        }

    }
}


