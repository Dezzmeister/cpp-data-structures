#ifndef INCLUDE_STRUCTURES_RADIX_TRIE_ITERATOR_H
#define INCLUDE_STRUCTURES_RADIX_TRIE_ITERATOR_H

#include <iterator>
#include <utility>

#include "radix_trie_node.h"

namespace data {
    /**
     * Custom iterator for RadixTrie. The "value type" of the iterator is a pair where
     * the first item is the full key to an entry, and the second item is a pointer to the
     * corresponding value. Null nodes are skipped.
     *
     * Because the value type is constructed by the iterator (and is not actually present in the
     * radix trie), the iterator can only be used to get full pairs (instead of references or pointers
     * to pairs).
     */
    template <typename K, typename V>
    class RadixTrieIterator {
        private:
            std::vector<RadixTrieNode<K, V> *> * top_nodes;
            RadixTrieNode<K, V> * curr_node;
            bool end;

            RadixTrieNode<K, V> * next_node_up(RadixTrieNode<K, V> * node);

            constexpr void check_impl();

        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::pair<std::vector<K>, V *>;
            using difference_type = int;
            using pointer = value_type *;
            using reference = value_type&;

            RadixTrieIterator();
 
            RadixTrieIterator(std::vector<RadixTrieNode<K, V> *> * top_nodes, RadixTrieNode<K, V> * node, bool end);

            RadixTrieIterator<K, V>& operator++();

            RadixTrieIterator<K, V> operator++(int);

            bool operator==(const RadixTrieIterator<K, V> &it) const;

            bool operator!=(const RadixTrieIterator<K, V> &it) const;

            value_type operator*() const;
    };
}

template <typename K, typename V>
constexpr void data::RadixTrieIterator<K, V>::check_impl() {
    // Require that this class satisfies the forward iterator concept at compile time.
    // The assertion is checked when the template is instantiated, so it cannot be
    // outside of RadixTrieIterator with generic template arguments. It can be in the
    // template class declaration, but it won't be valid because the class
    // does not exist at this point. The assertion has to be checked when the class exists
    // and is instantiated with type arguments.
    static_assert(std::forward_iterator<RadixTrieIterator<K, V>>);
}

template <typename K, typename V>
data::RadixTrieIterator<K, V>::RadixTrieIterator() : top_nodes(nullptr), curr_node(nullptr), end(true) {
    this->check_impl();
}

template <typename K, typename V>
data::RadixTrieIterator<K, V>::RadixTrieIterator(std::vector<RadixTrieNode<K, V> *> * top_nodes, RadixTrieNode<K, V> * node, bool end)
    : top_nodes(top_nodes), curr_node(node), end(end) 
{
    this->check_impl();
}

template <typename K, typename V>
data::RadixTrieIterator<K, V>& data::RadixTrieIterator<K, V>::operator++() {
    if (this->end) {
        return *this;
    }

    if (curr_node->children.size()) {
        curr_node = curr_node->children[0];

        while (!curr_node->val.has_value()) {
            // A leaf node cannot have a null value; at some point we will reach
            // a non-null node
            curr_node = curr_node->children[0];
        }
    } else {
        RadixTrieNode<K, V> * node = this->next_node_up(curr_node);

        if (node) {
            this->curr_node = node;
        } else {
            this->end = true;
        }
    }

    return *this;
}

template <typename K, typename V>
data::RadixTrieIterator<K, V> data::RadixTrieIterator<K, V>::operator++(int) {
    RadixTrieIterator<K, V> it = RadixTrieIterator<K, V>(*this);

    ++(*this);

    return it;
}

template <typename K, typename V>
data::RadixTrieNode<K, V> * data::RadixTrieIterator<K, V>::next_node_up(RadixTrieNode<K, V> * node) {
    std::vector<RadixTrieNode<K, V> *> * siblings;

    if (node->parent) {
        siblings = &node->parent->children;
    } else {
        siblings = this->top_nodes;
    }

    for (size_t i = 0; i < siblings->size(); i++) {
        if ((*siblings)[i] != node) {
            continue;
        }

        if (i < (siblings->size() - 1)) {
            RadixTrieNode<K, V> * out = (*siblings)[i + 1];

            while (!out->val.has_value()) {
                // A leaf node cannot have a null value; at some point we will reach
                // a non-null node
                out = out->children[0];
            }

            return out;
        }

        if (node->parent) {
            return this->next_node_up(node->parent);
        }

        return nullptr;
    }

    return nullptr;
}

template <typename K, typename V>
bool data::RadixTrieIterator<K, V>::operator==(const RadixTrieIterator<K, V> &it) const {
    // There is no need to check if "top_nodes" is equal. Unless you're doing
    // something weird, a single node can't be shared by more than one radix trie
    return this->curr_node == it.curr_node && this->end == it.end;
}

template <typename K, typename V>
bool data::RadixTrieIterator<K, V>::operator!=(const RadixTrieIterator<K, V> &it) const {
    return !(*this == it);
}

template <typename K, typename V>
typename data::RadixTrieIterator<K, V>::value_type data::RadixTrieIterator<K, V>::operator*() const {
    return std::pair(this->curr_node->full_key(), &this->curr_node->val.value());
}

#endif
