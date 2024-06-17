#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <wait.h>
#include <algorithm>
#include <string>
#include <iterator>
#include "fsys.hpp"
#include "const.hpp"
#include "utils.hpp"
#include "types.hpp"
#include "logger.hpp"

Logger log("Main");

void map_building(std::vector<BuildingInfo> &buildings)
{
    for (size_t i = 0; i < buildings.size(); i++)
    {
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            log.error("failed to create pipe.");
            exit(1);
        }
        pid_t forkpid = fork();
        if (forkpid == 0)
        {
            close(pipefd[0]);
            close(STDOUT_FILENO);
            dup(pipefd[1]);
            close(pipefd[1]);
            execl(EXE_BUILDING, EXE_BUILDING, (buildings[i].path.parent_path() / buildings[i].path.filename()).c_str(),
                  std::to_string(buildings[i].id).c_str(), nullptr);
            std::cerr << "Failed to execute: " << EXE_BUILDING << std::endl;
            exit(1);
        }
        else if (forkpid < 0)
        {
            log.error("Failed to fork (building map).");
            exit(1);
        }
        close(pipefd[1]);
        buildings[i].fd = pipefd[0];
    }
}

void reduce_work(int files_num)
{
    pid_t forkpid = fork();
    if (forkpid == 0)
    {
        execl(EXE_BILLS, EXE_BILLS, std::to_string(files_num).c_str(), nullptr);
        std::cerr << "Failed to execute: " << EXE_BILLS << std::endl;
        exit(1);
    }
    else if (forkpid < 0)
    {
        log.error("Failed to fork (building map).");
        exit(1);
    }
}

void close_buildings_fd(std::vector<BuildingInfo> &buildings)
{
    for (BuildingInfo &building : buildings)
    {
        close(building.fd);
    }
}

void terminate(std::vector<BuildingInfo> &buildings)
{
    for (int i = 0; i < int(buildings.size()) + 1; i++)
        wait(NULL);
    close_buildings_fd(buildings);
}

void create_namedpipes(int building_count)
{
    for (int id = 1; id <= building_count; id++)
    {
        std::stringstream pipe_name1;
        std::stringstream pipe_name2;
        pipe_name1 << PIPE_PATH << BUILDING_NPR_CODE << id;
        pipe_name2 << PIPE_PATH << BUILDING_NPW_CODE << id;
        if ((access(pipe_name1.str().c_str(), F_OK) != -1 && unlink(pipe_name1.str().c_str()) == -1) || (access(pipe_name2.str().c_str(), F_OK) != -1 && unlink(pipe_name2.str().c_str()) == -1))
        {
            log.error("Failed to delete existing named pipe.");
            exit(1);
        }
        if (mkfifo(pipe_name1.str().c_str(), 0777) == -1 || mkfifo(pipe_name2.str().c_str(), 0777) == -1)
        {
            log.error("Failed to create named pipe.");
            exit(1);
        }
    }
}
void show_resources()
{
    std::cout << std::endl
              << BLUE_COLOR << "Choose Resources You Want: " << RESET_COLOR << std::endl;
    std::cout << GREEN_COLOR << WATER << "\t" << GAS << "\t" << ELECTRICITY << RESET_COLOR << std::endl;
}

bool validate_resources_input(std::vector<std::string> &resources)
{
    for (const auto &resource : resources)
    {
        if (resource != WATER && resource != GAS && resource != ELECTRICITY)
            return false;
    }
    return true;
}

std::vector<std::string> get_wanted_services()
{
    show_resources();
    std::string input;
    std::getline(std::cin, input);

    std::istringstream iss(input);
    std::vector<std::string> resources(std::istream_iterator<std::string>{iss},
                                       std::istream_iterator<std::string>());

    if (!validate_resources_input(resources))
    {
        log.error("You Entered Wrong Resources.");
        exit(1);
    }
    return resources;
}

void show_buildings(std::vector<BuildingInfo> &buildings)
{
    std::cout << BLUE_COLOR << "List of All Buildings: " << RESET_COLOR << std::endl;
    std::cout << GREEN_COLOR << "Number\tName" << RESET_COLOR << std::endl;
    for (int i = 0; i < int(buildings.size()); i++)
        std::cout << i + 1 << "\t" << buildings[i].path.filename().c_str() << std::endl;
}

bool validate_buildings_input(std::vector<BuildingInfo> &buildings, std::vector<std::string> wanted_buildings)
{
    bool found = false;
    for (int j = 0; j < int(wanted_buildings.size()); j++)
    {
        for (int i = 0; i < int(buildings.size()); i++)
        {
            if (buildings[i].path.filename() == wanted_buildings[j])
            {
                buildings[i].wanted = true;
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return true;
}

void get_wanted_buildings(std::vector<BuildingInfo> &buildings)
{
    show_buildings(buildings);
    std::cout << BLUE_COLOR << "Please Enter List of Buildings you Want: " << RESET_COLOR;
    std::string input;
    std::getline(std::cin, input);

    std::istringstream iss(input);
    std::vector<std::string> wanted_buildings(std::istream_iterator<std::string>{iss},
                                              std::istream_iterator<std::string>());

    if (!validate_buildings_input(buildings, wanted_buildings))
    {
        log.error("You Entered Wrong Building Name");
        exit(1);
    }
}

bool validate_wanted_months(std::vector<BuildingInfo> &buildings, std::vector<int> months)
{
    for (int &month : months)
    {
        if (month <= 0 || month > 12)
            return false;
        for (BuildingInfo &building : buildings)
        {
            building.w_months[month - 1].wanted = true;
            building.g_months[month - 1].wanted = true;
            building.e_months[month - 1].wanted = true;
        }
    }
    return true;
}
void get_wanted_months(std::vector<BuildingInfo> &buildings)
{
    std::vector<int> wanted_months;
    std::string input;
    std::cout << BLUE_COLOR << "Please Enter List of Months you Want:(1-12) " << RESET_COLOR;
    std::getline(std::cin, input);
    std::istringstream iss(input);
    int num;
    while (iss >> num)
        wanted_months.push_back(num);
    if (!validate_wanted_months(buildings, wanted_months))
    {
        log.error("You Entered Wrong Number.");
        exit(1);
    }
}

bool parameter_exists(std::string parameter)
{

    return (parameter == "Bill" || parameter == "Monthly-Consumption" || parameter == "Monthly-Average" || parameter == "Peak-Hour" || parameter == "Diff-Peak-Average");
}

void set_wanted_parameter(BuildingInfo &building, std::string parameter)
{
    for (int month = 0; month < 12; month++)
    {
        if (parameter == "Bill")
        {
            building.w_months[month].month_bill.first = true;
            building.g_months[month].month_bill.first = true;
            building.e_months[month].month_bill.first = true;
        }
        if (parameter == "Monthly-Consumption")
        {
            building.w_months[month].monthly_consum.first = true;
            building.g_months[month].monthly_consum.first = true;
            building.e_months[month].monthly_consum.first = true;
        }
        if (parameter == "Monthly-Average")
        {
            building.w_months[month].monthly_average.first = true;
            building.g_months[month].monthly_average.first = true;
            building.e_months[month].monthly_average.first = true;
        }
        if (parameter == "Peak-Hour")
        {
            building.w_months[month].peak_hour.first = true;
            building.g_months[month].peak_hour.first = true;
            building.e_months[month].peak_hour.first = true;
        }
        if (parameter == "Diff-Peak-Average")
        {
            building.w_months[month].diff_peak_average.first = true;
            building.g_months[month].diff_peak_average.first = true;
            building.e_months[month].diff_peak_average.first = true;
        }
    }
}
bool validate_wanted_parameters(std::vector<BuildingInfo> &buildings, std::vector<std::string> parameters)
{
    for (std::string &parameter : parameters)
    {
        if (!parameter_exists(parameter))
            return false;
        for (BuildingInfo &building : buildings)
            set_wanted_parameter(building, parameter);
    }
    return true;
}

void show_available_parameter()
{
    std::cout << GREEN_COLOR << "\t"
              << "Bill" << RESET_COLOR << std::endl;
    std::cout << GREEN_COLOR << "\t"
              << "Monthly-Consumption" << RESET_COLOR << std::endl;
    std::cout << GREEN_COLOR << "\t"
              << "Monthly-Average" << RESET_COLOR << std::endl;
    std::cout << GREEN_COLOR << "\t"
              << "Peak-Hour" << RESET_COLOR << std::endl;
    std::cout << GREEN_COLOR << "\t"
              << "Diff-Peak-Average" << RESET_COLOR << std::endl;
}

void get_wanted_parameters(std::vector<BuildingInfo> &buildings)
{
    std::vector<std::string> wanted_parameters;
    std::string input;
    show_available_parameter();
    std::cout << BLUE_COLOR << "Please Enter List of Parameters you Want: " << RESET_COLOR;
    std::getline(std::cin, input);
    std::istringstream iss(input);
    std::string parameter;
    while (iss >> parameter)
        wanted_parameters.push_back(parameter);
    if (!validate_wanted_parameters(buildings, wanted_parameters))
    {
        log.error("Entered parameters don't exist.");
        exit(1);
    }
}

void get_input(std::vector<BuildingInfo> &buildings, std::vector<std::string> &wanted_resources)
{
    get_wanted_buildings(buildings);
    wanted_resources = get_wanted_services();
    get_wanted_months(buildings);
    get_wanted_parameters(buildings);
}

void read_building_pipe(BuildingInfo &building)
{
    std::string info;
    char buffer[1024];
    int nb;
    while ((nb = read(building.fd, buffer, sizeof(buffer))) > 0)
    {
        info += buffer;
        memset(buffer, 0, sizeof(buffer));
    }
    std::vector<std::string> services_info = decode_utilities_data(info);
    decode_months_bills(services_info[0], building.w_months);
    decode_months_bills(services_info[1], building.g_months);
    decode_months_bills(services_info[2], building.e_months);
}
void read_buildings_pipes(std::vector<BuildingInfo> &buildings)
{
    for (BuildingInfo &building : buildings)
    {
        if (building.wanted)
            read_building_pipe(building);
        log.info("Building " + building.path.filename().string() + " data collected.");
    }
}

void print_month_data(MonthStatistic &month)
{
    std::cout << "\t" << MAGENTA_COLOR << "Month: " << month.id << RESET_COLOR << std::endl;
    if (month.month_bill.first)
        std::cout << "\t"
                  << "Bill: " << month.month_bill.second << std::endl;
    if (month.monthly_consum.first)
        std::cout << "\t"
                  << "Total Consumption: " << month.monthly_consum.second << std::endl;
    if (month.monthly_average.first)
        std::cout << "\t"
                  << "Average: " << month.monthly_average.second << std::endl;
    if (month.peak_hour.first)
        std::cout << "\t"
                  << "Peak Hour: " << month.peak_hour.second << std::endl;
    if (month.diff_peak_average.first)
        std::cout << "\t"
                  << "Diff Peak Hour Consumption and Average: " << month.diff_peak_average.second << std::endl;
}
void print_resource(MonthStatistic months[12])
{
    for (int i = 0; i < 12; i++)
    {
        if (!months[i].wanted)
            continue;
        print_month_data(months[i]);
    }
}
void print_statistics(BuildingInfo &building, std::vector<std::string> &wanted_resources)
{
    if (std::find(wanted_resources.begin(), wanted_resources.end(), WATER) != wanted_resources.end())
    {
        std::cout << BLUE_COLOR << "\t"
                  << WATER << RESET_COLOR << std::endl;
        print_resource(building.w_months);
    }
    if (std::find(wanted_resources.begin(), wanted_resources.end(), GAS) != wanted_resources.end())
    {
        std::cout << RED_COLOR << "\t"
                  << GAS << RESET_COLOR << std::endl;
        print_resource(building.g_months);
    }
    if (std::find(wanted_resources.begin(), wanted_resources.end(), ELECTRICITY) != wanted_resources.end())
    {
        std::cout << YELLOW_COLOR << "\t"
                  << ELECTRICITY << RESET_COLOR << std::endl;
        print_resource(building.e_months);
    }
}
void print_result(std::vector<BuildingInfo> &buildings, std::vector<std::string> &wanted_resources)
{
    for (BuildingInfo &building : buildings)
    {
        if (!building.wanted)
            continue;
        std::cout << CYAN_COLOR << "Building Name: " << building.path.filename().c_str() << RESET_COLOR << std::endl;
        print_statistics(building, wanted_resources);
    }
}

void run(std::vector<BuildingInfo> &buildings)
{
    std::vector<std::string> wanted_resources;
    get_input(buildings, wanted_resources);
    create_namedpipes(buildings.size());
    reduce_work(buildings.size());
    map_building(buildings);
    read_buildings_pipes(buildings);
    terminate(buildings);
    print_result(buildings, wanted_resources);
}

void set_buildings_path(std::vector<fs::path> &dirs, std::vector<BuildingInfo> &buildings)
{
    for (int i = 0; i < int(dirs.size()); i++)
    {
        buildings[i].id = i + 1;
        buildings[i].path = dirs[i];
    }
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        log.error("usage: bills <buildings folder>.");
        exit(1);
    }

    std::vector<fs::path> dirs;
    if (get_all_dir(argv[1], dirs) < 0)
    {
        log.error("Failed to get directories.");
        exit(1);
    }
    log.info("Main Process started!");
    std::vector<BuildingInfo> buildings(dirs.size());
    set_buildings_path(dirs, buildings);
    run(buildings);

    return 0;
}