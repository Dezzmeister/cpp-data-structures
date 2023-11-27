#ifndef INCLUDE_STRUCTURES_RADIX_TRIE_H
#define INCLUDE_STRUCTURES_RADIX_TRIE_H

#include <queue>
#include <optional>
#include <stdio.h>
#include <stdlib.h>
#include <utility>
#include <vector>

#include "radix_trie_iterator.h"
#include "radix_trie_node.h"

namespace data {
    template <typename K, typename V>
    class RadixTrie {
        private:
            typedef std::pair<std::vector<K>, const V *> entry_type;

            std::vector<RadixTrieNode<K, V> *> nodes;

            void split_and_insert(RadixTrieNode<K, V> * node, std::vector<K> key, const V value, const size_t prefix_len, const size_t char_count);

            void delete_node(RadixTrieNode<K, V> * node);

            void try_delete_node(RadixTrieNode<K, V> * node);

            size_t depth_rec(const std::vector<RadixTrieNode<K, V> *> * nodes) const;

            std::vector<entry_type> entries_rec(const std::vector<K> &key, const std::vector<RadixTrieNode<K, V> *> &nodes) const;

        public:
            RadixTrie();

            ~RadixTrie();

            std::optional<V> put(const std::vector<K> key, const V value);

            std::optional<V> get(const std::vector<K> key);

            std::optional<V> del(const std::vector<K> key);

            size_t depth() const;

            std::vector<entry_type> entries() const;

            RadixTrieIterator<K, V> begin();

            RadixTrieIterator<K, V> end();

            std::vector<entry_type> entries_with_prefix(const std::vector<K> &key) const;

#ifdef TEST
            void print();

            std::vector<RadixTrieNode<K, V> *>& get_nodes();

            RadixTrieNode<K, V> * get_node(const std::vector<K> key);
#endif
    };
}


template <typename K, typename V>
data::RadixTrie<K, V>::RadixTrie() : nodes(std::vector<RadixTrieNode<K, V> *>()) {}

template <typename K, typename V>
data::RadixTrie<K, V>::~RadixTrie() {
    for (size_t i = 0; i < this->nodes.size(); i++) {
        delete this->nodes[i];
    }
}

template <typename T>
static void print_vec(const std::vector<T> &vec) {
    for (size_t i = 0; i < vec.size(); i++) {
        printf("%c", vec[i]);
    }
}

template <typename K, typename V>
void data::RadixTrie<K, V>::split_and_insert(RadixTrieNode<K, V> * node, std::vector<K> key, const V value, const size_t prefix_len, const size_t char_count)  {
    std::vector<K> other_key_prev = std::vector<K>(node->key.begin(), node->key.begin() + prefix_len);
    RadixTrieNode<K, V> * other_node_prev = new RadixTrieNode<K, V>(other_key_prev, std::nullopt, node->parent);
    other_node_prev->children.push_back(node);

    std::vector<RadixTrieNode<K, V> *> * siblings;

    if (other_node_prev->parent) {
        siblings = &other_node_prev->parent->children;
    } else {
        siblings = &this->nodes;
    }
        
    for (size_t j = 0; j < siblings->size(); j++) {
        if ((*siblings)[j] == node) {
            (*siblings)[j] = other_node_prev;
            break;
        }
    }

    node->parent = other_node_prev;
    node->key = std::vector<K>(node->key.begin() + prefix_len, node->key.end());

    if (char_count == key.size()) {
        other_node_prev->val = std::optional(value);
    } else {
        RadixTrieNode<K, V> * key_node = new RadixTrieNode<K, V>(std::vector<K>(key.begin() + char_count, key.end()), std::optional(value), other_node_prev);
        other_node_prev->children.push_back(key_node);
    }
}

template <typename K, typename V>
std::optional<V> data::RadixTrie<K, V>::put(const std::vector<K> key, const V value) {
    size_t char_count = 0;

    RadixTrieNode<K, V> * curr_node = nullptr;
    std::vector<RadixTrieNode<K, V> *> * curr_nodes = &this->nodes;
    bool found_node = true;

    while (found_node) {
        found_node = false;

        for (size_t i = 0; i < curr_nodes->size(); i++) {
            RadixTrieNode<K, V> * node = (*curr_nodes)[i];
            const size_t prefix_len = node->common_prefix_len(key, char_count);

            if (!prefix_len) {
                continue;
            }

            // The key is not exhausted yet
            if ((prefix_len + char_count) < key.size()) {
                char_count += prefix_len;

                // The prefix is the entire node key; search node's children
                if (prefix_len == node->key.size()) {
                    curr_node = node;
                    curr_nodes = &node->children;
                    found_node = true;
                    break;
                }

                // make split branch, add node, and return
                if (prefix_len == node->key.size()) {
                    // add new node as child
                    RadixTrieNode<K, V> * new_node = new RadixTrieNode<K, V>(std::vector<K>(key.begin() + char_count, key.end()), std::optional(value), node);
                    node->children.push_back(new_node);
                    return std::nullopt;
                } else {
                    // split node, make branch
                    this->split_and_insert(node, key, value, prefix_len, char_count);
                }
                return std::nullopt;
            }

            // The entire key has been exhausted - we have to do something now
            if ((prefix_len + char_count) == key.size()) {
                char_count += prefix_len;

                // make split branch and return
                if (prefix_len == node->key.size()) {
                    // write over existing node's value
                    const std::optional<V> out = node->val;
                    node->val = std::optional(value);

                    return out;
                } else {
                    // split node, make branch
                    this->split_and_insert(node, key, value, prefix_len, char_count);
                }
                return std::nullopt;
            }

            __builtin_unreachable();
        }
    }

    RadixTrieNode<K, V> * key_node = new RadixTrieNode<K, V>(std::vector<K>(key.begin() + char_count, key.end()), std::optional(value), curr_node);
    if (curr_node) {
        curr_node->children.push_back(key_node);
    } else {
        this->nodes.push_back(key_node);
    }

    return std::nullopt;
}

template <typename K, typename V>
std::optional<V> data::RadixTrie<K, V>::get(const std::vector<K> key) {
    size_t char_count = 0;

    RadixTrieNode<K, V> * curr_node = nullptr;
    std::vector<RadixTrieNode<K, V> *> * curr_nodes = &this->nodes;
    bool found_node = true;

    while (found_node) {
        found_node = false;

        for (size_t i = 0; i < curr_nodes->size(); i++) {
            RadixTrieNode<K, V> * node = (*curr_nodes)[i];
            const size_t prefix_len = node->common_prefix_len(key, char_count);

            if (!prefix_len) {
                continue;
            } else if (prefix_len < node->key.size()) {
                return std::nullopt;
            } else if ((prefix_len + char_count) == key.size()) {
                return node->val;
            } else {
                curr_node = node;
                curr_nodes = &curr_node->children;
                char_count += prefix_len;
                found_node = true;
                break;
            }
        }
    }

    return std::nullopt;
}

template <typename K, typename V>
std::optional<V> data::RadixTrie<K, V>::del(const std::vector<K> key) {
    size_t char_count = 0;

    RadixTrieNode<K, V> * curr_node = nullptr;
    std::vector<RadixTrieNode<K, V> *> * curr_nodes = &this->nodes;
    bool found_node = true;

    while (found_node) {
        found_node = false;

        for (size_t i = 0; i < curr_nodes->size(); i++) {
            RadixTrieNode<K, V> * node = (*curr_nodes)[i];
            const size_t prefix_len = node->common_prefix_len(key, char_count);

            if (!prefix_len) {
                continue;
            } else if (prefix_len < node->key.size()) {
                return std::nullopt;
            } else if ((prefix_len + char_count) == key.size()) {
                const std::optional<V> out = node->val;

                node->val = std::nullopt;
                this->try_delete_node(node);
                return out;
            } else {
                curr_node = node;
                curr_nodes = &curr_node->children;
                char_count += prefix_len;
                found_node = true;
                break;
            }
        }
    }

    return std::nullopt;
}

template <typename K, typename V>
void data::RadixTrie<K, V>::delete_node(RadixTrieNode<K, V> * node) {
    std::vector<RadixTrieNode<K, V> *> * siblings;

    if (node->parent) {
        siblings = &node->parent->children;
    } else {
        siblings = &this->nodes;
    }

    for (size_t i = 0; i < siblings->size(); i++) {
        RadixTrieNode<K, V> * sibling = (*siblings)[i];

        if (sibling == node) {
            siblings->erase(siblings->begin() + i);
            break;
        }
    }

    RadixTrieNode<K, V> * parent = node->parent;

    if (parent && !parent->val.has_value()) {
        this->try_delete_node(parent);
    }

    delete node;
}

template <typename K, typename V>
void data::RadixTrie<K, V>::try_delete_node(RadixTrieNode<K, V> * node) {
    if (!node->children.size()) {
        this->delete_node(node);
    } else if (node->children.size() == 1) {
        RadixTrieNode<K, V> * child = node->children[0];

        node->key.insert(std::end(node->key), std::begin(child->key), std::end(child->key));
        node->val = child->val;
        node->children = child->children;
        this->delete_node(child);
    } 
}

template <typename K, typename V>
size_t data::RadixTrie<K, V>::depth_rec(const std::vector<RadixTrieNode<K, V> *> * nodes) const {
    size_t max = 0;

    for (size_t i = 0; i < nodes->size(); i++) {
        const size_t node_max = 1 + this->depth_rec(&(*nodes)[i]->children);

        if (node_max > max) {
            max = node_max;
        }
    }

    return max;
}

template <typename K, typename V>
size_t data::RadixTrie<K, V>::depth() const {
    return this->depth_rec(&this->nodes);
}

template <typename K, typename V>
std::vector<typename data::RadixTrie<K, V>::entry_type> data::RadixTrie<K, V>::entries_rec(const std::vector<K> &key, const std::vector<RadixTrieNode<K, V> *> &nodes) const {
    std::vector<entry_type> out;

    for (size_t i = 0; i < nodes.size(); i++) {
        const RadixTrieNode<K, V> * node = nodes[i];
        std::vector<K> full_key;

        full_key.insert(std::end(full_key), std::begin(key), std::end(key));
        full_key.insert(std::end(full_key), std::begin(node->key), std::end(node->key));

        if (node->val.has_value()) {
            out.push_back(entry_type(full_key, &node->val.value()));
        }

        std::vector<entry_type> node_entries = this->entries_rec(full_key, node->children);
 
        for (size_t j = 0; j < node_entries.size(); j++) {
            entry_type entry = node_entries[j];
            out.push_back(entry);
        }
    }

    return out;
}

template <typename K, typename V>
std::vector<typename data::RadixTrie<K, V>::entry_type> data::RadixTrie<K, V>::entries() const {
    const std::vector<K> key;

    return this->entries_rec(key, this->nodes);
}

template <typename K, typename V>
data::RadixTrieIterator<K, V> data::RadixTrie<K, V>::begin() {
    if (!this->nodes.size()) {
        return this->end();
    }

    RadixTrieNode<K, V> * node = this->nodes[0];

    while (!node->val.has_value() && node->children.size()) {
        node = node->children[0];
    }

    return RadixTrieIterator<K, V>(&this->nodes, node, false);
}

template <typename K, typename V>
data::RadixTrieIterator<K, V> data::RadixTrie<K, V>::end() {
    if (!this->nodes.size()) {
        return RadixTrieIterator<K, V>(&this->nodes, nullptr, true);
    }

    RadixTrieNode<K, V> * node = this->nodes.back();

    while (node->children.size()) {
        node = node->children.back();
    }

    return RadixTrieIterator<K, V>(&this->nodes, node, true);
}

template <typename K, typename V>
std::vector<typename data::RadixTrie<K, V>::entry_type> data::RadixTrie<K, V>::entries_with_prefix(const std::vector<K> &key) const {
    size_t char_count = 0;

    RadixTrieNode<K, V> * curr_node = nullptr;
    const std::vector<RadixTrieNode<K, V> *> * curr_nodes = &this->nodes;
    bool found_node = true;

    while (found_node) {
        found_node = false;

        for (size_t i = 0; i < curr_nodes->size(); i++) {
            RadixTrieNode<K, V> * node = (*curr_nodes)[i];
            const size_t prefix_len = node->common_prefix_len(key, char_count);

            if (!prefix_len) {
                continue;
            } else if (prefix_len < node->key.size()) {
                return {};
            } else if ((prefix_len + char_count) == key.size()) {
                std::vector<entry_type> out = this->entries_rec(key, node->children);

                if (node->val.has_value()) {
                    out.push_back({ key, &node->val.value() });
                }

                return out;
            } else {
                curr_node = node;
                curr_nodes = &curr_node->children;
                char_count += prefix_len;
                found_node = true;
                break;
            }
        }
    }

    return {};
}

#ifdef TEST
template <>
void data::RadixTrie<char, int>::print() {
    size_t level = 0;
    std::queue<RadixTrieNode<char, int> *> buf;
    for (size_t i = 0; i < this->nodes.size(); i++) {
        buf.push(this->nodes[i]);
    }

    printf("Starting radix trie printout. Parent nodes: %ld\n", this->nodes.size());

    while (buf.size()) {
        size_t pull_ct = buf.size();

        printf("=== LEVEL %ld ===\n", level);

        for (size_t i = 0; i < pull_ct; i++) {
            RadixTrieNode<char, int> * node = buf.front();
            buf.pop();

            printf("(");
            if (node->parent) {
                print_vec(node->parent->key);
            } else {
                printf("(null)");
            }
            printf("->");
            print_vec(node->key);
            if (node->val.has_value()) {
                printf(":%d", node->val.value());
            } else {
                printf(":(null)");
            }
            printf(") ");

            for (size_t j = 0; j < node->children.size(); j++) {
                buf.push(node->children[j]);
            }
        }
        printf("\n");

        level++;
    }

}

template <typename K, typename V>
void data::RadixTrie<K, V>::print() {
    fprintf(stderr, "Unimplemented: %s\n", __func__);
    throw "Unimplemented";
}

template <typename K, typename V>
std::vector<data::RadixTrieNode<K, V> *>& data::RadixTrie<K, V>::get_nodes() {
    return this->nodes;
}

template <typename K, typename V>
data::RadixTrieNode<K, V> * data::RadixTrie<K, V>::get_node(std::vector<K> key) {
    size_t char_count = 0;

    RadixTrieNode<K, V> * curr_node = nullptr;
    std::vector<RadixTrieNode<K, V> *> * curr_nodes = &this->nodes;
    bool found_node = true;

    while (found_node) {
        found_node = false;

        for (size_t i = 0; i < curr_nodes->size(); i++) {
            RadixTrieNode<K, V> * node = (*curr_nodes)[i];
            const size_t prefix_len = node->common_prefix_len(key, char_count);

            if (!prefix_len) {
                continue;
            } else if (prefix_len < node->key.size()) {
                return nullptr;
            } else if ((prefix_len + char_count) == key.size()) {
                return node;
            } else {
                curr_node = node;
                curr_nodes = &curr_node->children;
                char_count += prefix_len;
                found_node = true;
                break;
            }
        }
    }

    return nullptr;
}

#endif
#endif
