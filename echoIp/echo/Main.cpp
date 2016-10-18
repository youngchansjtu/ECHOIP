//
//  main.cpp
//  dnsmodel
//
//  Created by chan young on 13-10-31.
//  Copyright (c) 2013年 chan young. All rights reserved.
//



#include "Config.h"
#pragma pack(push, 1)

//Head of DNS packet
struct DnsHdr {
    UInt16 id;
    UInt16 flags;
    UInt16 questNum;
    UInt16 answerNum;
    UInt16 authorNum;
    UInt16 additionNum;
}ATTR_PACKED;

//Query part of DNS packet
struct QueryHdr{
    UInt16 type;
    UInt16 cls;
}ATTR_PACKED;

//Response part of DNS packet
struct ResponseHdr {
    UInt16 pos;
    UInt16 type;
    UInt16 clss;
    UInt32 ttl;
    UInt16 len;
}ATTR_PACKED;
#pragma pack(pop)



enum{
    Type_A = 0x1,                  //query address of ipv4
    Type_AAAA = 0x1c,              //query address of ipv6
};


//Generate answer packet in the string of the packet received
Int32 buildReponseData(char* msg, sockaddr* pcliaddr)
{
    Int32 size = 0;
    UInt16 tmp = 0;
    char* p = msg;
    
    //keep the packet id the same as received
    p +=2;
    
    //set the flag
    size += 2;
    tmp = htons(0x8180);
    memcpy(p, &tmp, sizeof(short));
    
    //set address count responsed
    p +=4;
    size +=4;
    tmp = htons(0x1);
    memcpy(p, &tmp, sizeof(short));
    
    //parsing the domain
    p += 6;
    size +=6;
    UInt16 value = static_cast<short>(*p);
    while(0 != value)
    {
        p = p + value + 1;
        size += value + 1;
        value = static_cast<short>(*p);
    }
    p += 1;
    size += 1;
    
    //parsing the query type needed when generating responsor part
    UInt16 type = ntohs(*((UInt16*)p));
    
    p += 2;
    size += 2;
    
    //parsing the query type needed when generating responsor part
    UInt16 cls = *((UInt16*)p);
    p += 2;
    size += 2;
    
    //generating responsor part
    ResponseHdr rhdr;
    memset(&rhdr, 0, sizeof(ResponseHdr));
    rhdr.pos = htons((UInt16)(0xC00C));
    rhdr.type = htons(type);
    rhdr.clss = cls;
    rhdr.ttl = htonl(0x1E);
    if(AF_INET == pcliaddr->sa_family)                               //query from computer of ipv4
    {
        Int32 ad = ((sockaddr_in*)pcliaddr)->sin_addr.s_addr;
		if(Type_A == type)                                              //ipv4
			rhdr.len = htons(4);
		else if(Type_AAAA == type)                                        //ipv6
			rhdr.len = htons(16);
        memcpy(p, &rhdr, sizeof(ResponseHdr));
        p += sizeof(ResponseHdr);
        //query ipv6 address from computer of ipv4, send address form like ：：ffff:x.x.x.x
		if(Type_AAAA == type)
		{
			char extra1[10] ={0,0,0,0,0,0,0,0,0,0};
            UInt16 extra2 = 0xFFFF;
			memcpy(p, extra1, 10);
            p += 10;
            memcpy(p, &extra2, 2);
			p +=2;
		}
        memcpy(p, &ad, 4);
        p += 4;
        size += sizeof(ResponseHdr) + ntohs(rhdr.len);
    }
    else if(Type_AAAA == type && AF_INET6 == pcliaddr->sa_family)             //query from computer of ipv6
    {
        rhdr.len = htons(16);
        char* ap = (char*)(&((sockaddr_in6*)pcliaddr)->sin6_addr);
        memcpy(p, &rhdr, sizeof(ResponseHdr));
        p += sizeof(ResponseHdr);
        memcpy(p, ap, 16);
        p += 16;
        size += sizeof(ResponseHdr) + 16;
        
    }
    *p = '\0';
    return size;
}




//Parsing DNS quering Packet, details reference　to the function of buildReponseData
void parseQueryData(char* pch, char* name, DnsHdr* pD, QueryHdr* pQ, ResponseHdr* pR, Int32& nameSize)
{
    char* p = pch;
    pD->id = *(UInt16*)(p);
    p +=2;
    pD->flags = *(UInt16*)(p);
    p +=2;
    pD->questNum = *(UInt16*)(p);
    p += 8;
    UInt16 value = static_cast<UInt16>(*p);
    char* q = name;
    while(0 != value)
    {
        memcpy(q, p, value + 1);
        p = p + value + 1;
        q = q + value + 1;
        nameSize += value + 1;
        value = static_cast<short>(*p);
    }
    *q = 0;
    p += 1;
    q += 1;
    *q = '\0';
    nameSize += 1;
    pQ->type = *(UInt16*)(p);
    p +=2;
    pQ->cls = *(UInt16*)(p);
    p +=2;
    pR->pos = htons((UInt16)(0xC00C));
    pR->type = pQ->type;
    pR->clss = pQ->cls;
    pR->ttl = htonl(0x542);
    pR->len = htons(0x4);
}


//Generate DNS answer Packet in a new string, details reference　to the function of buildReponseData
void packetReponseData(char* pch, char* name, DnsHdr* pD, QueryHdr* pQ, ResponseHdr* pR, int nameSize)
{
    char* p = pch;
    memcpy(p, pD, sizeof(DnsHdr));
    p += sizeof(DnsHdr);
    memcpy(p, name, nameSize);
    p += nameSize;
    memcpy(p, pQ, sizeof(QueryHdr));
    p += sizeof(QueryHdr);
    memcpy(p, pR, sizeof(ResponseHdr));
    p += sizeof(ResponseHdr);
    *p = '\0';
}



void process(int sockfd, sockaddr* pcliaddr, socklen_t clilen)
{
    Int32 n;
	Int32 totSize = 0;
    socklen_t len = clilen;
    char mesg[MAXLINE];
    memset(&mesg, 0, MAXLINE);
    n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
    if(n > 0)
    {
        totSize = buildReponseData(mesg, pcliaddr);
        if(totSize)
            sendto(sockfd, mesg, totSize, 0, pcliaddr, len);
    }
}




int main(int argc, const char * argv[])
{

    Int32 sockfd6;
    Int32 sockfd;
    //Default port is 53
    unsigned int servPort = 53;
    if(2 == argc)
        servPort = atoi(argv[1]);
    struct sockaddr_in6 servaddr6;
    struct sockaddr_in6 cliaddr6;
    struct sockaddr_in  servaddr;
    struct sockaddr_in cliaddr;
	SASTART();
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	sockfd6 = socket(AF_INET6, SOCK_DGRAM, 0);
    Int32 flag = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)(&flag), sizeof(UInt32));
	setsockopt(sockfd6, SOL_SOCKET, SO_REUSEADDR, (char*)(&flag), sizeof(UInt32));
    if(0 > sockfd || 0 >sockfd6)
    {
        std::cout<<"Sockets have been exhuasted!"<<std::endl;
        return 1;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
	memset(&servaddr6, 0, sizeof(servaddr6));
	memset(&cliaddr6, 0, sizeof(cliaddr6));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(servPort);
	servaddr6.sin6_family = AF_INET6;
	servaddr6.sin6_addr = in6addr_any;
	servaddr6.sin6_port = htons(servPort);
    
    //bind socket to ipv4 address and ipv6 adress
    Int32 res = bind(sockfd, (sockaddr*)(&servaddr), sizeof(servaddr));
	Int32 res6 = bind(sockfd6, (sockaddr*)(&servaddr6), sizeof(servaddr6));
    if(0 > res || 0 > res6)
    {
        std::cout<<"Can't bind socket!(Insufficient privileges)"<<std::endl;
        return 1;
    }
    fd_set rset;
    Int32 maxFd = sockfd > sockfd6 ? sockfd : sockfd6;
    Int32 nready = 0;
    std::cout<<"launched at port: "<< servPort << "!"<<std::endl;
    for( ; ; )
    {
        FD_ZERO(&rset);
        FD_SET(sockfd6, &rset);
        FD_SET(sockfd, &rset);
        nready = select(maxFd + 1, &rset, NULL, NULL, NULL);
        if(FD_ISSET(sockfd6, &rset))
            process(sockfd6, (sockaddr*)(&cliaddr6), sizeof(cliaddr6));
        if(FD_ISSET(sockfd, &rset))
            process(sockfd, (sockaddr*)(&cliaddr), sizeof(cliaddr));
    }
	SACLEAN();
    return 0;
}

