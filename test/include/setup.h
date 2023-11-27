#ifndef TEST_SETUP_H
#define TEST_SETUP_H

#include <stdlib.h>
#include <time.h>

extern void trie_tests();
extern void radix_trie_tests();
extern void sorted_vec_tests();

void setup_tests() {
    srand(time(NULL));

    trie_tests();
    radix_trie_tests();
    sorted_vec_tests();
}

#endif
