#ifndef TEST_SETUP_H
#define TEST_SETUP_H

void trie_tests();
void radix_trie_tests();

void setup_tests() {
    trie_tests();
    radix_trie_tests();
}

#endif
