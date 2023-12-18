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

#ifdef TEST
namespace {
    struct DepthResult {
        const size_t max_depth;
        const bool is_only_depth;

        DepthResult(size_t max_depth, bool is_only_depth) : max_depth(max_depth), is_only_depth(is_only_depth) {}
    };
}
#endif

namespace data {
    template <Ord K, typename V, const size_t N>
    class BTree {
        private:
            BTreeNode<K, V, N> * root;
            size_t len;

            void split_node(std::vector<BTreeNode<K, V, N> *> &parents, BTreeNode<K, V, N> * node);

        public:
            BTree();

            ~BTree();

            std::optional<V> get(const K &key) const;

            /**
             * Inserts a KV pair into the btree. If the key already exists, returns the previous value and
             * replaces it.
             */
            std::optional<V> put(const K key, const V val);

            std::optional<V> del(const K key);

            size_t size() const;

#ifdef TEST
            void debug_print() const;

            /**
             * Checks that the btree is balanced, i.e. that every leaf node is at the same depth.
             */
            bool is_balanced() const;

            DepthResult depth(BTreeNode<K, V, N> * node) const;

            /**
             * Checks that the btree satisfies the property that every internal node (non leaf and non root) has
             * at least N / 2 children. Simultaneously checks that every internal node has k + 1 children where k
             * is the number of keys in the node.
             */
            bool is_full_enough() const;
            bool is_full_enough(BTreeNode<K, V, N> * node) const;
#endif
    };
}

template <data::Ord K, typename V, const size_t N>
data::BTree<K, V, N>::BTree() : root(new BTreeNode<K, V, N>()), len(0) {}

template <data::Ord K, typename V, const size_t N>
data::BTree<K, V, N>::~BTree() {
    delete this->root;
}

template <data::Ord K, typename V, const size_t N>
std::optional<V> data::BTree<K, V, N>::get(const K &key) const {
    const BTreeNode<K, V, N> * curr_node = this->root;

    while (!curr_node->is_leaf()) {
        size_t index = curr_node->items.lower_bound(key);

        if (index == curr_node->items.size()) {
            curr_node = curr_node->post;
        } else if (curr_node->items[index].key == key) {
            return std::optional(curr_node->items[index].val);
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

template <data::Ord K, typename V, const size_t N>
std::optional<V> data::BTree<K, V, N>::put(const K key, const V val) {
    std::vector<BTreeNode<K, V, N> *> parents;
    BTreeNode<K, V, N> * curr_node = this->root;

    // First we have to find the right place to insert the key
    while (!curr_node->is_leaf()) {
        size_t index = curr_node->items.lower_bound(key);

        if (index == curr_node->items.size()) {
            parents.push_back(curr_node);
            curr_node = curr_node->post;
        } else if (curr_node->items[index].key == key) {
            // The key already exists, update and return early
            const V old_val = curr_node->items[index].val;
            curr_node->items[index].val = val;

            return std::optional<V>(old_val);
        } else {
            parents.push_back(curr_node);
            curr_node = curr_node->items[index].pre;
        }
    }

    size_t index = curr_node->items.lower_bound(key);

    if (index < curr_node->items.size() && curr_node->items[index].key == key) {
        const V old_val = curr_node->items[index].val;
        curr_node->items[index].val = val;

        return std::optional<V>(old_val);
    }

    // `pre` is null because this is a leaf node
    BTreeEntry<K, V, N> entry(key, val, nullptr);
    curr_node->items.put(entry);
    this->len++;

    if (curr_node->is_overflowed()) {
        // Split the node
        this->split_node(parents, curr_node);
    }

    return std::nullopt;
}

template <data::Ord K, typename V, const size_t N>
void data::BTree<K, V, N>::split_node(std::vector<BTreeNode<K, V, N> *> &parents, BTreeNode<K, V, N> * left_node) {
    BTreeEntry<K, V, N> pivot = left_node->items[N / 2];
    BTreeNode<K, V, N> * right_node = new BTreeNode<K, V, N>();
    right_node->items = left_node->items.substr(N / 2 + 1);
    left_node->items.truncate(N / 2);

    if (!left_node->is_leaf()) {
        right_node->post = new BTreeNode<K, V, N>(*left_node->post);

        if (left_node->post) {
            delete left_node->post;
        }

        left_node->post = new BTreeNode<K, V, N>(*pivot.pre);

        if (pivot.pre) {
            delete pivot.pre;
            pivot.pre = nullptr;
        }
    }

    if (!parents.size()) {
        // Split the root

        BTreeNode<K, V, N> * new_root = new BTreeNode<K, V, N>();
        size_t index = new_root->items.put(pivot);
        new_root->items[index].pre = left_node;
        new_root->post = right_node;
        this->root = new_root;

        return;
    }

    BTreeNode<K, V, N> * parent = parents.back();
    parents.pop_back();
    pivot.pre = nullptr;
 
    // TODO: Find a better way to do this
    for (size_t i = 0; i < parent->items.size(); i++) {
        if (parent->items[i].pre == left_node) {
            parent->items[i].pre = nullptr;
            break;
        }
    }

    size_t index = parent->items.put(pivot);
    parent->items[index].pre = left_node;

    // Now find pivot's right sibling and update its `pre` pointer, or `post` if it has
    // no right sibling
    if (index == (parent->items.size() - 1)) {
        parent->post = right_node;
    } else {
        parent->items[index + 1].pre = right_node;
    }

    if (parent->is_overflowed()) {
        // Split parent
        this->split_node(parents, parent);
    }
}

template <data::Ord K, typename V, const size_t N>
std::optional<V> data::BTree<K, V, N>::del(const K key) {
    std::vector<BTreeNode<K, V, N> *> parents;
    BTreeNode<K, V, N> * curr_node = this->root;

    while (!curr_node->is_leaf()) {
        size_t index = curr_node->items.lower_bound(key);

        if (index == curr_node->items.size()) {
            parents.push_back(curr_node);
            curr_node = curr_node->post;
        } else if (curr_node->items[index].key == key) {
            // TODO: Delete an internal key
        } else {
            parents.push_back(curr_node);
            curr_node = curr_node->items[index].pre;
        }
    }

    size_t index = curr_node->items.lower_bound(key);

    if (index < curr_node->items.size() && curr_node->items[index].key == key) {
        // Delete a leaf key
        if (curr_node->items.size() > 1) {
            BTreeEntry<K, V, N> entry = curr_node->items.del(index);
            this->len--;

            // No need to worry about pre and post pointers because this is a leaf node
            return std::optional<V>(entry.val);
        } // TODO: Handle this case
    }

    return std::nullopt;
}

template <data::Ord K, typename V, const size_t N>
size_t data::BTree<K, V, N>::size() const {
    return this->len;
}

#ifdef TEST

template <data::Ord K, typename V, const size_t N>
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

template <data::Ord K, typename V, const size_t N>
bool data::BTree<K, V, N>::is_balanced() const {
    return this->depth(this->root).is_only_depth;
}

template <data::Ord K, typename V, const size_t N>
DepthResult data::BTree<K, V, N>::depth(BTreeNode<K, V, N> * node) const {
    std::vector<DepthResult> depths;

    for (size_t i = 0; i < node->items.size(); i++) {
        if (node->items[i].pre) {
            depths.push_back(this->depth(node->items[i].pre));
        } else {
            depths.push_back(DepthResult(0, true));
        }
    }

    if (node->post) {
        depths.push_back(this->depth(node->post));
    } else {
        depths.push_back(DepthResult(0, true));
    }

    if (!depths.size()) {
        return DepthResult(0, true);
    }

    size_t max_depth = depths[0].max_depth;
    bool is_only_depth = depths[0].is_only_depth;

    for (size_t i = 1; i < depths.size(); i++) {
        DepthResult &res = depths[i];

        if (res.max_depth != max_depth) {
            is_only_depth = false;
 
            if (res.max_depth > max_depth) {
                max_depth = res.max_depth;
            }
        }

        is_only_depth = is_only_depth && res.is_only_depth;
    }

    return DepthResult(max_depth + 1, is_only_depth);
}

template <data::Ord K, typename V, const size_t N>
bool data::BTree<K, V, N>::is_full_enough() const {
    return this->is_full_enough(this->root);
}

template <data::Ord K, typename V, const size_t N>
bool data::BTree<K, V, N>::is_full_enough(BTreeNode<K, V, N> * node) const {
    if (node->is_leaf() || node == this->root) {
        return true;
    }

    for (size_t i = 0; i < node->items.size(); i++) {
        if (!node->items[i].pre) {
            return false;
        }
    }

    if (!node->post) {
        return false;
    }

    if ((node->items.size() + 1) < (N / 2)) {
        return false;
    }

    bool acc = true;
    for (size_t i = 0; i < node->items.size(); i++) {
        acc = acc && this->is_full_enough(node->items[i].pre);
    }

    return acc && this->is_full_enough(node->post);
}

#endif
#endif
