#pragma once

#include <sys/types.h>

#include <vector>

namespace myftp
{
    enum class Command
    {
        STORE = 0,
        RETRIEVE,
        LOCAL_LIST,
        REMOTE_LIST,
        EXIT
    };

    enum class Reply
    {
        SUCCESS = 100,
        FAIL
    };

    struct Message
    {
        int32_t cmdOrReply;
        u_int64_t length;
        std::vector<char> data;
    };

    const u_int64_t HEADER_SIZE = sizeof(int32_t) + sizeof(u_int64_t);
}