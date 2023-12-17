#ifndef INCLUDE_STRUCTURES_BTREE_NODES_H
#define INCLUDE_STRUCTURES_BTREE_NODES_H

#include <optional>
#include <stdlib.h>

#include "sorted_array.h"
#include "../traits.h"

#ifdef TEST
#include <stdio.h>
#endif

namespace data {
    template <PartialOrd K, typename V, const size_t N>
    struct BTreeNode;

    template <PartialOrd K, typename V, const size_t N>
    struct BTreeEntry {
        K key;
        V val;
        BTreeNode<K, V, N> * pre;

        BTreeEntry();

        BTreeEntry(const BTreeEntry<K, V, N> &other);

        BTreeEntry(BTreeEntry<K, V, N> &&other);

        void operator=(const BTreeEntry<K, V, N> &other);

        void operator=(BTreeEntry<K, V, N> &&other);

        ~BTreeEntry();

        BTreeEntry(K key);

        BTreeEntry(K key, V val, BTreeNode<K, V, N> * pre = nullptr);

        bool operator<(const BTreeEntry<K, V, N>  &other) const;
    };

    template <PartialOrd K, typename V, const size_t N>
    struct BTreeNode {
        SortedArray<BTreeEntry<K, V, N>, N> items;
        BTreeNode<K, V, N> * post;

        BTreeNode();

        BTreeNode(const BTreeNode<K, V, N> &other);

        BTreeNode(BTreeNode<K, V, N> &&other);

        void operator=(const BTreeNode<K, V, N> &other);

        void operator=(BTreeNode<K, V, N> &&other);

        ~BTreeNode();

        bool is_leaf() const;

        bool is_overflowed() const;

#ifdef TEST
        void debug_print() const;
#endif
    };
}

template <data::PartialOrd K, typename V, const size_t N>
data::BTreeEntry<K, V, N>::BTreeEntry() : key(K()), val(V()), pre(nullptr) {}

template <data::PartialOrd K, typename V, const size_t N>
data::BTreeEntry<K, V, N>::BTreeEntry(const BTreeEntry<K, V, N> &other) : key(K(other.key)), val(V(other.val)), pre(nullptr) {
    if (other.pre) {
        this->pre = new BTreeNode<K, V, N>(*other.pre);
    }
}

template <data::PartialOrd K, typename V, const size_t N>
data::BTreeEntry<K, V, N>::BTreeEntry(BTreeEntry<K, V, N> &&other) : key(K(std::move(other.key))), val(V(std::move(other.val))), pre(other.pre) {
    other.pre = nullptr;
}

template <data::PartialOrd K, typename V, const size_t N>
void data::BTreeEntry<K, V, N>::operator=(const data::BTreeEntry<K, V, N> &other) {
    if (this->pre) {
        delete this->pre;
    }

    if (other.pre) {
        this->pre = new BTreeNode<K, V, N>(*other.pre);
    } else {
        this->pre = nullptr;
    }

    this->key = K(other.key);
    this->val = V(other.val);
}

template <data::PartialOrd K, typename V, const size_t N>
void data::BTreeEntry<K, V, N>::operator=(data::BTreeEntry<K, V, N> &&other) {
    if (this->pre) {
        delete this->pre;
    }

    this->pre = other.pre;
    other.pre = nullptr;

    this->key = other.key;
    this->val = other.val;
}

template <data::PartialOrd K, typename V, const size_t N>
data::BTreeEntry<K, V, N>::BTreeEntry(K key) : key(key), val(V()), pre(nullptr) {}

template <data::PartialOrd K, typename V, const size_t N>
data::BTreeEntry<K, V, N>::BTreeEntry(K key, V val, BTreeNode<K, V, N> * pre) : key(key), val(val), pre(pre) {}

template <data::PartialOrd K, typename V, const size_t N>
data::BTreeEntry<K, V, N>::~BTreeEntry() {
    if (this->pre) {
        delete this->pre;
    }
}

template <data::PartialOrd K, typename V, const size_t N>
bool data::BTreeEntry<K, V, N>::operator<(const BTreeEntry<K, V, N> &other) const {
    return this->key < other.key;
}

template <data::PartialOrd K, typename V, const size_t N>
data::BTreeNode<K, V, N>::BTreeNode() : items({}), post(nullptr) {}

template <data::PartialOrd K, typename V, const size_t N>
data::BTreeNode<K, V, N>::BTreeNode(const BTreeNode<K, V, N> &other) : items(SortedArray<BTreeEntry<K, V, N>, N>(other.items)) {
    if (other.post) {
        this->post = new BTreeNode<K, V, N>(*other.post);
    } else {
        this->post = nullptr;
    }
}

template <data::PartialOrd K, typename V, const size_t N>
data::BTreeNode<K, V, N>::BTreeNode(BTreeNode<K, V, N> &&other) : items(other.items), post(other.post) {
    other.post = nullptr;
}

template <data::PartialOrd K, typename V, const size_t N>
void data::BTreeNode<K, V, N>::operator=(const BTreeNode<K, V, N> &other) {
    if (this->post) {
        delete this->post;
    }

    if (other.post) {
        this->post = new BTreeNode<K, V, N>(*other.post);
    } else {
        this->post = nullptr;
    }

    this->items = SortedArray<BTreeEntry<K, V, N>, N>(other.items);
}

template <data::PartialOrd K, typename V, const size_t N>
void data::BTreeNode<K, V, N>::operator=(BTreeNode<K, V, N> &&other) {
    this->post = other.post;
    this->items = std::move(other.items);

    other.post = nullptr;
}

template <data::PartialOrd K, typename V, const size_t N>
data::BTreeNode<K, V, N>::~BTreeNode() {
    if (this->post) {
        delete this->post;
    }
}

template <data::PartialOrd K, typename V, const size_t N>
bool data::BTreeNode<K, V, N>::is_leaf() const {
    return !this->post;
}

template <data::PartialOrd K, typename V, const size_t N>
bool data::BTreeNode<K, V, N>::is_overflowed() const {
    return this->items.size() == N;
}

#ifdef TEST

template <data::PartialOrd K, typename V, const size_t N>
void data::BTreeNode<K, V, N>::debug_print() const {
    printf("|");
    for (size_t i = 0; i < this->items.size(); i++) {
        printf("%d|", this->items[i].key);
    }
}

#endif
#endif
