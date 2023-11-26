#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#define TEST_FAILURE 1

#include <stdio.h>
#include <unordered_map>

#ifdef INVERT_EXPECT
#define expect(cond) \
    if ((cond)) { \
        fprintf(stderr, "%s: expected failure at line %d: %s\n", __FILE__, __LINE__, #cond); \
        throw TEST_FAILURE; \
    }
#else
#define expect(cond) \
    if (!(cond)) { \
        fprintf(stderr, "%s: failed at line %d: %s\n", __FILE__, __LINE__, #cond); \
        throw TEST_FAILURE; \
    }
#endif

#ifdef INVERT_EXPECT
#define fail_test() \
    fprintf(stderr, "%s: expected failure at line %d\n", __FILE__, __LINE__); \
    throw TEST_FAILURE;
#else
#define fail_test() \
    fprintf(stderr, "%s: failed at line %d\n", __FILE__, __LINE__); \
    throw TEST_FAILURE;
#endif

namespace data {
    namespace test {
        typedef void (*TestFunc)(void);

        extern std::unordered_map<const char *, std::unordered_map<const char *, TestFunc>> tests;
    }
}

#endif
