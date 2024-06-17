#include "logger.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>

Logger::Logger(std::string program) : program_(std::move(program)) {}

void Logger::error(const std::string &msg)
{
    std::string buf = RED_COLOR + "[ERR:" + program_ + "] " + RESET_COLOR + msg + '\n';
    std::cerr << buf;
}

void Logger::warning(const std::string &msg)
{
    std::string buf = YELLOW_COLOR + msg + RESET_COLOR + msg + '\n';
    std::cerr << buf;
}

void Logger::info(const std::string &msg)
{
    std::string buf = BLUE_COLOR + "[INF:" + program_ + "] " + RESET_COLOR + msg + '\n';
    std::cerr << buf;
}
