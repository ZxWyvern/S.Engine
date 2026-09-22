#pragma once

#include <string>

namespace Foundation {

enum class LogLevel {
    Info,
    Warning,
    Error
};

class Logger {
public:
    static void Info(const std::string& message);
    static void Warning(const std::string& message);
    static void Error(const std::string& message);
    static void Log(LogLevel level, const std::string& message);

private:
    static const char* LevelToString(LogLevel level);
};

} // namespace Foundation
