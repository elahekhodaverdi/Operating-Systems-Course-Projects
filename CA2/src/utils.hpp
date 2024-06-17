#ifndef __UTILS_HPP
#define __UTILS_HPP

#include <iostream>
#include <string>
#include <sstream>
#include "types.hpp"
std::string remove_prefix(const std::string &input, const std::string &prefix);
std::string encode_consume_data(Month month);
std::string encode_months_data(Month months[12]);
std::string encode_bill_data(Statistics statistic);
std::string encode_months_bills(Statistics statistics[12]);
std::string encode_utilities_data(std::string water_info, std::string gas_info, std::string elec_info);

Statistics decode_consume_data(const std::string &encoded_data);
void decode_bill_data(const std::string &encoded_data, MonthStatistic &statistic);
void decode_months_data(const std::string &encoded_data, Statistics months[12]);
void decode_months_bills(const std::string &encoded_data, MonthStatistic statistics[12]);
std::vector<std::string> decode_utilities_data(const std::string &encoded_data);

#endif