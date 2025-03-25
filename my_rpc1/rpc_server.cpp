#include <iostream>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

class RPCServer {
public:
    RPCServer(int port) : port(port) {
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);
        bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
        listen(serverSocket, 5);
    }

    ~RPCServer() {
        closesocket(serverSocket);
        WSACleanup();
    }

    void run() {
        std::cout << "Listening on port " << port << "..." << std::endl;
        while (true) {
            SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
            handleClient(clientSocket);
            closesocket(clientSocket);
        }
    }

private:
    void handleClient(SOCKET clientSocket) {
        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, 1024, 0);
        buffer[bytesReceived] = '\0';
        std::string request(buffer);
        std::string response = handleRequest(request);
        send(clientSocket, response.c_str(), response.size(), 0);
    }

    std::string handleRequest(const std::string& request) {
        if (request == "hello") {
            return "Hello, World!";
        }
        return "Unknown request";
    }

    WSADATA wsaData;
    SOCKET serverSocket;
    sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    int port;
};

int main() {
    RPCServer server(12345);
    server.run();
    return 0;
}