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
{

}

template <typename T>
TestSuite<T>::~TestSuite() { }

template <typename T>
void TestSuite<T>::add_case(
    std::string_view case_label,
    std::string_view input,
    std::string_view expected
) {

}

template <typename T>
std::vector<TestResult> TestSuite<T>::run_all() const
{

}

template <typename T>
void TestSuite<T>::report(
    const std::vector<TestResult>& results
) const {

}