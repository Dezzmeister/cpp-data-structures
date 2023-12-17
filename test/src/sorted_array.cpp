#include "../include/utils.h"
#include "../../include/structures/sorted_array.h"

namespace {
    template <const size_t N>
    data::SortedArray<int, N> make_array() {
        data::SortedArray<int, N> arr;

        arr.append_unsorted({
            7, 7, 8, 9, 11, 13, 13, 17, 18
        });

        return arr;
    }

    template <typename T, const size_t N>
    data::SortedArray<T, N> make_array_insert() {
        data::SortedArray<T, N> arr;

        arr.put(11);
        arr.put(13);
        arr.put(7);
        arr.put(7);
        arr.put(18);
        arr.put(8);
        arr.put(9);
        arr.put(13);
        arr.put(17);

        return arr;
    }
}

void sorted_array_tests() {
    data::test::tests["sorted array"]["inserting, getting, and deleting elements"] = []() {
        data::SortedArray<int, 16> arr = make_array_insert<int, 16>();
        data::SortedArray<int, 16> exp_arr = make_array<16>();

        expect(arr.size() == exp_arr.size());
        for (size_t i = 0; i < arr.size(); i++) {
            expect(arr[i] == exp_arr[i]);
            expect(arr.get(i) == exp_arr.get(i));
        }

        for (int i = arr.size() - 1; i >= 0; i--) {
            expect(arr.del(i) == exp_arr[i]);
            expect(arr.size() == (size_t) i);
        }
    };

    data::test::tests["sorted array"]["sorts complex types"] = []() {
        struct complex_type {
            int item;

            complex_type() = default;

            complex_type(int item) : item(item) {}

            bool operator<(const complex_type &other) const {
                return item < other.item;
            }
        };

        data::SortedArray<complex_type, 16> arr = make_array_insert<complex_type, 16>();
        data::SortedArray<int, 16> exp_arr = make_array<16>();

        expect(arr.size() == exp_arr.size());

        for (size_t i = 0; i < arr.size(); i++) {
            expect(arr[i].item == exp_arr[i]);
        }
    };

    data::test::tests["sorted array"]["equality"] = []() {
        data::SortedArray<int, 16> arr = make_array_insert<int, 16>();
        data::SortedArray<int, 24> exp_arr = make_array<24>();

        expect(arr == arr);
        expect(arr == exp_arr);

        arr.del(0);
        expect(arr == arr);
        expect(arr != exp_arr);
    };

    data::test::tests["sorted array"]["moving and copying"] = []() {
        data::SortedArray<int, 16> arr = make_array<16>();
        data::SortedArray<int, 24> copy_constructed(arr);
        data::SortedArray<int, 24> move_constructed(std::move(arr));

        data::SortedArray<int, 16> arr2 = make_array<16>();
        data::SortedArray<int, 24> copy_assigned = arr2;
        data::SortedArray<int, 24> move_assigned = std::move(copy_assigned);

        data::SortedArray<int, 32> arr3 = make_array<32>();

        expect(arr3.size());
        expect(arr3.size() == copy_constructed.size());
        expect(copy_constructed.size() == copy_assigned.size());
        expect(copy_assigned.size() == move_constructed.size());
        expect(move_constructed.size() == move_assigned.size());

        for(size_t i = 0; i < arr3.size(); i++) {
            expect(arr3[i] == copy_constructed[i]);
            expect(copy_constructed[i] == copy_assigned[i]);
            expect(copy_assigned[i] == move_constructed[i]);
            expect(move_constructed[i] == move_assigned[i]);
        }
    };

    data::test::tests["sorted array"]["inserting many elements"] = []() {
        data::SortedArray<int, 1024> arr;

        for (size_t i = 0; i < 1000; i++) {
            expect(arr.size() == i);
            arr.put(rand() % 1000);
            expect(arr.size() == i + 1);
        }

        for (size_t i = 0; i < 999; i++) {
            expect(arr[i] <= arr[i + 1]);
        }
    };

    data::test::tests["sorted array"]["throws when out of memory"] = []() {
        data::SortedArray<int, 64> arr;

        for (size_t i = 0; i < 64; i++) {
            arr.put(rand() % 1000);
        }

        try {
            arr.put(rand() % 1000);
            fail_test();
        } catch (const char * const err) {
            expect(true);
        }
    };

    data::test::tests["sorted array"]["doesn't leak memory"] = []() {
        struct complex_type {
            char * mem;
            int val;

            complex_type(int val) : mem(new char[10]), val(val) {}

            ~complex_type() {
                delete[] this->mem;
            }

            bool operator<(const complex_type &other) const {
                return this->val < other.val;
            }
        };

        data::SortedArray<int, 256> arr;

        for (size_t i = 0; i < 128; i++) {
            expect(arr.size() == i);
            arr.put(rand() % 1000);
            expect(arr.size() == i + 1);
        }

        for (size_t i = 0; i < 64; i++) {
            expect(arr.size() == 128 - i);
            complex_type val = arr.del(i);
            expect(arr.size() == 128 - i - 1);
        }
    };
}
