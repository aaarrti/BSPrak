//
// Created by Artem Sereda on 19.01.21.
//
#include <stdint.h>
#include <lib/log.h>
#include <kernel.h>
#include <lib/bit_util.h>
#include <system/scheduler.h>

#define L1_LEN 4096
#define L2_LEN 256
#define I_CACHE_BIT_N 12
#define D_CACHE_BIT_N 2
#define SHORT_TRANSLATION 0
#define ADR_TRANSLATION_EN_BIT_N 0
#define USER_DOMAIN 1
#define ALIGNMENT_CHECK_EN_BIT_N 1

#define LOCAL_TIMER_SEC 0x3f0
#define INTERRUPT_REG_SEC 0x400

// Section entry specific constants
#define SECTION_ENTRY_ADR_OFFSET 20
#define SECTION_ENTRY_BIT_N 1
//Not Global. Bei uns 0
#define NG_BIT_N 17
// Shareable. Bei uns 0.
#define S_BIT_N  16
// Access Permission bits
#define AP_BIT2_N2 15
// Memory Region Attributes. Bei uns alle 0
#define TEX_BIT_N_E  14
#define TEX_BIT_N_S  12
// Access Permission bits
#define AP_BIT_N1 11
#define AP_BIT_N0 10
// Execute never
#define XN_BIT_N 4
// Memory Region Attributes. Bei uns alle 0
#define C_BIT_N 3
#define B_BIT_N 2
// Privileged execute never
#define PXN_BIT_N 0

//L2 specific
#define SMALL_PAGE_ADR_OFFSET 12
#define L2_APX_N 9
#define L2_AP_N1 5
#define L2_AP_N0 4
#define L2_XN_N 0
//L2 pointer specific (still in L1 table)
#define L2_POINTER_PXN_BIT_N 2
#define L2_POINTER_ADR_OFFSET 10
#define L2_POINTER_BIT_N 0
#define L2_BIT_N 1

//#define DEBUG_MMU
#define MAX_STACK_SIZE THREAD_STACK_SIZE
#define ADR_TO_MB(adr) ( (adr) >> SECTION_ENTRY_ADR_OFFSET)
#define FAULT_ENTRY 0

extern uint32_t _get_system_control_reg();
extern void _set_system_control_reg(uint32_t s);
extern uint32_t _get_domain_access_control_reg();
extern void _set_domain_access_control_reg(uint32_t d);
extern uint32_t _get_translation_table_base_control_reg();
extern void _set_translation_table_base_control_reg(uint32_t d);
extern uint32_t *_get_translation_table_base_reg();
extern void _set_translation_table_base_reg(uint32_t *addr);
extern void _invalidate_tlb();

extern char _kernel_code_start[];
extern char _kernel_code_end[];
extern char _kernel_data_start[];
extern char _kernel_data_end[];
extern char _user_code_start[];
extern char _user_code_end[];
extern char _user_data_start[];
extern char _user_data_end[];

static const uint32_t ADDRESS_KERNEL_CODE_START = (uint32_t) _kernel_code_start;
static const uint32_t ADDRESS_KERNEL_CODE_END = (uint32_t) _kernel_code_end;
static const uint32_t ADDRESS_KERNEL_DATA_START = (uint32_t) _kernel_data_start;
static const uint32_t ADDRESS_KERNEL_DATA_END = (uint32_t) _kernel_data_end;
static const uint32_t ADDRESS_USER_CODE_START = (uint32_t) _user_code_start;
static const uint32_t ADDRESS_USER_CODE_END = (uint32_t) _user_code_end;
static const uint32_t ADDRESS_USER_DATA_START = (uint32_t) _user_data_start;
static const uint32_t ADDRESS_USER_DATA_END = (uint32_t) _user_data_end;


uint32_t big_page_adr_to_mb(uint32_t adr) {
    return adr >> SECTION_ENTRY_ADR_OFFSET;
}

uint32_t big_page_mb_to_adr(uint32_t mb) {
    return mb << SECTION_ENTRY_ADR_OFFSET;
}

uint32_t STACK_BASE __attribute__((section ("user_data"))) = USR_STACK_BASE;
uint32_t STACK_SIZE __attribute__((section ("user_data"))) = MAX_STACK_SIZE;

uint32_t ADR_UART __attribute__((section ("user_data"))) = 0x7E201000 - 0x3F000000;
#define UART_SEC (ADR_TO_MB((ADR_UART)))
uint32_t ADR_KERNEL_STACK __attribute__((section ("user_data"))) = (uint32_t) KERNEL_STACK_BASE - kB;

void log_sections() {
    LOG_DEBUG("Sections after linking:");
    LOG_DEBUG("ADDRESS_KERNEL_CODE_START  %x", ADDRESS_KERNEL_CODE_START);
    LOG_DEBUG("ADDRESS_KERNEL_CODE_END    %x", ADDRESS_KERNEL_CODE_END);
    LOG_DEBUG("ADDRESS_KERNEL_DATA_START  %x", ADDRESS_KERNEL_DATA_START);
    LOG_DEBUG("ADDRESS_KERNEL_DATA_END    %x", ADDRESS_KERNEL_DATA_END);
    LOG_DEBUG("KERNEL_STACKs are here till %x", KERNEL_STACK_BASE);
    LOG_DEBUG("ADDRESS_USER_CODE_START    %x", ADDRESS_USER_CODE_START);
    LOG_DEBUG("ADDRESS_USER_CODE_END      %x", ADDRESS_USER_CODE_END);
    LOG_DEBUG("ADDRESS_USER_DATA_START    %x", ADDRESS_USER_DATA_START);
    LOG_DEBUG("ADDRESS_USER_DATA_END      %x", ADDRESS_USER_DATA_END);
    LOG_DEBUG("Overflow here %x", USR_STACK_BASE - MAX_STACK_SIZE);
    LOG_DEBUG("USER_STACKs are here till %x", USR_STACK_BASE);
}


/**AP
[15,11,10]| priv. Modus| User-Modus| Kommentar
000       | No access  | No access | kein Zugriff
001       | Read/write | No access | System-Zugriff
101       | Read-only  | No access | System-Nur-Lesen
111       | Read-only  | Read-only | Beide-Nur-Lesen
010       | Read/write | Read-only | Nur-Lesen
011       | Read/write | Read/write| Vollzugriff
 */
enum _permission {
    NONE = 0, SYS_RW = 1, SYS_RO = 2, RO = 3, SYS_RW_USR_RO = 4, FULL = 5
};
typedef enum _permission permission_t;
c_str permission_str[] = {"NONE", "SYS_RW", "SYS_RO", "RO", "SYS_RW_USR_RO", "FULL"};
#define PERM_TO_STR(p) (((p)<6)?permission_str[(p)]:"INVALID")


//PX:XN |  11  |  01   |   10   |   11
enum _execution {
    NEVER_EX = 0, SYS_EX = 1, USR_EX = 2, BOTH_EX = 3
};
typedef enum _execution execution_t;
c_str execution_str[] = {"NEVER_EX", "SYS_EX", "USR_EX", "BOTH_EX"};
#define EX_TO_STR(e) (((e)<4)?execution_str[(e)]:"INVALID")


uint32_t L1_TABLE[L1_LEN] __attribute__((aligned(L1_LEN * 4)));
//arr[a][b]
//arr[n] = arr[n][]
uint32_t L2_TABLE[MAX_THREADS][L2_LEN] __attribute__((aligned(L2_LEN * MAX_THREADS*4)));


uint32_t section_entry(uint32_t mb_n, permission_t permission, execution_t execution) {
    uint32_t entry = big_page_mb_to_adr(mb_n);
    SET_NTH_BIT(entry, SECTION_ENTRY_BIT_N); // Marks sections entry
    switch (permission) {
        case FULL: {
            // 011
            SET_NTH_TO_MTH_BIT(entry, AP_BIT_N0, AP_BIT_N1);
            break;
        }
        case SYS_RO: {
            // 101
            SET_NTH_BIT(entry, AP_BIT2_N2);
            SET_NTH_BIT(entry, AP_BIT_N0);
            break;
        }
        case RO: {
            // 111
            SET_NTH_BIT(entry, AP_BIT2_N2);
            SET_NTH_TO_MTH_BIT(entry, AP_BIT_N0, AP_BIT_N1);
            break;
        }
        case NONE: {
            // 000
            break;
        }
        case SYS_RW: {
            // 001
            SET_NTH_BIT(entry, AP_BIT_N0);
            break;
        }
        case SYS_RW_USR_RO: {
            // 010
            SET_NTH_BIT(entry, AP_BIT_N1);
            break;
        }
        default: {
            LOG_ERROR("Not supported for section entry permission %s", PERM_TO_STR(permission));
            break;
        }
    }
    switch (execution) {
        case NEVER_EX: {
            SET_NTH_BIT(entry, XN_BIT_N);
            SET_NTH_BIT(entry, PXN_BIT_N);
            break;
        }
        case SYS_EX: {
            break;
        }
        case USR_EX: {
            SET_NTH_BIT(entry, PXN_BIT_N);
            break;
        }
        case BOTH_EX: {
            break;
        }
        default: {
            LOG_ERROR("Not supported for section entry execution %s", EX_TO_STR(execution));
            break;
        }
    }
    return entry;
}


uint32_t small_page_entry(uint32_t ph_adr, permission_t permission) {
    uint32_t entry = ph_adr;
    CLEAR_NTH_TO_MTH_BIT(entry, 0, SMALL_PAGE_ADR_OFFSET);
    SET_NTH_BIT(entry, L2_BIT_N);
    switch (permission) {
        case FULL: {
            // 011
            SET_NTH_TO_MTH_BIT(entry, L2_AP_N0, L2_AP_N1);
            break;
        }
        case NONE: {
            // 000
            break;
        }
        default: {
            LOG_ERROR("Unsupported permission %s for small page entry", PERM_TO_STR(permission));
            break;
        }
    }
    // Small pages are used only for user stacks, it is always USR_EX
    CLEAR_NTH_BIT(entry, L2_XN_N);
    return entry;
}


void fill_l2_tables() {
    u_int thread = 0;
    for (; thread < MAX_THREADS; thread++) {
        u_int i = 0;
        for (; i < L2_LEN; ++i) {
            uint32_t base_adr = (USR_STACK_BASE - SECTION_SIZE + (kB * 4 * i));
            permission_t permission = (base_adr < USR_STACK_BASE - MAX_STACK_SIZE) ? (NONE) : FULL;
            uint32_t small_page = small_page_entry(base_adr, permission);
            L2_TABLE[thread][i] = small_page;
        }
    }
}


uint32_t KERNEL_STACK_INDEX = ADR_TO_MB(KERNEL_STACK_BASE) - 1; // stack grows smaller
void fill_l1_table() {
    L1_TABLE[0] = FAULT_ENTRY;
    for (uint32_t i = 1; i < L1_LEN; ++i) {
        permission_t permission = FULL;
        execution_t execution = BOTH_EX;
        if (big_page_adr_to_mb(ADDRESS_KERNEL_CODE_START) == i) {
            permission = SYS_RO;
            execution = SYS_EX;
        }
        if (big_page_adr_to_mb(ADDRESS_KERNEL_DATA_START) == i) {
            permission = SYS_RW;
            execution = NEVER_EX;
        }
        if (UART_SEC == i || LOCAL_TIMER_SEC == i || INTERRUPT_REG_SEC == i || KERNEL_STACK_INDEX == i) {
            permission = SYS_RW;
            execution = NEVER_EX;
        } else if (big_page_mb_to_adr(i) > MAX_ADDR) {
            // All addresses after MAX_ADDR are invalid, except for important peripherals
            L1_TABLE[i] = FAULT_ENTRY;
            continue;
        }
        if (big_page_adr_to_mb(KERNEL_STACK_BASE) == i) {
            // Offset
            L1_TABLE[i] = 0;
            continue;
        }
        if (big_page_adr_to_mb(ADDRESS_USER_CODE_START) == i) {
            execution = USR_EX;
            permission = RO;
        }
        if (big_page_adr_to_mb(ADDRESS_USER_DATA_START) == i) {
            execution = NEVER_EX;
        }
        L1_TABLE[i] = section_entry(i, permission, execution);
    }
}


bool L2_ENABLED;

void init_mmu(bool l2_enabled) {
    L2_ENABLED = l2_enabled;
    log_sections();
    fill_l1_table();
    fill_l2_tables();
    // Set domain to User
    _set_domain_access_control_reg(USER_DOMAIN);
    #ifdef DEBUG_MMU
    if (_get_domain_access_control_reg() != USER_DOMAIN) {
        LOG_ERROR("domain control is not USER");
    }
    #endif
    // Set short translation
    _set_translation_table_base_control_reg(SHORT_TRANSLATION);
    #ifdef DEBUG_MMU
    if (_get_translation_table_base_control_reg() != SHORT_TRANSLATION) {
        LOG_ERROR("Translation status is not SHORT");
    }
    #endif
    // Set base address of TT
    _set_translation_table_base_reg(L1_TABLE);
    #ifdef DEBUG_MMU
    if (_get_translation_table_base_reg() != L1_TABLE) {
        LOG_ERROR("Translation table base address was set incorrectly");
    }
    #endif
    // Deactivate cache
    uint32_t scr = _get_system_control_reg();
    CLEAR_NTH_BIT(scr, I_CACHE_BIT_N);
    CLEAR_NTH_BIT(scr, D_CACHE_BIT_N);
    _set_system_control_reg(scr);
    #ifdef DEBUG_MMU
    if (is_nth_bit_set(_get_system_control_reg(), I_CACHE_BIT_N)) {
        LOG_ERROR("Instruction cache enabled!");
    }
    if (is_nth_bit_set(_get_system_control_reg(), D_CACHE_BIT_N)) {
        LOG_ERROR("Data cache enabled!");
    }
    #endif
    // Activate MMU
    uint32_t scr_m = _get_system_control_reg();
    SET_NTH_BIT(scr_m, ADR_TRANSLATION_EN_BIT_N);
    SET_NTH_BIT(scr_m, ALIGNMENT_CHECK_EN_BIT_N);
    _set_system_control_reg(scr_m);
}


uint32_t USR_STACK_INDEX = ADR_TO_MB(USR_STACK_BASE) - 1; // stack grows smaller
void __attribute__((used)) scheduler_on_schedule(int old, int next) {
    if (old == next || !L2_ENABLED) {
        return;
    }
    _invalidate_tlb();
    uint32_t l2_pointer_entry = (uint32_t) L2_TABLE[next];
    SET_NTH_BIT(l2_pointer_entry, L2_POINTER_BIT_N);
    SET_NTH_BIT(l2_pointer_entry, L2_POINTER_PXN_BIT_N);
    L1_TABLE[USR_STACK_INDEX] = l2_pointer_entry;
    LOG_DEBUG("Old thread %i, new thread %u", old, next);
}
