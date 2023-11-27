#include <set>

#include "../include/utils.h"
#include "../../include/structures/sorted_vec.h"

namespace {
    data::SortedVec<int> make_vec() {
        data::SortedVec<int> vec;

        vec.append_unsorted({
            7, 7, 8, 9, 11, 13, 13, 17, 18
        });

        return vec;
    }

    template <typename T>
    data::SortedVec<T> make_vec_insert() {
        data::SortedVec<T> vec;

        vec.put(11);
        vec.put(13);
        vec.put(7);
        vec.put(7);
        vec.put(18);
        vec.put(8);
        vec.put(9);
        vec.put(13);
        vec.put(17);

        return vec;
    }
}

void sorted_vec_tests() {
    data::test::tests["sorted vec"]["inserting, getting, and deleting elements"] = []() {
        data::SortedVec<int> vec = make_vec_insert<int>();
        data::SortedVec<int> exp_vec = make_vec();

        expect(vec.size() == exp_vec.size());
        for (size_t i = 0; i < vec.size(); i++) {
            expect(vec[i] == exp_vec[i]);
            expect(vec.get(i) == exp_vec.get(i));
        }

        for (int i = vec.size() - 1; i >= 0; i--) {
            expect(vec.del(i) == exp_vec[i]);
            expect(vec.size() == i);
        }
    };

    data::test::tests["sorted vec"]["resizing (memory stress test)"] = []() {
        data::SortedVec<int> vec;
        std::multiset<int> exp_set;

        for (size_t i = 0; i < 1000; i++) {
            const int item = rand() % 1000;

            exp_set.insert(item);
            vec.put(item);
            
            expect(vec.size() == i + 1);
        }

        expect(vec.cap() == 2048);

        auto it = std::begin(exp_set);

        for(size_t i = 0; i < vec.size(); i++, it++) {
            expect(vec[i] == *it);
        }

        expect(vec.size() == exp_set.size());

        auto rit = std::rbegin(exp_set);

        for (int i = vec.size() - 1; i >= 500; i--, rit++) {
            expect(vec.del(i) == *rit);
        }

        expect(vec.cap() == 2048);
        vec.shrink();
        expect(vec.cap() == 1024);
    };

    data::test::tests["sorted vec"]["sorts complex types"] = []() {
        struct complex_type {
            int item;

            complex_type() = default;

            complex_type(int item) : item(item) {}

            bool operator<(const complex_type &other) const {
                return item < other.item;
            }
        };

        data::SortedVec<complex_type> vec = make_vec_insert<complex_type>();
        data::SortedVec<int> exp_vec = make_vec();

        expect(vec.size() == exp_vec.size());

        for (size_t i = 0; i < vec.size(); i++) {
            expect(vec[i].item == exp_vec[i]);
        }
    };

    data::test::tests["sorted vec"]["resizes with non trivially copyable type"] = []() {
        struct complex_type {
            int item;
            size_t len;
            char * bytes;

            complex_type() : item(0), len(rand() % 50), bytes((char *) malloc(this->len)) {
                for (size_t i = 0; i < this->len; i++) {
                    this->bytes[i] = i;
                }
            }

            complex_type(int item) : item(item), len(rand() % 50), bytes((char *) malloc(this->len)) {
                for (size_t i = 0; i < this->len; i++) {
                    this->bytes[i] = i;
                }
            }

            complex_type(const complex_type& other) : item(other.item), len(other.len), bytes((char *) malloc(this->len)) {
                for (size_t i = 0; i < this->len; i++) {
                    this->bytes[i] = other.bytes[i];
                }
            }

            complex_type(complex_type &&other) : item(other.item), len(other.len), bytes(other.bytes) {
                other.bytes = nullptr;
            }

            ~complex_type() {
                if (this->bytes) {
                    free(this->bytes);
                }
            }

            void operator=(const complex_type &other) {
                if (this->bytes) {
                    free(this->bytes);
                }

                this->len = other.len;
                this->bytes = (char *) malloc(this->len);
                this->item = other.item;

                for (size_t i = 0; i < this->len; i++) {
                    this->bytes[i] = other.bytes[i];
                }
            }

            void operator=(complex_type &&other) {
                if (this->bytes) {
                    free(this->bytes);
                }

                this->bytes = other.bytes;
                this->len = other.len;
                this->item = other.item;

                other.bytes = nullptr;
            }

            bool operator<(const complex_type &other) {
                return this->item < other.item;
            }
        };

        data::SortedVec<complex_type> vec;
        std::multiset<int> exp_set;

        for (size_t i = 0; i < 400; i++) {
            const int item = rand();
            vec.put(item);
            exp_set.insert(item);
        }

        expect(vec.cap() == 1024);
    };

    data::test::tests["sorted vec"]["equality"] = []() {
        data::SortedVec<int> vec = make_vec_insert<int>();
        data::SortedVec<int> exp_vec = make_vec();

        expect(vec == vec);
        expect(vec == exp_vec);

        vec.del(0);
        expect(vec == vec);
        expect(vec != exp_vec);
    };

    data::test::tests["sorted vec"]["moving and copying"] = []() {
        data::SortedVec<int> vec = make_vec();
        data::SortedVec<int> copy_constructed(vec);
        data::SortedVec<int> move_constructed(std::move(vec));

        data::SortedVec<int> vec2 = make_vec();
        data::SortedVec<int> copy_assigned = vec2;
        data::SortedVec<int> move_assigned = std::move(vec2);

        data::SortedVec<int> vec3 = make_vec();

        expect(vec3.size());
        expect(vec3.size() == copy_constructed.size());
        expect(copy_constructed.size() == copy_assigned.size());
        expect(copy_assigned.size() == move_constructed.size());
        expect(move_constructed.size() == move_assigned.size());

        for(size_t i = 0; i < vec.size(); i++) {
            expect(vec3[i] == copy_constructed[i]);
            expect(copy_constructed[i] == copy_assigned[i]);
            expect(copy_assigned[i] == move_constructed[i]);
            expect(move_constructed[i] == move_assigned[i]);
        }
    };
}
