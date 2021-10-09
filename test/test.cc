//test file piece read and write

#include "Fileio.h"
#include <random>
using namespace myftp;
int main(int argc, char **argv)
{
    Fileio fio("./");
    std::random_device rd;
    std::uniform_int_distribution<int> dist(33, 125);
    const int testcount = 100000;
    std::vector<char> test(testcount);
    for (int i = 0; i < test.size(); ++i)
    {
        test[i] = static_cast<char>(dist(rd));
    }

    std::string fileName = "testfile";
    fio.enrollFile(fileName, Fileio::RWOption::kWrite, sizeof(char) * testcount);
    int left = testcount;
    int offset = 0;
    while (left)
    {
        int thisCount = 0;
        if (left > kBufSize)
        {
            thisCount = kBufSize;
        }
        else
        {
            thisCount = left;
        }
        left -= thisCount;
        std::vector<char> buf(test.begin() + offset, test.begin() + offset + thisCount);
        fio.writeFilePiece(fileName, buf);
        offset += thisCount;
    }
    std::vector<char> retBuf(testcount);
    fio.enrollFile(fileName, Fileio::RWOption::kRead, sizeof(char) * testcount);
    left = testcount;
    offset = 0;
    while (left)
    {
        int thiscount = 0;
        if (left > kBufSize)
        {
            thiscount = kBufSize;
        }
        else
        {
            thiscount = left;
        }
        left -= thiscount;
        std::vector<char> buf(thiscount);
        fio.readFilePiece(fileName, &buf);
        std::copy(buf.begin(), buf.end(), retBuf.begin() + offset);
        offset += thiscount;
    }

    //validate
    for (int i = 0; i < testcount; ++i)
    {
        if (retBuf[i] != test[i])
        {
            printf("error \n");
            return -1;
        }
    }
    printf("validate success");
}