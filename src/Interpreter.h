#pragma once

#include <sys/types.h>

#include <vector>
#include <string>

#include "src/CommandReply.h"

namespace myftp
{
    class Interpreter
    {
    public:
        static Message parseRawMsg(const std::vector<char> &rawMsg);
        static std::vector<char> formatPacket(int32_t cmd, const std::vector<char> &data);
        static std::vector<char> formatPacket(int32_t cmd);
        static void parseFileInfo(const std::vector<char> &data, size_t *length, std::string *fileName);
        static std::vector<char> formatFileInfo(const std::string &fileName, size_t len);
        static std::vector<std::vector<char>> splitByUnitSeperator(const std::vector<char> &s, bool splitOnce);
    };
} // namespace myftp
