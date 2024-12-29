#define _DEFAULT_SOURCE

#define SYSTEM_MESSAGE(MESSAGE) "\x1B[102m\x1B[30m[JadeOS][✅]\x1B[0m "#MESSAGE"\n"
#define SYSTEM_ERROR(MESSAGE) "\x1B[101m\x1B[30m[JadeOS][❌]\x1B[0m "#MESSAGE"\n"
#define SYSTEM_MESSAGE_WITH_PARAM(MESSAGE, FORMATS) "\x1B[102m\x1B[30m[JadeOS][✅]\x1B[0m "#MESSAGE"\n", FORMATS