#ifndef __LOGGER_HPP
#define __LOGGER_HPP

#include <string>
#include "const.hpp"
class Logger
{
public:
    Logger(std::string program);

    void error(const std::string &msg);
    void warning(const std::string &msg);
    void info(const std::string &msg);

private:
    std::string program_;
};

#endif
