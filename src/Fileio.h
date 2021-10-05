#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include <vector>

#include "src/ErrCheck.h"

namespace myftp
{
    namespace fs = std::filesystem;
    class Fileio
    {
    public:
        Fileio(const std::string &wkdir) : workDir_(wkdir)
        {
            fs::create_directories(workDir_);
            fs::permissions(workDir_, fs::perms::others_all, fs::perm_options::add);
            fs::directory_entry entry(workDir_);
            ASSERT_EXIT(entry.is_directory() == true, "not a diretory");
            fs::current_path(wkdir);
        }

        Fileio(const Fileio &) = delete;
        Fileio &operator=(const Fileio &) = delete;

        std::vector<char> readFile(const std::string &fileName) const;
        void writeFile(const std::vector<char> &buf);
        std::vector<char> getRemoteFiles() const;
        std::vector<std::string> getLocalFiles() const;
        std::string getCurDir() const;
        bool fileExist(const std::string &fileName) const;

    private:
        std::string workDir_;
    };
} //namespace myftp