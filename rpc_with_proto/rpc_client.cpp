#include <iostream>
#include <string>
#include <winsock2.h>
#include "rpc.pb.h"
#pragma comment(lib, "ws2_32.lib")

class RPCClient {
public:
    RPCClient(const std::string& serverIP, int port) : serverIP(serverIP), port(port) {
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
        serverAddr.sin_port = htons(port);
        connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    }

    ~RPCClient() {
        closesocket(clientSocket);
        WSACleanup();
    }

    std::string call(const std::string& message) {
        rpc::Request request;
        request.set_message(message);

        std::string requestStr;
        request.SerializeToString(&requestStr);
        send(clientSocket, requestStr.c_str(), requestStr.size(), 0);

        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, 1024, 0);
        buffer[bytesReceived] = '\0';

        rpc::Response response;
        response.ParseFromString(buffer);

        return response.message();
    }

private:
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
    std::string serverIP;
    int port;
};

int main() {
    RPCClient client("127.0.0.1", 12345);
    std::string response = client.call("hello");
    std::cout << "Reply: " << response << std::endl;
    return 0;
}