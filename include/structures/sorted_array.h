#ifndef INCLUDE_STRUCTURES_SORTED_ARRAY_H
#define INCLUDE_STRUCTURES_SORTED_ARRAY_H

#include <stdlib.h>
#ifdef TEST
#include <vector>
#endif

#include "../traits.h"

namespace data {
    /**
     * A sorted array with a fixed capacity. The size is known at compile time, so the entire array
     * is stored directly in the object instead of somewhere else on the heap.
     *
     * See SortedVec for an implementation of a resizable sorted vector.
     */
    template <PartialOrd T, const size_t N>
    class SortedArray {
        template <PartialOrd U, const size_t M> 
        friend class SortedArray;

        private:
            T items[N] = {0};
            size_t len;

        public:
            SortedArray();

            // This looks like a copy constructor but it isn't because SortedArray<T, M> is not
            // SortedArray<T, N>
            template <const size_t M>
            SortedArray(const SortedArray<T, M> &other);

            template <const size_t M>
            void operator=(const SortedArray<T, M> &other);

            size_t size() const;

            /**
             * Returns the position of the first element in the array that is not less than `item`
             * (i.e., the first element that is greater than or equal to `item`).
             */
            size_t lower_bound(const T &item) const;

            void put(const T item);

            const T& operator[](size_t i) const;

            T& operator[](size_t i);

            T get(size_t i) const;

            T del(size_t i);

            template <const size_t M>
            bool operator==(const SortedArray<T, M> &other) const requires Eq<T>;

            /**
             * Copies everything from the given `from` index (inclusive) to the given `to` index (exclusive)
             * into a new array of the same type and parameterization.
             */
            SortedArray<T, N> substr(size_t from, size_t to) const;

            /**
             * Copies everything from the given `from` index (inclusive) to the end of the array into
             * a new array of the same type and parameterization.
             */
            SortedArray<T, N> substr(size_t from) const;

            void truncate(size_t new_len);

#ifdef TEST
            void append_unsorted(const T item);

            void append_unsorted(const std::vector<T> &items);
#endif
    };
}

template <data::PartialOrd T, const size_t N>
data::SortedArray<T, N>::SortedArray() : len(0) {}

template <data::PartialOrd T, const size_t N>
template <const size_t M>
data::SortedArray<T, N>::SortedArray(const SortedArray<T, M> &other) {
    if (other.len <= N) {
        for (size_t i = 0; i < other.len; i++) {
            this->items[i] = other.items[i];
        }
        this->len = other.len;
    } else {
        for (size_t i = 0; i < N; i++) {
            this->items[i] = other.items[i];
        }
        this->len = N;
    }
}

template <data::PartialOrd T, const size_t N>
template <const size_t M>
void data::SortedArray<T, N>::operator=(const SortedArray<T, M> &other) {
    if (other.len <= N) {
        for (size_t i = 0; i < other.len; i++) {
            this->items[i] = other.items[i];
        }
        this->len = other.len;
    } else {
        for (size_t i = 0; i < N; i++) {
            this->items[i] = other.items[i];
        }
        this->len = N;
    }
}

template <data::PartialOrd T, const size_t N>
size_t data::SortedArray<T, N>::size() const {
    return this->len;
}

template <data::PartialOrd T, const size_t N>
size_t data::SortedArray<T, N>::lower_bound(const T &item) const {
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

template <data::PartialOrd T, const size_t N>
void data::SortedArray<T, N>::put(const T item) {
    if (this->len == N) {
        throw "Out of memory";
    }

    size_t index = this->lower_bound(item);

    for (size_t i = this->len; i > index; i--) {
        this->items[i] = this->items[i - 1];
    }

    this->items[index] = item;
    this->len++;
}

template <data::PartialOrd T, const size_t N>
const T& data::SortedArray<T, N>::operator[](size_t i) const {
    return this->items[i];
}

template <data::PartialOrd T, const size_t N>
T& data::SortedArray<T, N>::operator[](size_t i) {
    return this->items[i];
}

template <data::PartialOrd T, const size_t N>
T data::SortedArray<T, N>::get(size_t i) const {
    return this->items[i];
}

template <data::PartialOrd T, const size_t N>
T data::SortedArray<T, N>::del(size_t i) {
    T out = this->items[i];

    if (this->len > 1) {
        for (size_t j = i; j < this->len - 1; j++) {
            this->items[j] = this->items[j + 1];
        }
    }

    this->len--;

    return out;
}

template <data::PartialOrd T, const size_t N>
template <const size_t M>
bool data::SortedArray<T, N>::operator==(const SortedArray<T, M> &other) const requires data::Eq<T> {
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

template <data::PartialOrd T, const size_t N>
data::SortedArray<T, N> data::SortedArray<T, N>::substr(size_t from, size_t to) const {
    data::SortedArray<T, N> out;
    out.len = to - from;

    for (size_t i = 0; i < out.len; i++) {
        out.items[i] = this->items[from + i];
    }

    return out;
}

template <data::PartialOrd T, const size_t N>
data::SortedArray<T, N> data::SortedArray<T, N>::substr(size_t from) const {
    return this->substr(from, this->len);
}

template <data::PartialOrd T, const size_t N>
void data::SortedArray<T, N>::truncate(size_t new_len) {
    this->len = new_len;
}

#ifdef TEST
template <data::PartialOrd T, const size_t N>
void data::SortedArray<T, N>::append_unsorted(const T item) {
    this->items[len] = item;
    this->len++;
}

template <data::PartialOrd T, const size_t N>
void data::SortedArray<T, N>::append_unsorted(const std::vector<T> &items) {
    for (const T& item : items) {
        this->append_unsorted(item);
    }
}
#endif
#endif
