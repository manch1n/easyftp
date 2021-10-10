#pragma once

#include <sys/types.h>

#include <vector>

//header: [4:cmd][8:length]
namespace myftp
{
    enum class Command
    {
        kSTORE = 0,
        kRETRIEVE,
        kLOCAL_LIST,
        kREMOTE_LIST,
        kTRANSFERING,
        kTRANSCOMPLETED,
        kEXIT
    };
    //1 transfer data format:[4][8][filename][unitseperator][filedata]
    //2 transfer file block by block:[4:FILENAMECMD][8][filename]   [4:transfering|completed][8][leftsize][unitseperater][filedata]
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