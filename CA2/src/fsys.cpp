#include "fsys.hpp"
int get_all_dir(const std::string path, std::vector<fs::path> &dirs)
{
    const fs::path pathfs = path;
    const fs::file_status stats = fs::status(pathfs);

    if (stats.type() == fs::file_type::not_found)
        return -1;
    if (stats.type() != fs::file_type::directory)
        return -2;
    dirs.clear();

    for (const auto &entry : fs::directory_iterator(pathfs))
        if (fs::is_directory(entry.path()))
            dirs.push_back(entry.path());

    return 0;
}
