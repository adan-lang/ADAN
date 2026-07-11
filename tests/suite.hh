#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <string_view>
#include <vector>

struct Case
{
    std::string label;
    std::string input;
    std::string expected;
};

struct TestResult
{
    std::string label;
    std::string input;
    std::string expected;
    std::string actual;
    bool passed;
};

template <typename T>
class TestSuite
{
private:
    std::string test_name_;
    std::function<T(std::string_view)> run_fn_;
    std::function<std::string(const T&)> serialize_fn_;
    std::vector<Case> cases_;

public:
    TestSuite(
        std::string test_name,
        std::function<T(std::string_view)> run_fn,
        std::function<std::string(const T&)> serialize_fn
    )
        : test_name_(std::move(test_name)),
          run_fn_(std::move(run_fn)),
          serialize_fn_(std::move(serialize_fn))
    { }

    ~TestSuite() { }

    void add_case(
        std::string_view case_label,
        std::string_view input,
        std::string_view expected
    ) {
        cases_.push_back(Case{
            std::string(case_label),
            std::string(input),
            std::string(expected)
        });
    }

    std::vector<TestResult> run_all() const
    {
        std::vector<TestResult> results;

        for (const Case& c : cases_)
        {
            T raw = run_fn_(c.input);
            std::string actual_string = serialize_fn_(raw);

            results.emplace_back(
                c.label,
                c.input,
                c.expected,
                actual_string,
                actual_string == c.expected
            );
        }

        return results;
    }

    void report(
        const std::vector<TestResult>& results
    ) const {
        int passed_count{};
        std::vector<TestResult> failed_results;
        std::string include_s = "";

        if (results.size() == 0 || results.size() > 1)
        {
            include_s = "s";
        }

        std::cout << "Running \x1b[1m" << results.size() << "\x1b[0m test" << include_s << " and testing each one for issues..." << "\n\n";

        for (const TestResult& result : results)
        {
            if (result.passed)
            {
                passed_count += 1;

                std::cout << "  \x1b[1;32m[PASS]\x1b[0m: " << result.label << "\n";
            }

            else
            {
                std::cout <<

                    "  \x1b[1;31m[FAIL]\x1b[0m: " << result.label << "\n"
                    "    \x1b[1mI →\x1b[0m " << result.input << "\n" 
                    "    \x1b[1mE ✓\x1b[0m " << result.expected << "\n" 
                    "    \x1b[1mA ✗\x1b[0m " << result.actual << "\n";

                failed_results.emplace_back(
                    result.label,
                    result.input,
                    result.expected,
                    result.actual,
                    result.passed
                );
            }
        }

        if (failed_results.size() > 0)
        {
            std::cout << "\n\x1b[2;1m--------------\x1b[0;1;31m FAILED TESTS \x1b[0m(" << failed_results.size() << ")\x1b[0m \x1b[1;2m--------------\x1b[0m" << "\n";
        }

        for (const TestResult& result : failed_results)
        {
            std::cout << "\n";
            std::cout <<

                "  \x1b[1;31m[FAIL]\x1b[0m: " << result.label << "\n"
                "    \x1b[1m→\x1b[0m " << result.input << "\n" 
                "    \x1b[1m✓\x1b[0m " << result.expected << "\n" 
                "    \x1b[1m✗\x1b[0m " << result.actual << "\n";
        }

        double success_rate = ((results.size() - failed_results.size()) * 100.0) / results.size();

        std::cout << "\n\x1b[1m" << passed_count << "\x1b[0m of \x1b[1m" << results.size() << "\x1b[0m tests were successful, scoring \x1b[1m" << success_rate << "%\x1b[0m." << "\n";
    }
};
