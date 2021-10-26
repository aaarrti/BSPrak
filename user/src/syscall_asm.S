#define PRINT_SVC   #101
#define READ_SVC    #102
#define KILL_SVC    #103
#define CREATE_SVC  #104
#define SLEEP_SVC   #105
#define UNKNOWN_SVC #100
#define LOG_SVC_D   #106
#define LOG_SVC_I   #107
#define LOG_SVC_E   #108
#define RETURN MOV pc, lr


.global _print_char_syscall
_print_char_syscall:
	push {r7,lr}
    	mov r7, PRINT_SVC
    	svc #0
    	pop {r7, pc}
    	RETURN


.global _read_char_syscall
_read_char_syscall:
	push {r7,lr}
    	mov r7, READ_SVC
    	svc #0
    	pop {r7, pc}
    	RETURN


.global _current_thread_end_syscall
_current_thread_end_syscall:
	push {r7,lr}
    	mov r7, KILL_SVC
    	svc #0
    	pop {r7, pc}
    	RETURN


.global _thread_create_syscall
_thread_create_syscall:
	push {r7,lr}
    	mov r7, CREATE_SVC
    	svc #0
    	pop {r7, pc}
    	RETURN


.global _current_thread_sleep_syscall
_current_thread_sleep_syscall:
	push {r7,lr}
    	mov r7, SLEEP_SVC
    	svc #0
    	pop {r7, pc}
    	RETURN


.global _unknown_syscall
_unknown_syscall:
	push {r7,lr}
    	mov r7, UNKNOWN_SVC
    	svc #0
    	pop {r7, pc}
    	RETURN



.global _user_log_syscall_debug
_user_log_syscall_debug:
	push {r7,lr}
	mov r7, LOG_SVC_D
	svc #0
    	pop {r7, pc}
	RETURN




