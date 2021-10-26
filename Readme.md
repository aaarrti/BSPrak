### Memory Layout V1

|Begin address|Begin in MB|End address|End in MB|Description|
|---|---|---|---|---|
|`0x000 00000`|0MB|`0x001 00000`|1Mb|No man's land|
|`0x001 00000`|1Mb|`0x002 00000`|2MB|Kernel code|
|`0x002 00000`|2MB|`0x003 00000`|3MB|Kernel data|
|`0x003 00000`|3MB|`0x004 00000`|4MB|Kernel stack (grows smaller during runtime!!!)|
|`0x005 00000`|4MB|`0x006 00000`|5MB|No man's offset|
|`0x005 00000`|5MB|`0x006 00000`|6MB|User code|
|`0x006 00000`|6MB|`0x007 00000`|7MB|User data|
|`0x007 00000`|7MB|`0x008 00000`|8MB|User stack (grows smaller during runtime!!!|
|`0x008 00000`|8MB|`0x128 00000`|128MB|No man's land|

### SVC Conventions

Immediate value is written in r7 register (just because it is really unlikely to be used by smth else)
. </br> [As written here](https://developer.arm.com/documentation/dui0203/j/handling-processor-exceptions/armv6-and-earlier--armv7-a-and-armv7-r-profiles/svc-handlers)
reading svc from instruction immediate is only possible in top level assembler handler </br>
and we don't have acces to it's source code.

- print char -> svc 101 -> char in r0
- read char -> svc 102 -> char in r0
- end thread -> svc 103
- create thread -> svc 104 -> func in r0, arg in r1, arg_size in r2
- thread sleep_blocking -> svc 105 -> value to sleep in r0
- log syscall - -> file in r0, line in r1, format in r2, *to va_arg in r2 debug -> svc 106, info -> svc 107, error -> svc 109

### [Notion](https://www.notion.so/c3cf309d90e94459b06ca224f34fc6e0?v=bc74d7dd8c454292a24f604fe06c34ca)

### Debugging

- run make (clean) qemu_debug
- run debug client configuration
- debug qemu -> run qemu_*monitor -> telnet localhost 45454 -> (gva2gpa a.e.)
  more [here](https://lists.gnu.org/archive/html/qemu-devel/2019-04/msg02183.html)
- if required port is in use -> sudo lsof -i :3000 -> kill -9 <PID>

### Coding conventions

- ALWAYS use compiler from `../toolchain/arm/bin/arm-none-eabi-gcc` (U know what haapend with HA3)
  `brew unlink arm-none-eabi-gcc` <br> then add `../toolchain/arm/bin/arm-none-eabi-gcc`.  to ur `/private/etc/paths`. Verify
  with `which arm-none-eabi-gcc`
- "global" constant should be defined in .h and with #define directive
- don't modify global variables directly, create function with descriptive name for that
- constans, enums -> SCREAMING_SNAKE_CASE
- function names, variables -> undescore_case
- ALWAYS check make clean all output before commiting
- extern/weak functions -> _function_name()
- ALWAYS manual check submission archive's contents

### Tips

- always mark directories as "Project source and include", so inspections/static analysis works at least halfway properly
- use bundled GDB for debuggin in UI (CLion fails with one from toolchain)
