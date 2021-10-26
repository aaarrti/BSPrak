//
// Created by Artem Sereda on 08.01.21.
//

#ifndef KERNEL_E3_TYPES_H
#define KERNEL_E3_TYPES_H

#define NULLPTR ((void *)0)
#define BOOL_S(b) ( ((b) == 0)? "false" : "true")
#define u_int unsigned int
#define c_str const char* const
#define kB 1024
#define DIE() __builtin_trap();
#endif //KERNEL_E3_TYPES_H
