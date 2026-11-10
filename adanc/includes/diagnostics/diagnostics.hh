#pragma once

#include <format>
#include <print>
#include <string>
#include <utility>
#include <vector>

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

  std::string to_string() const {
    std::string severity_name;
    switch (severity) {
    case Error:
      severity_name = "Error";
      break;
    case Warning:
      severity_name = "Warning";
      break;
    }

    if (line > 0 && column > 0)
      return std::format("{}: {} ({}:{})", severity_name, message, line,
                         column);

    return std::format("{}: {}", severity_name, message);
  }
};

class Diagnostics {
  std::vector<Log> collected_logs;

public:
  inline void collect(Log log) { collected_logs.emplace_back(log); }

  [[noreturn]] inline void error(std::string message, int line = 0,
                                 int column = 0) {
    collect(Log{Severity::Error, std::move(message), line, column});
    throw DiagnosticAbort{};
  }

  inline void warning(std::string message, int line = 0, int column = 0) {
    collect(Log{Severity::Warning, std::move(message), line, column});
  }

  inline bool has_errors() const {
    for (const auto &log : collected_logs) {
      if (log.severity == Severity::Error)
        return true;
    }

    return false;
  }

  inline void output() {
    for (auto &log : collected_logs) {
      std::println("{}", log.to_string());
    }
  }
};