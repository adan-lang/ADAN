#include <iostream>
#include <string>
#include <functional>
#include <string_view>
#include <vector>

#include "suite.hh"

template <typename T>
TestSuite<T>::TestSuite(
    std::string test_name,
    std::function<T(std::string_view)> run_fn,
    std::function<std::string(const T&)> serialize_fn
)
    : test_name_(std::move(test_name)),
      run_fn_(std::move(run_fn)),
      serialize_fn_(std::move(serialize_fn))
{ }

template <typename T>
TestSuite<T>::~TestSuite() { }

template <typename T>
void TestSuite<T>::add_case(
    std::string_view case_label,
    std::string_view input,
    std::string_view expected
) {
    cases_.emplace_back(
        case_label,
        input,
        expected
    );
}

template <typename T>
std::vector<TestResult> TestSuite<T>::run_all() const
{
    std::vector<TestResult> results;

    for (const Case& c : cases_)
    {
        T raw = run_fn_(c.input);
        std::string actual_string = serialize_fn_(raw);

        results.emplace_back(
            c.input,
            c.label,
            c.expected,
            actual_string,
            actual_string == c.expected
        );
    }

    return results;
}

template <typename T>
void TestSuite<T>::report(
    const std::vector<TestResult>& results
) const {
    int passed_count{};
    std::vector<TestResult> failed_results;
    
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
            
                "  \x1b[1;31m[FAIL]\x1b[0m: " << result.expected << "\n" <<
                "    Input: " << result.input << "\n" <<
                "    Expected: " << result.expected << "\n" <<
                "    Result: " << result.actual << "\n"
                
            << "\n";

            failed_results.emplace_back(
                result.input,
                result.label,
                result.expected,
                result.actual,
                result.passed
            );
        }
    }

    for (const TestResult& result : failed_results)
    {
        std::cout << "\n";
        std::cout <<
        
            "  \x1b[1;31m[FAIL]\x1b[0m: " << result.expected << "\n" <<
            "    Input: " << result.input << "\n" <<
            "    Expected: " << result.expected << "\n" <<
            "    Result: " << result.actual << "\n"
            
        << "\n";
    }

    std::cout << passed_count << " of " << results.size() << " passed!" << "\n";
}