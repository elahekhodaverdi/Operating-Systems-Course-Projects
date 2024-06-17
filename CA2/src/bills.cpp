#include <iostream>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <wait.h>
#include <numeric>
#include <string>
#include <vector>
#include <algorithm>
#include "types.hpp"
#include "fsys.hpp"
#include "utils.hpp"
#include "const.hpp"
#include "logger.hpp"

Logger log("bills");
class Utility
{
public:
    Utility()
    {
        for (int i = 1; i <= 12; i++)
            months_statistics[i - 1].id = i;
    }
    virtual void calc_bill() = 0;
    void decode(std::string encoded);
    Statistics months_statistics[12];
};

class Water : public Utility
{
public:
    Water() : Utility() {}
    void calc_bill();
};

class Gas : public Utility
{
public:
    Gas() : Utility() {}
    void calc_bill();
};

class Electricity : public Utility
{
public:
    Electricity() : Utility() {}
    void calc_bill();
};

void Utility::decode(std::string encoded)
{
    decode_months_data(encoded, months_statistics);
}

void Water::calc_bill()
{
    for (auto &month : months_statistics)
    {
        int sum = 0;
        month.diff_peak_average = month.hourly_consume_sum[month.peak_hour] - month.monthly_average;
        for (int hour = 0; hour < 6; hour++)
        {
            sum += (hour == month.peak_hour) ? 1.25 * month.hourly_consume_sum[hour] : month.hourly_consume_sum[hour];
        }
        month.month_bill = sum;
    }
}

void Gas::calc_bill()
{
    for (auto &month : months_statistics)
    {
        month.diff_peak_average = month.hourly_consume_sum[month.peak_hour] - month.monthly_average;
        month.month_bill = std::accumulate(month.hourly_consume_sum.begin(), month.hourly_consume_sum.end(), 0);
    }
}

void Electricity::calc_bill()
{
    for (Statistics &month : months_statistics)
    {
        month.diff_peak_average = month.hourly_consume_sum[month.peak_hour] - month.monthly_average;
        int sum = 0;
        for (int hour = 0; hour < 6; hour++)
        {
            if (hour == month.peak_hour)
                sum += 1.25 * month.hourly_consume_sum[hour];
            else if (month.hourly_consume_sum[hour] < month.monthly_average)
                sum += 0.75 * month.hourly_consume_sum[hour];
            else
                sum += month.hourly_consume_sum[hour];
        }
        month.month_bill = sum;
    }
}

class Building
{
public:
    Building() {}
    void calc_bills();
    void read_from_namedpipe();
    void write_to_namedpipe();
    std::string encode_bills();
    int id;
    Water water;
    Gas gas;
    Electricity elec;
};

void Building::calc_bills()
{
    water.calc_bill();
    gas.calc_bill();
    elec.calc_bill();
}

std::string Building::encode_bills()
{
    std::string water_info = encode_months_bills(water.months_statistics);
    std::string gas_info = encode_months_bills(gas.months_statistics);
    std::string elec_info = encode_months_bills(elec.months_statistics);
    return encode_utilities_data(water_info, gas_info, elec_info);
}
void Building::write_to_namedpipe()
{
    std::string info = encode_bills();
    std::string pipename = BUILDING_NPR_CODE + std::to_string(id);
    std::string pipepath = PIPE_PATH + pipename;
    int pipefd = open(pipepath.c_str(), O_WRONLY);
    if (pipefd == -1)
    {
        log.error("Failed to open the named pipe.");
        exit(1);
    }
    write(pipefd, info.c_str(), strlen(info.c_str()) + 1);
    close(pipefd);
}

void Building::read_from_namedpipe()
{
    std::string pipename = BUILDING_NPW_CODE + std::to_string(id);
    std::string pipepath = PIPE_PATH + pipename;
    int pipefd = open(pipepath.c_str(), O_RDONLY);
    if (pipefd == -1)
    {
        log.error("Failed to open the named pipe.");
        exit(1);
    }
    std::string info;
    char buffer[1024];
    int nb;
    while ((nb = read(pipefd, buffer, sizeof(buffer))) > 0)
    {
        info += buffer;
        memset(buffer, 0, sizeof(buffer));
    }
    std::vector<std::string> utilitys_info = decode_utilities_data(info);
    water.decode(utilitys_info[0]);
    gas.decode(utilitys_info[1]);
    elec.decode(utilitys_info[2]);
    close(pipefd);
    unlink(pipepath.c_str());
}

void find_peak(std::vector<Building> &buildings)
{
    auto calculate_peak_hour = [](auto &utilities, auto get_month_data)
    {
        for (size_t month = 0; month < 12; month++)
        {
            std::vector<int> sumVector(6, 0);
            for (const auto &building : buildings)
            {
                for (size_t hour = 0; hour < 6; hour++)
                {
                    sumVector[hour] += get_month_data(building).months_statistics[month].hourly_consume_sum[hour];
                }
            }
            int peak = std::distance(sumVector.begin(), std::max_element(sumVector.begin(), sumVector.end()));
            for (auto &building : buildings)
            {
                get_month_data(building).months_statistics[month].peak_hour = peak;
            }
        }
    };

    calculate_peak_hour(buildings, [](const Building &b)
                        { return b.water; });
    calculate_peak_hour(buildings, [](const Building &b)
                        { return b.gas; });
    calculate_peak_hour(buildings, [](const Building &b)
                        { return b.elec; });
}

void calc_average(std::vector<Building> &buildings)
{
    auto calculate_average = [](auto &utilities, auto get_month_data)
    {
        if (buildings.empty())
            return;

        for (size_t month = 0; month < 12; month++)
        {
            for (auto &building : buildings)
            {
                int total = std::accumulate(get_month_data(building).months_statistics[month].hourly_consume_sum.begin(),
                                            get_month_data(building).months_statistics[month].hourly_consume_sum.end(), 0);
                get_month_data(building).months_statistics[month].monthly_consum = total;
                get_month_data(building).months_statistics[month].monthly_average = total / 6;
            }
        }
    };

    calculate_average(buildings, [](Building &b)
                      { return b.water; });
    calculate_average(buildings, [](Building &b)
                      { return b.gas; });
    calculate_average(buildings, [](Building &b)
                      { return b.elec; });
}

void calculate_statistics(std::vector<Building> &buidings)
{
    find_peak(buidings);
    calc_averge(buidings);
}

void calc_bills(std::vector<Building> &buildings)
{
    for (Building &building : buildings)
        building.calc_bills();
}
int main(int argc, const char *argv[])
{

    int filecount = std::stoi(argv[1]);
    std::vector<Building> buildings(filecount);
    for (int i = 0; i < filecount; i++)
        buildings[i].id = i + 1;

    for (Building &building : buildings)
        building.read_from_namedpipe();
    log.info("Data has been received from buildings.");

    calculate_statistics(buildings);
    calc_bills(buildings);
    log.info("Bills was calculated.");

    for (Building &building : buildings)
        building.write_to_namedpipe();
    log.info("Bills was sent to buildings.");
}