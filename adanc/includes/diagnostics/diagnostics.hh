#pragma once

#include <format>
#include <print>
#include <string>
#include <vector>

enum Severity
{
    Error,
    Warning,
};

struct Log
{
    Severity severity;
    std::string message;

    int line;
    int column;

    const std::string to_string()
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

        return std::format("{}: {} ({}:{})", severity_name, message, line, column);
    }
};

class Diagnostics
{
    std::vector<Log> collected_logs;

public:
    inline void collect(Log log)
    {
        collected_logs.emplace_back(log);
    }

    inline bool has_errors() const
    {
        for (const auto &log : collected_logs)
        {
            if (log.severity == Severity::Error)
                return true;
        }

        return false;
    }

    inline void output()
    {
        for (auto &log : collected_logs)
        {
            std::println("{}", log.to_string());
        }
    }
};