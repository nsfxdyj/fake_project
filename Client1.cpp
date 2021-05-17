#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WINSOCK2.H>
#include <iostream>
#include <stdio.h>
#include <string>
#include <windows.h>
#include <time.h>

#pragma comment(lib,"ws2_32.lib")

int main(int argc, char** argv)
{
    int err;
    WORD versionRequired;
    WSADATA wsaData;
    versionRequired = MAKEWORD(1, 1);
    err = WSAStartup(versionRequired, &wsaData);
    if (!err);
    else {
        printf("ERROR:客户端的嵌套字打开失败!\n");
        return 1;
    }
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);


    SOCKADDR_IN clientsock_in;
    clientsock_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    clientsock_in.sin_family = AF_INET;
    clientsock_in.sin_port = htons(20001);

    connect(clientSocket, (SOCKADDR*)&clientsock_in, sizeof(SOCKADDR));
    char receiveBuf[100];
    srand(time(NULL));
    int tt = rand() % 6000;
    Sleep(tt);

    srand(time(NULL));
    char Buf[101]= "Hello Baby";
    int port = rand()%8;
    printf("%d\n",port);


    while (true) {
        char sendBuf[101];
        sendBuf[0] = '0' + port;
        int i;
        for (i = 0; Buf[i] != '\0'; i++) {
            sendBuf[i + 1] = Buf[i];
        }
        sendBuf[i + 1] = '\0';
        send(clientSocket, sendBuf, 100, 0);

        //recv(clientSocket, receiveBuf, 100, 0);
        //printf("Recv:%s\n", receiveBuf);
        while (true) {
            Sleep(1000);
        }
    }
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}