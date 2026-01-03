#ifndef _LOGING_H
#define _LOGING_H

#include <stdio.h>

#define RET_VOID
#define _LOG_FILE stderr
#define _LOG_RED  "\033[31m" 
#define _LOG_AEND "\033[0m" 

#define log_error_(__expr_, __return_, __fmt_, ...) do {        \
    if (__expr_) {                                              \
        fprintf(_LOG_FILE, _LOG_RED"[ERROR]"_LOG_AEND" %s:%d:%s(): " __fmt_ "\n", \
        __FILE__, __LINE__, __func__, __VA_ARGS__);           \
        return __return_; }                                     \
    } while (0)

#endif /* _LOGING_H */
