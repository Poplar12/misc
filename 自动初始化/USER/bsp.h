#ifndef BSP_H
#define BSP_H

typedef struct 
{
    void (*set_io)(char state);
    void (*reg)(void *cb);
}io_opt_t;



#endif
