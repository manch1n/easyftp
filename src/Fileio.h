#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include <vector>
#include "ErrCheck.h"

namespace fs = std::filesystem;
class Fileio
{
public:
    Fileio(const std::string &wkdir) : workDir(wkdir)
    {
        fs::create_directories(workDir);
        fs::permissions(workDir, fs::perms::others_all, fs::perm_options::add);
        fs::directory_entry entry(workDir);
        ASSERT_EXIT(entry.is_directory() == true, "not a diretory");
        fs::current_path(wkdir);
    }

    std::vector<char> readFile(const std::string &fileName);
    void writeFile(const std::vector<char> &buf);
    std::vector<char> getRemoteFiles();
    std::vector<std::string> getLocalFiles();
    std::string getCurDir();
    bool fileExist(const std::string &fileName);

private:
    const char unitSeperator = '\31';
    std::string workDir;
};
