#include "Fileio.h"

#include <fstream>
#include <cstring>
#include <algorithm>

using namespace myftp;

std::vector<char> Fileio::readFile(const std::string &fileName) //file must exist
{
    fs::directory_entry entry(fileName);
    // ASSERT_EXIT(entry.is_regular_file() == true, "no such file");
    // ASSERT_EXIT((fs::status(fileName).permissions() & fs::perms::others_read) != fs::perms::none, "could not read");
    size_t fileSize = entry.file_size();
    std::ifstream ifs(fileName, std::ios::binary);
    std::vector<char> ret(fileSize + fileName.size() + 1);
    std::memcpy(ret.data(), fileName.data(), fileName.size());
    ret[fileName.size()] = unitSeperator;
    ifs.read(ret.data() + fileName.size() + 1, fileSize);
    ifs.close();
    return ret;
}

void Fileio::writeFile(const std::vector<char> &buf)
{
    auto nameEnd = std::find(buf.begin(), buf.end(), unitSeperator);
    ASSERT_EXIT(nameEnd != buf.end(), "write file error");
    std::string fileName(buf.begin(), nameEnd);
    std::ofstream ofstrm(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
    size_t fileNameHeaderLen = fileName.size() + 1;
    ASSERT_EXIT(buf.size() >= fileNameHeaderLen, "buf size error");
    ofstrm.write(buf.data() + fileNameHeaderLen, buf.size() - fileNameHeaderLen);
    ofstrm.close();
    fs::permissions(fileName, fs::perms::others_read | fs::perms::others_write, fs::perm_options::add);
}

std::vector<char> Fileio::getRemoteFiles()
{
    std::vector<char> ret;
    fs::directory_iterator list("./");
    for (const auto &iter : list)
    {
        std::string fileStr(iter.path().filename());
        for (char c : fileStr)
        {
            ret.push_back(c);
        }
        ret.push_back('\31');
    }
    return ret;
}

std::vector<std::string> Fileio::getLocalFiles()
{
    std::vector<std::string> ret;
    fs::directory_iterator list("./");
    for (const auto &iter : list)
    {
        ret.push_back(iter.path().filename());
    }
    return ret;
}

std::string Fileio::getCurDir()
{
    return workDir;
}

bool Fileio::fileExist(const std::string &fileName)
{
    if (fs::exists(fileName) == false)
    {
        return false;
    }
    fs::directory_entry entry(fileName);
    if (entry.is_regular_file() == false)
    {
        return false;
    }
    auto status = entry.status();
    if ((status.permissions() & (fs::perms::others_write | fs::perms::others_read)) != fs::perms::none)
    {
        return true;
    }
    else
        return false;
}
