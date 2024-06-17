#ifndef _CONST_HPP
#define _CONST_HPP

#include <iostream>
#include <string>
const char PIPE_PATH[] = "namedpipes/";
const char EXE_BUILDING[] = "build/bin/building.out";
const char EXE_BILLS[] = "build/bin/bills.out";
const char EXE_SHAKHES[] = "build/bin/service.out";
const int HOURS_PER_DAY = 6;
const int DAYS_PER_MONTH = 30;
const int MONTH_PER_YEAR = 12;

const char BUILDING_NPW_CODE[] = "b_w";
const char BUILDING_NPR_CODE[] = "b_r";

const std::string RED_COLOR = "\033[1;31m";
const std::string GREEN_COLOR = "\033[1;32m";
const std::string YELLOW_COLOR = "\033[1;33m";
const std::string BLUE_COLOR = "\033[1;34m";
const std::string MAGENTA_COLOR = "\033[1;35m";
const std::string CYAN_COLOR = "\033[1;36m";
const std::string RESET_COLOR = "\033[0m";

const std::string WATER = "Water";
const std::string GAS = "Gas";
const std::string ELECTRICITY = "Electricity";
#endif