#include "ServerHandler.h"

using namespace myftp;

int main(int argc, char **argv)
{
    ServerHandler serv(55555);
    serv.start();
}