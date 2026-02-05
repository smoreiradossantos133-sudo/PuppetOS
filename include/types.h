#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>
#include <stddef.h>

typedef int                 bool;
#define true                1
#define false               0

#define NULL                ((void *)0)

/* Helper macros */
#define ALIGN_UP(n, align)     (((n) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(n, align)   ((n) & ~((align) - 1))
#define MIN(a, b)              ((a) < (b) ? (a) : (b))
#define MAX(a, b)              ((a) > (b) ? (a) : (b))

#endif /* __TYPES_H__ */
