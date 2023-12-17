#ifndef INCLUDE_STRUCTURES_BTREE_H
#define INCLUDE_STRUCTURES_BTREE_H

#include <stdlib.h>
#include <vector>

#include "../traits.h"
#include "btree_node.h"

#ifdef TEST
#include <stdio.h>
#include <queue>
#endif

namespace data {
    template <PartialOrd K, typename V, const size_t N>
    class BTree {
        private:
            BTreeNode<K, V, N> * root;
            size_t size;

        public:
            BTree();

            ~BTree();

            std::optional<V> get(const K &key) const;

#ifdef TEST
            void debug_print() const;
#endif
    };
}

template <data::PartialOrd K, typename V, const size_t N>
data::BTree<K, V, N>::BTree() : root(new BTreeNode<K, V, N>()) {}

template <data::PartialOrd K, typename V, const size_t N>
data::BTree<K, V, N>::~BTree() {
    delete this->root;
}

template <data::PartialOrd K, typename V, const size_t N>
std::optional<V> data::BTree<K, V, N>::get(const K &key) const {
    const BTreeNode<K, V, N> * curr_node = this->root;

    while (!curr_node->is_leaf()) {
        size_t index = curr_node->items.lower_bound(key);

        if (index == curr_node->items.size()) {
            curr_node = curr_node->post;
        } else if (curr_node->items[index].key == key) {
            return std::optional(curr_node->items[index + 1].val);
        } else {
            curr_node = curr_node->items[index].pre;
        }
    }

    size_t index = curr_node->items.lower_bound(key);

    if (index == curr_node->items.size()) {
        return std::nullopt;
    } else if (curr_node->items[index].key == key) {
        return std::optional(curr_node->items[index].val);
    } else {
        return std::nullopt;
    }
}

#ifdef TEST

template <data::PartialOrd K, typename V, const size_t N>
void data::BTree<K, V, N>::debug_print() const {
    std::queue<BTreeNode<K, V, N> *> nodes;
    nodes.push(this->root);
    nodes.push(nullptr);
    BTreeNode<K, V, N> * last = this->root;
    size_t i = 0;

    while (nodes.size()) {
        BTreeNode<K, V, N> * node = nodes.front();
        nodes.pop();

        if (!node) {
            printf("\n");
            i = 0;
            continue;
        }

        printf("%ld:", i);
        node->debug_print();
        printf("   ");
        i++;

        if (!node->is_leaf()) {
            for (size_t i = 0; i < node->items.size(); i++) {
                nodes.push(node->items[i].pre);
            }

            nodes.push(node->post);
        }

        if (node == last) {
            last = node->post;
            nodes.push(nullptr);
        }
    }
}

#endif
#endif
