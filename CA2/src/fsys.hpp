#ifndef __FSYS_HPP
#define __FSYS_HPP

#include <iostream>
#include <string.h>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

int get_all_dir(const std::string path, std::vector<fs::path> &dirs);

#endif