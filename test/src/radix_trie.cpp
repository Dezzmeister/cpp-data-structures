#include <algorithm>
#include <vector>

#include "../include/utils.h"
#include "../../include/structures/radix_trie.h"
#include "../../include/structures/radix_trie_iterator.h"
#include "../../include/structures/sorted_vec.h"

namespace {
    typedef std::pair<std::vector<char>, const int *> ro_value_type;
    typedef std::pair<std::vector<char>, int> value_type;

    template <typename T>
    std::vector<T> c_str_to_vec(const T * const c_arr) {
        std::vector<T> out;
        int i = 0;
        T c;

        while ((c = c_arr[i++])) {
            out.push_back(c);
        }

        return out;
    }

    const std::vector<value_type> exp_items = {
        { c_str_to_vec("tester"), 1 },
        { c_str_to_vec("slow"), 2 },
        { c_str_to_vec("water"), 3 },
        { c_str_to_vec("slower"), 4 },
        { c_str_to_vec("test"), 5 },
        { c_str_to_vec("team"), 6 },
        { c_str_to_vec("toast"), 7 }
    };

    data::RadixTrie<char, int> setup_r_trie() {
        data::RadixTrie<char, int> r_trie;

        expect(!r_trie.put(c_str_to_vec("tester"), 1).has_value());
        expect(!r_trie.put(c_str_to_vec("slow"), 2).has_value());
        expect(!r_trie.put(c_str_to_vec("water"), 3).has_value());
        expect(!r_trie.put(c_str_to_vec("slower"), 4).has_value());
        expect(!r_trie.put(c_str_to_vec("test"), 5).has_value());
        expect(!r_trie.put(c_str_to_vec("team"), 6).has_value());
        expect(!r_trie.put(c_str_to_vec("toast"), 7).has_value());

        return r_trie;
    }

    void test_item_equality(const std::vector<ro_value_type> &items, const std::vector<value_type> &exp_items) {
        expect(items.size() == exp_items.size());

        for (size_t i = 0; i < items.size(); i++) {
            expect(items[i].first == exp_items[i].first);
            expect(*items[i].second == exp_items[i].second);
        }
    }
}

void radix_trie_tests() {
    data::test::tests["radix trie"]["inserting elements"] = []() {
        data::RadixTrie<char, int> r_trie = setup_r_trie();

        expect(r_trie.depth() == 4);

        {
            data::RadixTrieNode<char, int> * node = r_trie.get_node(c_str_to_vec("toast"));
            expect(!!node);
            expect(node->key == c_str_to_vec("oast"));
            expect(node->val == 7);

            node = node->parent;
            expect(!!node);
            expect(node->key == c_str_to_vec("t"));
            expect(!node->val.has_value());

            expect(!node->parent);
        }

        {
            data::RadixTrieNode<char, int> * node = r_trie.get_node(c_str_to_vec("tester"));
            expect(!!node);
            expect(node->key == c_str_to_vec("er"));
            expect(node->val == 1);

            node = node->parent;
            expect(!!node);
            expect(node->key == c_str_to_vec("st"));
            expect(node->val == 5);

            node = node->parent;
            expect(!!node);
            expect(node->key == c_str_to_vec("e"));
            expect(!node->val.has_value());

            node = node->parent;
            expect(!!node);
            expect(node->key == c_str_to_vec("t"));
            expect(!node->val.has_value());

            expect(!node->parent);
        }
    };

    data::test::tests["radix trie"]["getting elements"] = []() {
        data::RadixTrie<char, int> r_trie = setup_r_trie();

        expect(r_trie.get(c_str_to_vec("tester")) == 1);
        expect(r_trie.get(c_str_to_vec("slow")) == 2);
        expect(r_trie.get(c_str_to_vec("water")) == 3);
        expect(r_trie.get(c_str_to_vec("slower")) == 4);
        expect(r_trie.get(c_str_to_vec("test")) == 5);
        expect(r_trie.get(c_str_to_vec("team")) == 6);
        expect(r_trie.get(c_str_to_vec("toast")) == 7);
    };

    data::test::tests["radix trie"]["deleting elements"] = []() {
        data::RadixTrie<char, int> r_trie = setup_r_trie();

        expect(r_trie.del(c_str_to_vec("tester")) == 1);
        expect(r_trie.depth() == 3);

        expect(r_trie.del(c_str_to_vec("test")) == 5);
        expect(r_trie.del(c_str_to_vec("team")) == 6);
        expect(r_trie.depth() == 2);

        expect(!r_trie.get_node(c_str_to_vec("te")));

        expect(r_trie.del(c_str_to_vec("toast")) == 7);
        expect(r_trie.del(c_str_to_vec("slower")) == 4);
        expect(r_trie.depth() == 1);

        expect(r_trie.del(c_str_to_vec("slow")) == 2);
        expect(r_trie.del(c_str_to_vec("water")) == 3);
        expect(r_trie.depth() == 0);
    };

    data::test::tests["radix trie"]["merges a chain of nodes on deletion"] = []() {
        data::RadixTrie<char, int> r_trie;

        r_trie.put(c_str_to_vec("faster"), 1);
        r_trie.put(c_str_to_vec("fastest"), 2);
        r_trie.put(c_str_to_vec("fastester"), 3);
        r_trie.put(c_str_to_vec("fastestest"), 4);

        expect(r_trie.del(c_str_to_vec("fastester")) == 3);
        expect(r_trie.del(c_str_to_vec("fastest")) == 2);
        expect(r_trie.del(c_str_to_vec("faster")) == 1);
 
        data::SortedVec<data::RadixTrieNode<char, int> *> &nodes = r_trie.get_nodes();
        expect(nodes.size() == 1);
        expect(nodes[0]->key == c_str_to_vec("fastestest"));
        expect(nodes[0]->val == 4);
        expect(nodes[0]->children.size() == 0);
    };

    data::test::tests["radix trie"]["iterator"] = []() {
        data::RadixTrie<char, int> r_trie = setup_r_trie();
        std::vector<ro_value_type> items;

        for (auto node : r_trie) {
            items.push_back(node);
        }

        std::sort(std::begin(items), std::end(items), [](const ro_value_type &a, const ro_value_type &b) {
            return *a.second < *b.second;
        });

        test_item_equality(items, exp_items);

        data::RadixTrieIterator<char, int> it = std::begin(r_trie);
        data::RadixTrieIterator<char, int> it2 = data::RadixTrieIterator<char, int>(it);
        data::RadixTrieIterator<char, int> end = std::end(r_trie);

        for (; (it != end); ++it, it2++) {
            expect(*it == *it2);
        }
    };

    data::test::tests["radix trie"]["entries"] = []() {
        data::RadixTrie<char, int> r_trie = setup_r_trie();

        std::vector<ro_value_type> items = r_trie.entries();

        std::sort(std::begin(items), std::end(items), [](const ro_value_type &a, const ro_value_type &b) {
            return *a.second < *b.second;
        });

        test_item_equality(items, exp_items);
    };

    data::test::tests["radix trie"]["entries_with_prefix"] = []() {
        const std::vector<value_type> exp_items_test = {
            { c_str_to_vec("tester"), 1 },
            { c_str_to_vec("test"), 5 }
        };

        const std::vector<value_type> exp_items_t = {
            { c_str_to_vec("tester"), 1 },
            { c_str_to_vec("test"), 5 },
            { c_str_to_vec("team"), 6 },
            { c_str_to_vec("toast"), 7 }
        };

        data::RadixTrie<char, int> r_trie = setup_r_trie();

        std::vector<ro_value_type> items = r_trie.entries_with_prefix(c_str_to_vec("test"));

        std::sort(std::begin(items), std::end(items), [](const ro_value_type &a, const ro_value_type &b) {
            return *a.second < *b.second;
        });

        test_item_equality(items, exp_items_test);

        items = r_trie.entries_with_prefix(c_str_to_vec("t"));

        std::sort(std::begin(items), std::end(items), [](const ro_value_type &a, const ro_value_type &b) {
            return *a.second < *b.second;
        });

        test_item_equality(items, exp_items_t);
    };
}
