#ifndef __TYPES_HPP
#define __TYPES_HPP

#include <vector>
#include <iostream>
#include "fsys.hpp"
struct ConsumptionData
{
    int year = 0;
    int month = 0;
    int day = 0;
    std::vector<int> hourly_consumption = std::vector<int>(6, 0);
};

struct Statistics
{
    int id = 0;
    int peak_hour = 0;
    int monthly_average = 0;
    int monthly_consum = 0;
    int diff_peak_average = 0;
    int month_bill = 0;
    std::vector<int> hourly_consume_sum = std::vector<int>(6, 0);
};

struct Month
{
    int id;
    std::vector<ConsumptionData> consumptions;
    std::vector<int> hourly_consume_sum = std::vector<int>(6, 0);
};

struct MonthStatistic
{
    int id = 0;
    std::pair<bool, int> month_bill = {false, 0};
    std::pair<bool, int> monthly_consum = {false, 0};
    std::pair<bool, int> monthly_average = {false, 0};
    std::pair<bool, int> peak_hour = {false, 0};
    std::pair<bool, int> diff_peak_average = {false, 0};
    bool wanted = false;
};

struct BuildingInfo
{
    int id = 0;
    int fd = 0;
    bool wanted = false;
    fs::path path;
    MonthStatistic w_months[12];
    MonthStatistic g_months[12];
    MonthStatistic e_months[12];
};

#endif