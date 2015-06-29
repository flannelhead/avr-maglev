#ifndef UTIL_H
#define UTIL_H

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define CLAMP(x, m, M) (((x) < (M)) ? (((x) > (m)) ? (x) : (m)) : (M))

#endif
