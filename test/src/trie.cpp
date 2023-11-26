#include "../include/utils.h"
#include "../../include/structures/trie.h"

void trie_tests() {
    data::test::tests["trie"]["inserting, getting, and deleting elements"] = []() {
        data::Trie<char, int> trie;

        expect(trie.node_count() == 0);

        trie.put("abc", 3, 7);
        trie.put("abd", 3, 4);
        trie.put("ab", 2, 5);
        trie.put("adb", 3, 8);

        expect(trie.get("abc", 3) == 7);
        expect(trie.get("abd", 3) == 4);
        expect(trie.get("ab", 2) == 5);
        expect(trie.get("adb", 3) == 8);

        expect(trie.node_count() == 6);

        expect(trie.del("ab", 2) == 5);
        expect(trie.node_count() == 6);
        expect(!trie.get("ab", 2).has_value());

        expect(!trie.del("a", 1).has_value());
        expect(trie.node_count() == 6);

        expect(trie.del("abc", 3) == 7);
        expect(trie.node_count() == 5);

        expect(trie.del("abd", 3) == 4);
        expect(trie.node_count() == 3);

        expect(trie.del("adb", 3) == 8);
        expect(trie.node_count() == 0);
    };
}
