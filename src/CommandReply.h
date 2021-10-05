#pragma once

#include <sys/types.h>

#include <vector>

namespace myftp
{
    enum class Command
    {
        kSTORE = 0,
        kRETRIEVE,
        kLOCAL_LIST,
        kREMOTE_LIST,
        kEXIT
    };

    enum class Reply
    {
        kSUCCESS = 100,
        kFAIL
    };

    struct Message
    {
        int32_t cmdOrReply;
        u_int64_t length;
        std::vector<char> data;
    };

    const u_int64_t kHEADER_SIZE = sizeof(int32_t) + sizeof(u_int64_t);
    const char kunitSeperator = '\31';
} //namespace myftp