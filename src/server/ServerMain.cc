#include "ServerHandler.h"

int main(int argc, char **argv)
{
    ServerHandler serv(55555);
    serv.start();
}