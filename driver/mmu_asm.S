#define COPROC_READ 	MRC
#define COPROC_WRITE    MCR
#define RETURN 		MOV PC, LR

.global _get_system_control_reg
_get_system_control_reg:
    	COPROC_READ p15, 0, r0, c1, c0, 0
    	RETURN

.global _set_system_control_reg
_set_system_control_reg:
	COPROC_WRITE p15, 0, r0, c1, c0, 0
    	RETURN



.global _set_domain_access_control_reg
_set_domain_access_control_reg:
	COPROC_WRITE p15, 0, r0, c3, c0, 0
	RETURN

.global _get_domain_access_control_reg
_get_domain_access_control_reg:
	COPROC_READ p15, 0, r0, c3, c0, 0
	RETURN



.global _get_translation_table_base_control_reg
_get_translation_table_base_control_reg:
	COPROC_READ p15, 0, r0, c2, c0, 2
	RETURN

.global _set_translation_table_base_control_reg
_set_translation_table_base_control_reg:
	COPROC_WRITE p15, 0, r0, c2, c0, 2
	RETURN


.global _get_translation_table_base_reg
_get_translation_table_base_reg:
	COPROC_READ p15, 0, r0, c2, c0, 0
	RETURN

.global _set_translation_table_base_reg
_set_translation_table_base_reg:
	COPROC_WRITE p15, 0, r0, c2, c0, 0
	RETURN



.global _invalidate_tlb
_invalidate_tlb:
	COPROC_WRITE p15, 0, r0, c8, c7, 0
	RETURN
