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
    );

    ~TestSuite();

    void add_case(
        std::string_view case_label,
        std::string_view input,
        std::string_view expected
    );

    std::vector<TestResult> run_all() const;

    void report(
        const std::vector<TestResult>& results
    ) const;
};