#ifndef INCLUDE_STRUCTURES_SORTED_VEC_H
#define INCLUDE_STRUCTURES_SORTED_VEC_H

#include <algorithm>

#ifdef TEST
#include <vector>
#endif

#include "../traits.h"

namespace data {
    /**
     * A sorted vector with variable capacity. The vector grows when the number of elements reaches
     * 2/3 of its capacity. NB: The vector does not shrink on its own - you have to call `shrink()`.
     * 
     * See SortedArray for an implementation of a sorted array with a fixed size.
     */
    template <PartialOrd T>
    class SortedVec {
        private:
            static constexpr size_t INITIAL_CAPACITY = 64;
            T * items;
            size_t len;
            size_t capacity;

            /**
             * Moving a sorted vec consists of freeing the target's items and copying the
             * items pointer to the target. To prevent double freeing, `items` is set to null in
             * the source object. Moving a vector does not invalidate it, so the vector needs
             * to be able to recreate its items array if necessary.
             */
            void resurrect_array(size_t capacity = INITIAL_CAPACITY);
            
        public:
            SortedVec();

            SortedVec(size_t capacity);

            SortedVec(const SortedVec<T> &other);

            SortedVec(SortedVec<T> &&other);

            ~SortedVec();

            void operator=(const SortedVec<T> &other);

            void operator=(SortedVec<T> &&other);

            size_t size() const;

            size_t cap() const;

            /**
             * Returns the position of the first element in the vector that is not less than `item`
             * (i.e., the first element that is greater than or equal to `item`).
             */
            size_t lower_bound(const T &item) const;

            void put(const T item);

            T& operator[](size_t i);

            const T& operator[](size_t i) const;

            T get(size_t i) const;

            /**
             * Deletes the element at the given position. Note that this does NOT resize the underlying
             * array. If you are bulk deleting elements, you might want to call shrink() afterward to 
             * reduce the vector's capacity and free up unused memory.
             */
            T del(size_t i);

            void shrink();

            bool operator==(const SortedVec<T> &other) const requires Eq<T>;

#ifdef TEST
            void append_unsorted(const T item);

            void append_unsorted(const std::vector<T> &items);
#endif
    };
}

template <data::PartialOrd T>
data::SortedVec<T>::SortedVec() 
    : items(new T[SortedVec<T>::INITIAL_CAPACITY]), len(0), capacity(SortedVec<T>::INITIAL_CAPACITY) {}

template <data::PartialOrd T>
data::SortedVec<T>::SortedVec(size_t capacity) : items(new T[capacity]), len(0), capacity(capacity) {}

template <data::PartialOrd T>
data::SortedVec<T>::SortedVec(const SortedVec<T> &other) : items(new T[other.capacity]), len(other.len), capacity(other.capacity) {
    std::copy(other.items, other.items + other.len, this->items);
}

template <data::PartialOrd T>
data::SortedVec<T>::SortedVec(SortedVec<T> &&other) : items(other.items), len(other.len), capacity(other.capacity) {
    other.items = nullptr;
    other.capacity = 0;
    other.len = 0;
}

template <data::PartialOrd T>
data::SortedVec<T>::~SortedVec() {
    if (this->items) {
        delete[] this->items;
    }
}

template <data::PartialOrd T>
void data::SortedVec<T>::operator=(const SortedVec<T> &other) {
    if (this->capacity != other.capacity) {
        T * new_items = new T[other.capacity];

        if (this->items) {
            delete[] this->items;
        }

        this->items = new_items;
        this->capacity = other.capacity;
    }

    if (!this->items) {
        this->resurrect_array(other.capacity);
    }

    this->len = other.len;
    std::copy(other.items, other.items + other.len, this->items);
}

template <data::PartialOrd T>
void data::SortedVec<T>::operator=(SortedVec<T> &&other) {
    if (this->items) {
        delete[] this->items;
    }

    this->items = other.items;
    this->capacity = other.capacity;
    this->len = other.len;

    other.items = nullptr;
    other.capacity = 0;
    other.len = 0;
}

template <data::PartialOrd T>
void data::SortedVec<T>::resurrect_array(size_t capacity) {
    this->items = new T[capacity];
    this->capacity = capacity;
}

template <data::PartialOrd T>
size_t data::SortedVec<T>::size() const {
    return this->len;
}

template <data::PartialOrd T>
size_t data::SortedVec<T>::cap() const {
    return this->capacity;
}

template <data::PartialOrd T>
size_t data::SortedVec<T>::lower_bound(const T &item) const {
    size_t l = 0;
    size_t r = this->len;

    while (l < r) {
        size_t m = (l + r) >> 1;

        if (this->items[m] < item) {
            l = m + 1;
        } else {
            r = m;
        }
    }

    return l;
}

template <data::PartialOrd T>
void data::SortedVec<T>::put(const T item) {
    if (!this->items) {
        this->resurrect_array();
    }

    size_t index = this->lower_bound(item);
 
    for (size_t i = this->len; i > index; i--) {
        this->items[i] = this->items[i - 1];
    }

    this->items[index] = item;
    this->len++;

    if (this->len * 1.5 > this->capacity) {
        this->capacity *= 2;
        T * new_items = new T[this->capacity];

        std::move(this->items, this->items + this->len, new_items);
        delete[] this->items;

        this->items = new_items;
    }
}

template <data::PartialOrd T>
T& data::SortedVec<T>::operator[](size_t i) {
    return this->items[i];
}

template <data::PartialOrd T>
const T& data::SortedVec<T>::operator[](size_t i) const {
    return this->items[i];
}

template <data::PartialOrd T>
T data::SortedVec<T>::get(size_t i) const {
    return this->items[i];
}

template <data::PartialOrd T>
T data::SortedVec<T>::del(size_t i) {
    T out = this->items[i];

    if (this->len > 1) {
        for (size_t j = i; j < this->len - 1; j++) {
            this->items[j] = this->items[j + 1];
        }
    }

    this->len--;

    return out;
}

template <data::PartialOrd T>
void data::SortedVec<T>::shrink() {
    if (this->len * 3 >= this->capacity) {
        return;
    }

    // Divide capacity by 2 and make it a power of 2

    this->capacity >>= 1;

    size_t c = this->capacity;
    size_t pos = 0;

    while (c >>= 1) {
        pos++;
    }

    this->capacity &= (1 << pos);

    T * new_items = new T[this->capacity];
    
    if (this->items) {
        std::move(this->items, this->items + this->len, new_items);
        delete[] this->items;
    }

    this->items = new_items;
}

template <data::PartialOrd T>
bool data::SortedVec<T>::operator==(const SortedVec<T> &other) const requires data::Eq<T> {
    if (this->len != other.len) {
        return false;
    }

    if (this->items == other.items) {
        return true;
    }

    for (size_t i = 0; i < this->len; i++) {
        if (this->items[i] != other.items[i]) {
            return false;
        }
    }

    return true;
}

#ifdef TEST
template <data::PartialOrd T>
void data::SortedVec<T>::append_unsorted(const T item) {
    this->items[this->len] = item;
    this->len++;

    if (this->len * 1.5 > this->capacity) {
        this->capacity *= 2;
        T * new_items = new T[this->capacity];

        std::move(this->items, this->items + this->len, new_items);
        delete[] this->items;

        this->items = new_items;
    }
}

template <data::PartialOrd T>
void data::SortedVec<T>::append_unsorted(const std::vector<T> &items) {
    for (const T &item : items) {
        this->append_unsorted(item);
    }
}

#endif
#endif
