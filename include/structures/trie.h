#ifndef INCLUDE_STRUCTURES_TRIE_H
#define INCLUDE_STRUCTURES_TRIE_H

#include <stdlib.h>
#include <vector>
#include <optional>

namespace data {
    template <typename K, typename V>
    struct TrieNode {
        const K key;
        std::optional<V> val;
        struct TrieNode<K, V> * parent;
        std::vector<struct TrieNode<K, V> *> children;

        TrieNode(K key, std::optional<V> val, struct TrieNode<K, V> * parent);
    };

    /**
     * Trie implementation. Each node can have any number of children, which are
     * stored in an std::vector in no particular order. Worst case lookup time is O(ka)
     * where k is the length of the longest string, and a is the size of the alphabet.
     * The factor of a is incurred because of the use of a compact std::vector instead of
     * a sparse array for each element of the alphabet. The latter would enable constant-time
     * lookup at each level of the trie, but it would also use an enormous amount of space.
     *
     * "K" is the type of a character in the key. K should implement operator==.
     * "V" is the type of the value.
     */
    template <typename K, typename V>
    class Trie {
        private:
            std::vector<TrieNode<K, V> *> nodes;

            void delete_parents(TrieNode<K, V> * node);

        public:
            Trie();

            void put(const K * const key, const size_t key_len, const V value);

            std::optional<V> get(const K * const key, const size_t key_len) const;

            std::optional<V> del(const K * const key, const size_t key_len);

            size_t node_count();
    };
}

template <typename K, typename V>
data::TrieNode<K, V>::TrieNode(K key, std::optional<V> val, TrieNode<K, V> * parent) 
    : key(key), val(val), parent(parent), children(std::vector<TrieNode<K, V> *>()) {}

template <typename K, typename V>
data::Trie<K, V>::Trie() : nodes(std::vector<TrieNode<K, V> *>()) {}

template <typename K, typename V>
void data::Trie<K, V>::put(const K * const key, const size_t key_len, const V value) {
    TrieNode<K, V> * prev_node = nullptr;
    TrieNode<K, V> * curr_node = nullptr;
    std::vector<TrieNode<K, V> *> * curr_nodes = &this->nodes;

    for (size_t i = 0; i < key_len; i++) {
        prev_node = curr_node;
        curr_node = nullptr;

        for (size_t j = 0; j < curr_nodes->size(); j++) {
            if ((*curr_nodes)[j]->key == key[i]) {
                curr_node = (*curr_nodes)[j];
                curr_nodes = &curr_node->children;
                break;
            }
        }
 
        if (!curr_node) {
            curr_node = new TrieNode<K, V>(key[i], std::nullopt, prev_node);
            curr_nodes->push_back(curr_node);
            curr_nodes = &curr_node->children;
        }
    }

    if (curr_node) {
        curr_node->val = std::optional<V>(value);
    }
}

template <typename K, typename V>
std::optional<V> data::Trie<K, V>::get(const K * const key, const size_t key_len) const {
    TrieNode<K, V> * curr_node = nullptr;
    const std::vector<TrieNode<K, V> *> * curr_nodes = &this->nodes;

    for (size_t i = 0; i < key_len; i++) {
        for (size_t j = 0; j < curr_nodes->size(); j++) {
            if ((*curr_nodes)[j]->key == key[i]) {
                curr_node = (*curr_nodes)[j];
                curr_nodes = &curr_node->children;
                break;
            }
        }
    }

    if (!curr_node) {
        return std::nullopt;
    }

    return curr_node->val;
}

template <typename K, typename V>
std::optional<V> data::Trie<K, V>::del(const K * const key, const size_t key_len) {
    TrieNode<K, V> * curr_node = nullptr;
    std::vector<TrieNode<K, V> *> * curr_nodes = &this->nodes;

    for (size_t i = 0; i < key_len; i++) {
        for (size_t j = 0; j < curr_nodes->size(); j++) {
            if ((*curr_nodes)[j]->key == key[i]) {
                curr_node = (*curr_nodes)[j];
                curr_nodes = &curr_node->children;
                break;
            }
        }
    }

    if (!curr_node) {
        return std::nullopt;
    }

    const std::optional<V> out = curr_node->val;
    curr_node->val = std::nullopt;
    // We can't delete the node if it has children
    delete_parents(curr_node);

    return out;
}

template <typename K, typename V>
size_t data::Trie<K, V>::node_count() {
    std::vector<TrieNode<K, V> *> buf;
    buf.insert(std::end(buf), std::begin(this->nodes), std::end(this->nodes));

    size_t out = 0;

    while (buf.size()) {
        TrieNode<K, V> * node = buf.back();
        buf.pop_back();
        out++;

        buf.insert(std::end(buf), std::begin(node->children), std::end(node->children));
    }

    return out;
}

template <typename K, typename V>
void data::Trie<K, V>::delete_parents(TrieNode<K, V> * node) {
    if (!node) {
        return;
    }

    if (!node->children.size() && !node->val.has_value()) {
        // Remove node from parent's children
        std::vector<TrieNode<K, V> *> * siblings; 

        if (node->parent) {
            siblings = &node->parent->children;
        } else {
            siblings = &this->nodes;
        }

        for (size_t i = 0; i < siblings->size(); i++) {
            TrieNode<K, V> * sibling = (*siblings)[i];

            if (sibling == node) {
                siblings->erase(siblings->begin() + i);
                break;
            }
        }

        delete_parents(node->parent);
        delete node;
    }
}

#endif
