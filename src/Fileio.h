#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "src/ErrCheck.h"

namespace myftp
{
    namespace fs = std::filesystem;

    const size_t kBufSize = 8192;
    class Fileio
    {
    public:
        enum class RWOption
        {
            kRead,
            kWrite
        };

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
        void enrollFile(const std::string &fileName, RWOption opt, size_t left);
        bool writeFilePiece(const std::string &fileName, const std::vector<char> &buf);
        bool readFilePiece(const std::string &fileName, std::vector<char> *buf);
        bool getFileIfDone(const std::string &fileName) const;

    private:
        class FileData
        {
        public:
            FileData(const std::string &fileName, size_t l, RWOption o) : left_(l), opt_(o)
            {
                if (opt_ == RWOption::kRead)
                {
                    strm_.open(fileName, std::ios::in | std::ios::binary);
                }
                else if (opt_ == RWOption::kWrite)
                {
                    strm_.open(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
                }
                ASSERT_EXIT(strm_.is_open(), "no open");
            }
            FileData(const FileData &) = delete;
            FileData() = default;
            FileData(FileData &&) = default;
            FileData &operator=(FileData &&) = default;
            bool ifDone() const
            {
                return left_ == 0;
            }
            RWOption getOpt() const
            {
                return opt_;
            }
            void writeFilePiece(const std::vector<char> &buf)
            {
                strm_.write(buf.data(), buf.size());
                subLeft(buf.size());
            }
            void readFilePiece(std::vector<char> *buf)
            {
                size_t thisRead = 0;
                if (left_ >= kBufSize)
                {
                    thisRead = kBufSize;
                }
                else
                {
                    thisRead = left_;
                }
                subLeft(thisRead);
                strm_.read(buf->data(), thisRead);
                buf->resize(thisRead);
            }

        private:
            size_t left_ = 0;
            RWOption opt_;
            std::fstream strm_;

            void subLeft(size_t sub)
            {
                left_ -= sub;
            }
        };

        void cancelFile(const std::string &fileName);
        std::string workDir_;
        std::map<std::string, FileData> fileHandler_;
    };
} //namespace myftp