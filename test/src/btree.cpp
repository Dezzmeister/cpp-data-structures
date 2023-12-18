#include "../include/utils.h"
#include "../../include/structures/btree.h"

void btree_tests() {
    data::test::tests["btree"]["does not leak memory for simple types"] = []() {
        data::BTree<int, int, 20> tree;

        size_t exp_size = 0;

        for (int i = 0; i < 10000; i++) {
           int val = rand () % 2000;
           std::optional<int> old_val = tree.put(val, val * 2);
           expect(tree.get(val) == val * 2);

           if (!old_val.has_value()) {
               exp_size++;
           }
        }

        expect(tree.size() == exp_size);
        expect(tree.is_balanced());
        expect(tree.is_full_enough());
    };

    data::test::tests["btree"]["does not leak memory for complex types"] = []() {
        struct complex_type {
            char * mem;
            int val;

            complex_type() : mem(nullptr), val(0) {}

            complex_type(int val) : mem(new char[10]), val(val) {}

            ~complex_type() {
                if (this->mem) {
                    delete[] this->mem;
                }
            }

            complex_type(const complex_type &other) : mem(nullptr), val(other.val) {
                this->mem = new char[10];

                if (other.mem) {
                    for (size_t i = 0; i < 10; i++) {
                        this->mem[i] = other.mem[i];
                    }
                }
            }

            complex_type(complex_type &&other) : mem(nullptr), val(other.val) {
                this->mem = other.mem;
                other.mem = nullptr;
            }

            void operator=(const complex_type &other) {
                this->val = other.val;

                if (other.mem) {
                    if (this->mem) {
                        delete[] this->mem;
                    }

                    this->mem = new char[10];

                    for (size_t i = 0; i < 10; i++) {
                        this->mem[i] = other.mem[i];
                    }
                } else {
                    this->mem = nullptr;
                }
            }

            void operator=(complex_type &&other) {
                this->val = other.val;

                if (this->mem) {
                    delete[] this->mem;
                }

                this->mem = other.mem;
                other.mem = nullptr;
            }

            bool operator<(const complex_type &other) const {
                return this->val < other.val;
            }

            bool operator==(const complex_type &other) const {
                return this->val == other.val;
            }
        };

        data::BTree<complex_type, int, 20> tree;

        size_t exp_size = 0;

        for (size_t i = 0; i < 10000; i++) {
            int val = rand() % 2000;
            std::optional<int> old_val = tree.put(val, val);
            expect(tree.get(val) == val);

            if (!old_val.has_value()) {
                exp_size++;
            }
        }

        expect(tree.size() == exp_size);
        expect(tree.is_balanced());
        expect(tree.is_full_enough());
    };
}
