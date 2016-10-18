//
//  main.cpp
//  dnsclient
//
//  Created by chan young on 13-10-31.
//  Copyright (c) 2013年 chan young. All rights reserved.
//

#include <iostream>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#define MAXLINE 1024
void dns_req(FILE* fp, int sockfd, const sockaddr* pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
   // while (fgets(sendline, MAXLINE, fp)) {
    while (strcpy(sendline, "young")) {
        int sz = strlen(sendline);
        int tmptmp = sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        if(tmptmp < 0)
            std::cout<<"can\'t send"<<std::endl;
        n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        if(n >= 0)
        {
            recvline[n] = 0;
            std::cout<<recvline<<std::endl;
        }
        sleep(10);
        
    }
}

int main(int argc, const char * argv[])
{

    // insert code here...
    int sockfd;
    unsigned int port = 8881;
    struct sockaddr_in6 servaddr;
    /*
    if(argc != 2)
    {
        std::cout<<"usage: dnsclient <IPaddress>"<<std::endl;
        return 1;
    }
     */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin6_family = AF_INET;
    servaddr.sin6_port = htons(port);
    int tmp = inet_pton(AF_INET6, /*argv[1]"192.168.0.103"*/"::1", &servaddr.sin6_addr);
    sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    
    dns_req(stdin, sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
    return 0;
}

