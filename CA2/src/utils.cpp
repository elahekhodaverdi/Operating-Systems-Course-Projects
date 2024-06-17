#include "utils.hpp"

std::string remove_prefix(const std::string &input, const std::string &prefix)
{
    if (input.substr(0, prefix.length()) == prefix)
    {
        return input.substr(prefix.length());
    }
    else
    {
        return input;
    }
}

std::string encode_consume_data(Month month)
{
    std::string month_info = std::to_string(month.id) + "|";
    for (int hour = 0; hour < 6; hour++)
    {
        if (hour)
            month_info += "$";
        month_info += std::to_string(month.hourly_consume_sum[hour]);
    }

    return month_info;
}

std::string encode_months_data(Month months[12])
{
    std::string info;
    for (int month = 0; month < 12; month++)
    {
        info += encode_consume_data(months[month]);
        if (month != 11)
            info += "&";
    }
    return info;
}

std::string encode_utilities_data(std::string water_info, std::string gas_info, std::string elec_info)
{
    return water_info + "@" + gas_info + "@" + elec_info;
}

std::string encode_bill_data(Statistics statistic)
{
    std::string month_info = std::to_string(statistic.id) + "|";
    month_info += std::to_string(statistic.month_bill) + "$";
    month_info += std::to_string(statistic.monthly_consum) + "$";
    month_info += std::to_string(statistic.monthly_average) + "$";
    month_info += std::to_string(statistic.peak_hour) + "$";
    month_info += std::to_string(statistic.diff_peak_average);
    return month_info;
}
std::string encode_months_bills(Statistics statistics[12])
{
    std::string info;
    for (int month = 0; month < 12; month++)
    {
        info += encode_bill_data(statistics[month]);
        if (month != 11)
            info += "&";
    }
    return info;
}

Statistics decode_consume_data(const std::string &encoded_data)
{
    Statistics month;
    std::vector<std::string> tokens;
    std::stringstream ss(encoded_data);
    std::string token;

    while (std::getline(ss, token, '|'))
    {
        tokens.push_back(token);
    }

    if (tokens.size() == 2)
    {
        month.id = std::stoi(tokens[0]);

        std::stringstream sum_ss(tokens[1]);
        std::string sum_token;
        int hour = 0;

        while (std::getline(sum_ss, sum_token, '$'))
        {
            if (hour < 6)
            {
                month.hourly_consume_sum[hour] = std::stoi(sum_token);
                hour++;
            }
        }
    }

    return month;
}

void decode_months_data(const std::string &encoded_data, Statistics months[12])
{
    std::vector<std::string> month_tokens;
    std::stringstream ss(encoded_data);
    std::string month_token;

    int month = 0;
    while (std::getline(ss, month_token, '&'))
    {
        month_tokens.push_back(month_token);
        month++;
        if (month >= 12)
            break;
    }

    for (int i = 0; i < int(month_tokens.size()); i++)
    {
        months[i] = decode_consume_data(month_tokens[i]);
    }
}

std::vector<std::string> decode_utilities_data(const std::string &encoded_data)
{
    std::vector<std::string> decoded_data;
    std::istringstream iss(encoded_data);
    std::string element;

    while (std::getline(iss, element, '@'))
    {
        decoded_data.push_back(element);
    }

    return decoded_data;
}

void decode_bill_data(const std::string &encoded_data, MonthStatistic &statistic)
{
    std::vector<std::string> tokens;
    std::stringstream ss(encoded_data);
    std::string token;

    while (std::getline(ss, token, '|'))
    {
        tokens.push_back(token);
    }

    if (tokens.size() == 2)
    {
        statistic.id = std::stoi(tokens[0]);

        std::stringstream value_ss(tokens[1]);
        std::string value_token;
        int value_index = 0;

        while (std::getline(value_ss, value_token, '$'))
        {
            switch (value_index)
            {
            case 0:
                statistic.month_bill.second = std::stoi(value_token);
                break;
            case 1:
                statistic.monthly_consum.second = std::stoi(value_token);
                break;
            case 2:
                statistic.monthly_average.second = std::stoi(value_token);
                break;
            case 3:
                statistic.peak_hour.second = std::stoi(value_token);
                break;
            case 4:
                statistic.diff_peak_average.second = std::stoi(value_token);
                break;
            default:
                break;
            }

            value_index++;
        }
    }
}

void decode_months_bills(const std::string &encoded_data, MonthStatistic statistics[12])
{
    std::vector<std::string> bill_tokens;
    std::stringstream ss(encoded_data);
    std::string bill_token;

    int month = 0;
    while (std::getline(ss, bill_token, '&'))
    {
        bill_tokens.push_back(bill_token);
        month++;
        if (month >= 12)
            break;
    }

    for (int i = 0; i < int(bill_tokens.size()); i++)
        decode_bill_data(bill_tokens[i], statistics[i]);
}