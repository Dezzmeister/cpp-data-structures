#ifndef INCLUDE_STRUCTURES_RADIX_TRIE_NODE_H
#define INCLUDE_STRUCTURES_RADIX_TRIE_NODE_H

#include <optional>
#include <vector>

namespace data {
    template <typename K, typename V>
    struct RadixTrieNode {
        std::vector<K> key;
        std::optional<V> val;
        // The parent is not owned by the node
        struct RadixTrieNode<K, V> * parent;
        std::vector<struct RadixTrieNode<K, V> *> children;

        RadixTrieNode(std::vector<K> key, std::optional<V> val, struct RadixTrieNode<K, V> * parent);

        ~RadixTrieNode();

        /**
         * Returns the length of the longest common prefix shared by this key and the other key.
         */
        size_t common_prefix_len(const std::vector<K> &key, size_t offset) const;

        std::vector<K> full_key() const;
    };
}

template <typename K, typename V>
data::RadixTrieNode<K, V>::RadixTrieNode(std::vector<K> key, std::optional<V> val, RadixTrieNode<K, V> * parent)
    : key(key), val(val), parent(parent), children(std::vector<RadixTrieNode<K, V> *>()) {}

template <typename K, typename V>
data::RadixTrieNode<K, V>::~RadixTrieNode() {
    for (size_t i = 0; i < this->children.size(); i++) {
        delete this->children[i];
    }
}

template <typename K, typename V>
size_t data::RadixTrieNode<K, V>::common_prefix_len(const std::vector<K> &other_key, size_t offset) const {
    const size_t min_len = std::min(this->key.size(), other_key.size() - offset);

    for (size_t i = 0; i < min_len; i++) {
        if (this->key[i] != other_key[i + offset]) {
            return i;
        }
    }

    return min_len;
}

template <typename K, typename V>
std::vector<K> data::RadixTrieNode<K, V>::full_key() const {
    std::vector<const std::vector<K> *> keys;
    const RadixTrieNode<K, V> * node = this;

    while (node) {
        keys.push_back(&node->key);
        node = node->parent;
    }

    std::vector<K> out;

    while (keys.size()) {
        const std::vector<K> * key = keys.back();
        keys.pop_back();

        out.insert(std::end(out), std::begin(*key), std::end(*key));
    }

    return out;
}

#endif
