#include <iostream>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <wait.h>
#include <vector>
#include <string>
#include "logger.hpp"
#include "fsys.hpp"
#include "utils.hpp"
#include "const.hpp"

enum Service_FD
{
    WATER_FD,
    GAS_FD,
    ELECTRICITY_FD
};
int reduce(fs::path &dir, std::string type)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        std::cerr << "failed to create pipe." << std::endl;
        return -1;
    }
    pid_t forkpid = fork();
    if (forkpid == 0)
    {
        close(pipefd[0]);
        close(STDOUT_FILENO);
        dup(pipefd[1]);
        close(pipefd[1]);
        std::string csvType = type + ".csv";
        execl(EXE_SHAKHES, EXE_SHAKHES, (dir / csvType).c_str(), type.c_str(), nullptr);
        std::cerr << "Failed to execute: " << EXE_SHAKHES << std::endl;
        return -1;
    }
    else if (forkpid < 0)
    {
        std::cerr << "Failed to fork (building map).\n";
        return -1;
    }
    close(pipefd[1]);
    return pipefd[0];
}

void reduce_service(fs::path dirpath, std::vector<int> &services_fd)
{
    int fd;
    if ((fd = reduce(dirpath, WATER)) == -1)
        exit(1);
    services_fd[WATER_FD] = fd;
    if ((fd = reduce(dirpath, GAS)) == -1)
        exit(1);
    services_fd[GAS_FD] = fd;
    if ((fd = reduce(dirpath, ELECTRICITY)) == -1)
        exit(1);
    services_fd[ELECTRICITY_FD] = fd;
}

void terminate(Logger &log)
{
    wait(NULL);
    wait(NULL);
    wait(NULL);
    log.info("Terminate.");
}

void write_to_namedpipe(std::string id, std::string &info, Logger &log)
{
    std::string pipename = BUILDING_NPW_CODE + id;
    std::string pipepath = PIPE_PATH + pipename;
    int pipefd = open(pipepath.c_str(), O_WRONLY);

    if (pipefd == -1)
    {
        std::cerr << "Failed to open the named pipe\t53.\n";
        exit(1);
    }
    write(pipefd, info.c_str(), strlen(info.c_str()) + 1);
    close(pipefd);
    log.info("Consumption data sent to Bills Process.");
}

void read_from_namedpipe(std::string id, std::string &info, Logger &log)
{
    std::string pipename = BUILDING_NPR_CODE + id;
    std::string pipepath = PIPE_PATH + pipename;
    int pipefd = open(pipepath.c_str(), O_RDONLY);
    if (pipefd == -1)
    {
        std::cerr << "Failed to open the named pipe\t49.\n";
        exit(1);
    }
    info.clear();
    char buffer[1024];
    int nb;
    while ((nb = read(pipefd, buffer, sizeof(buffer))) > 0)
    {
        info += buffer;
        memset(buffer, 0, sizeof(buffer));
    }
    close(pipefd);
    unlink(pipepath.c_str());
    log.info("Receive bills has been done.");
}

int read_pipe(std::vector<int> &services_fd, std::vector<std::string> &services_info, Logger &log)
{
    std::string info;
    for (int i = 0; i < int(services_fd.size()); i++)
    {
        char buffer[1024];
        int nb;
        while ((nb = read(services_fd[i], buffer, sizeof(buffer))) > 0)
        {
            info += buffer;

            memset(buffer, 0, sizeof(buffer));
        }
        services_info[i] = info;

        info.clear();
    }
    log.info("Consumption data was gathered.");
    return 1;
}

void run(fs::path dirpath, std::string id, Logger &log)
{
    std::vector<int> services_fd(3);
    std::vector<std::string> services_info(3);
    reduce_service(dirpath, services_fd);
    read_pipe(services_fd, services_info, log);
    std::string info = encode_utilities_data(services_info[WATER_FD], services_info[GAS_FD], services_info[ELECTRICITY_FD]);
    write_to_namedpipe(id, info, log);
    read_from_namedpipe(id, info, log);
    write(STDOUT_FILENO, info.c_str(), info.size() + 1);
    close(STDOUT_FILENO);
}

int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "bulding: not enough arg\n";
        exit(1);
    }

    std::string dir = argv[1];
    std::string id = argv[2];
    fs::path dirpath(dir);
    Logger log(dirpath.filename().c_str());
    log.info("Building process has been created.");
    run(dirpath, id, log);
    terminate(log);
}