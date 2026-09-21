#include "core/logger.h"

#include <iostream>

namespace Core {

const char* Logger::LevelToString(const LogLevel level) {
    switch (level) {
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

void Logger::Log(const LogLevel level, const std::string& message) {
    const char* prefix = LevelToString(level);
    if (level == LogLevel::Error) {
        std::cerr << "[" << prefix << "] " << message << "\n";
    } else {
        std::cout << "[" << prefix << "] " << message << "\n";
    }
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::Info, message);
}

void Logger::Warning(const std::string& message) {
    Log(LogLevel::Warning, message);
}

void Logger::Error(const std::string& message) {
    Log(LogLevel::Error, message);
}

} // namespace Core
