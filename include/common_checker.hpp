#ifndef __OPENCVPP11_CHECKER__
#define __OPENCVPP11_CHECKER__

#include <cstdio>

#define CHECKER_ASSERT(x, ...) do {\
    if(!(x)) {\
        fprintf(stderr, __VA_ARGS__);\
    }\
} while(0)

#endif
