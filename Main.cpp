#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

using namespace std;

const int PORT = 8080;
const int BUFFER_SIZE = 4096;

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    read(client_socket, buffer, BUFFER_SIZE);

    // Log the request
    cout << "Received request:\n" << buffer << endl;

    // Check if it's a GET request
    if (strncmp(buffer, "GET", 3) == 0) {
        ifstream file("index.html");
        stringstream content;
        content << file.rdbuf();
        string body = content.str();

        string response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + to_string(body.size()) + "\r\n"
            "\r\n" + body;

        send(client_socket, response.c_str(), response.size(), 0);
    } else {
        string response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        send(client_socket, response.c_str(), response.size(), 0);
    }

    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (sockaddr *)&address, sizeof(address));
    listen(server_fd, 10);

    cout << "Server is running on port " << PORT << "...\n";

    while (true) {
        client_socket = accept(server_fd, (sockaddr *)&address, (socklen_t*)&addrlen);
        thread(handle_client, client_socket).detach();
    }

    return 0;
}
