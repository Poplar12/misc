#ifndef  PRINT_H
#define  PRINT_H

typedef void(*transmit)(void);

int read_data(unsigned char *data);
int my_printf(const char *fmt, ...);
void reg_transmit(transmit cb);
 
#define DEBUG   0
#define ERR     1
#define INFO    1

#if INFO
    #define LOG_INFO(str, ...)  my_printf("[INFO]:"str "\r\n", ##__VA_ARGS__)
#else
    #define LOG_INFO(str, ...)
#endif

#if ERR
    #define LOG_ERR(str, ...)   my_printf("[ERR]:"str "\r\n", ##__VA_ARGS__)
#else
    #define LOG_ERR(str, ...)
#endif

#if DEBUG
    #define LOG_DEBUG(str, ...) my_printf("[DEBUG]:"str "\r\n", ##__VA_ARGS__)
#else
    #define LOG_DEBUG(str, ...)
#endif

#endif
