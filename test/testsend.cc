#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
using namespace std;

int main(int argc, char **argv)
{
    string servIP("192.168.0.102");
    uint16_t port = 55555;

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    inet_pton(AF_INET, servIP.c_str(), &servaddr.sin_addr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    int one = 1;
    sleep(10);
    send(sockfd, &one, sizeof(one), 0);
    cout << "send return" << endl;
    return 0;
}