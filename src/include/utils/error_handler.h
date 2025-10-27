#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

void error_handler(const char* file, int line, const char* msg);

#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            error_handler(__FILE__, __LINE__, #condition); \
        } \
    } while (0)

#endif // ERROR_HANDLER_H