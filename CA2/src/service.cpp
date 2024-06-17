#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>

#include <numeric>
#include "const.hpp"
#include "types.hpp"
#include "logger.hpp"
#include "utils.hpp"

class Service
{
public:
    void calc_hourly_sum();
    std::string encoded_data();
    Service()
    {
        for (int i = 1; i <= 12; i++)
            months[i - 1].id = i;
    }
    void read_csv(std::string filepath, Logger &log);

private:
    Month months[12];
};

std::string Service::encoded_data()
{

    return encode_months_data(months);
}

void Service::calc_hourly_sum()
{
    for (auto &month : months)
    {
        for (const ConsumptionData &data : month.consumptions)

            for (int hour = 0; hour < int(6); hour++)

                month.hourly_consume_sum[hour] += data.hourly_consumption[hour];
    }
}

void Service::read_csv(std::string filepath, Logger &log)
{
    std::ifstream file(filepath);
    std::string line;

    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string token;

        ConsumptionData entry;

        std::getline(iss, token, ',');
        entry.year = std::stoi(token);
        std::getline(iss, token, ',');
        entry.month = std::stoi(token);
        std::getline(iss, token, ',');
        entry.day = std::stoi(token);
        int hour = 0;
        while (std::getline(iss, token, ','))
        {
            entry.hourly_consumption[hour] = std::stod(token);
            hour += 1;
        }
        months[entry.month - 1].consumptions.push_back(entry);
    }
    log.info("CSV has been read.");
}

int main(int argc, const char *argv[])
{

    std::string csv_path = argv[1];
    std::string type = argv[2];
    fs::path path(csv_path);
    Service service = Service();
    std::string program = path.parent_path().filename().string() + "-" + type;
    Logger log(program);
    service.read_csv(csv_path, log);
    service.calc_hourly_sum();
    std::string data = service.encoded_data();
    write(STDOUT_FILENO, data.c_str(), data.size() + 1);
    close(STDOUT_FILENO);
    log.info("The consumption data has been encoded and sent.");
    return 0;
}