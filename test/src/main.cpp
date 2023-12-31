#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <optional>

#include "../include/utils.h"
#include "../include/setup.h"

std::unordered_map<const char *, std::unordered_map<const char *, data::test::TestFunc>> data::test::tests;

struct TestSummary {
    size_t passed;
    size_t total;
};

void run_tests(std::optional<std::string> pattern) {
    std::unordered_map<const char *, TestSummary> test_summaries;

    for (auto &test_suite_pair : data::test::tests) {
        TestSummary &summary = test_summaries[test_suite_pair.first];
        summary.total = test_suite_pair.second.size();

        for (auto &test_case_pair : test_suite_pair.second) {
            if (pattern.has_value()) {
                std::string &str = pattern.value();
                std::string suite_name(test_suite_pair.first);
                std::string case_name(test_case_pair.first);

                if (!suite_name.contains(str) && !case_name.contains(str)) {
                    summary.total--;
                    continue;
                }
            }

            printf("%s > %s ... ", test_suite_pair.first, test_case_pair.first);
            fflush(stdout);

            try {
                test_case_pair.second();
                printf("(PASS)\n");
                summary.passed++;
            } catch (int err) {
                printf("(FAIL)\n");
            }
        }
    }

    printf("\n=============== SUMMARY ===============\n");

    for (auto &summary_pair : test_summaries) {
        if (summary_pair.second.total) {
            printf("%s: %ld/%ld passed\n", summary_pair.first, summary_pair.second.passed, summary_pair.second.total);
        }
    }

#ifdef INVERT_EXPECT
    printf("NB: INVERT_EXPECT is enabled, so all of these tests should have failed. If any test did not fail, then it is likely useless.\n");
#endif
}

std::optional<std::string> get_test_pattern(int argc, char ** argv) {
    if (argc < 2) {
        return std::nullopt;
    }

    std::string str(argv[1]);

    return std::optional<std::string>(str);
}

int main(int argc, char ** argv) {
    std::optional<std::string> pattern = get_test_pattern(argc, argv);
    setup_tests();
    run_tests(pattern);

    return EXIT_SUCCESS;
}
