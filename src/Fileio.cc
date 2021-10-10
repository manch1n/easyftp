#include "Fileio.h"

#include <cstring>
#include <algorithm>

#include "CommandReply.h"

using namespace myftp;

std::vector<char> Fileio::readFile(const std::string &fileName) const //file must exist
{
    fs::directory_entry entry(fileName);
    // ASSERT_EXIT(entry.is_regular_file() == true, "no such file");
    // ASSERT_EXIT((fs::status(fileName).permissions() & fs::perms::others_read) != fs::perms::none, "could not read");
    size_t fileSize = entry.file_size();
    std::ifstream ifs(fileName, std::ios::binary);
    std::vector<char> ret(fileSize + fileName.size() + 1);
    std::memcpy(ret.data(), fileName.data(), fileName.size());
    ret[fileName.size()] = kunitSeperator;
    ifs.read(ret.data() + fileName.size() + 1, fileSize);
    ifs.close();
    return ret;
}

void Fileio::writeFile(const std::vector<char> &buf)
{
    auto nameEnd = std::find(buf.begin(), buf.end(), kunitSeperator);
    ASSERT_EXIT(nameEnd != buf.end(), "write file error");
    std::string fileName(buf.begin(), nameEnd);
    std::ofstream ofstrm(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
    size_t fileNameHeaderLen = fileName.size() + 1;
    ASSERT_EXIT(buf.size() >= fileNameHeaderLen, "buf size error");
    ofstrm.write(buf.data() + fileNameHeaderLen, buf.size() - fileNameHeaderLen);
    ofstrm.close();
    fs::permissions(fileName, fs::perms::others_read | fs::perms::others_write, fs::perm_options::add);
}

std::vector<char> Fileio::getRemoteFiles() const
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
        ret.push_back(kunitSeperator);
    }
    return ret;
}

std::vector<std::string> Fileio::getLocalFiles() const
{
    std::vector<std::string> ret;
    fs::directory_iterator list("./");
    for (const auto &iter : list)
    {
        ret.push_back(iter.path().filename());
    }
    return ret;
}

std::string Fileio::getCurDir() const
{
    return workDir_;
}

bool Fileio::fileExist(const std::string &fileName) const
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

void myftp::Fileio::enrollFile(const std::string &fileName, RWOption opt, size_t left)
{
    ASSERT_EXIT(fileHandler_.find(fileName) == fileHandler_.end(), "already exist");
    fileHandler_[fileName] = std::move(FileData(fileName, left, opt));
}

void myftp::Fileio::cancelFile(const std::string &fileName)
{
    ASSERT_EXIT(fileHandler_.find(fileName) != fileHandler_.end(), "not found");
    fileHandler_.erase(fileName);
}

bool myftp::Fileio::writeFilePiece(const std::string &fileName, const std::vector<char> &buf)
{
    FileData &fileData = fileHandler_[fileName];
    ASSERT_EXIT(fileData.getOpt() == RWOption::kWrite, "not write");
    fileData.writeFilePiece(buf);
    bool done = fileData.ifDone();
    if (done)
    {
        cancelFile(fileName);
    }
    return done;
}

bool myftp::Fileio::readFilePiece(const std::string &fileName, std::vector<char> *buf)
{
    buf->resize(kBufSize);
    FileData &fileData = fileHandler_[fileName];
    ASSERT_EXIT(fileData.getOpt() == RWOption::kRead, "not read");
    fileData.readFilePiece(buf);
    bool done = fileData.ifDone();
    if (done)
    {
        cancelFile(fileName);
    }
    return done;
}

bool myftp::Fileio::getFileIfDone(const std::string &fileName) const
{
    auto iter = fileHandler_.find(fileName);
    if (iter == fileHandler_.end())
    {
        return true;
    }
    return iter->second.ifDone();
}

size_t myftp::Fileio::getFileSize(const std::string &fileName) const
{
    fs::directory_entry entry(fileName);
    return entry.file_size();
}
