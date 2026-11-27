#pragma once

#include <format>
#include <print>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

enum Severity {
    Error,
    Warning,
};

struct DiagnosticAbort {};

struct Log {
    Severity severity;
    std::string message;

    int line;
    int column;

    [[nodiscard]] std::string to_string() const
    {
        std::string severity_name;
        switch (severity)
        {
        case Error:
            severity_name = "Error";
            break;
        case Warning:
            severity_name = "Warning";
            break;
        }

        if (line > 0 && column > 0)
            return std::format("{}: {} ({}:{})", severity_name, message, line, column);

        return std::format("{}: {}", severity_name, message);
    }
};

class Diagnostics {
    std::vector<Log> collected_logs;

  public:
    void collect(const Log& log)
    {
        collected_logs.emplace_back(log);
    }

    [[noreturn]] void error(std::string message, const int line = 0, const int column = 0)
    {
        collect(Log{Severity::Error, std::move(message), line, column});
        throw DiagnosticAbort{};
    }

    void warning(std::string message, const int line = 0, const int column = 0)
    {
        collect(Log{Severity::Warning, std::move(message), line, column});
    }

    [[nodiscard]] bool has_errors() const
    {
        return std::ranges::any_of(collected_logs, [](const auto &log) {
            return log.severity == Severity::Error;
        });
    }

    void output() const
    {
        for (auto &log : collected_logs)
        {
            std::println("{}", log.to_string());
        }
    }
};