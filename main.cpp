#include <iostream>
#include <sstream>
#include <SFML/Network.hpp>
#include "messages.pb.h"

const unsigned short PORT = 5000;
const std::string IPADDRESS("127.0.0.1");

std::string name;
std::string msgSend;
bool new_message = false;

sf::TcpSocket socket;
sf::Mutex globalMutex;
bool quit = false;

void Sender(void)
{
    while(!quit)
    {
        sf::Packet packetSend;
        bool send = false;
        globalMutex.lock();
        if(new_message)
        {
            new_message = false;
            send = true;

            Message msg;
            msg.set_name(name);
            msg.set_chat_message(msgSend);

            std::stringstream stream;
            msg.SerializeToOstream(&stream);

            packetSend << stream.str();
        }
        globalMutex.unlock();

        if(send)
            socket.send(packetSend);
    }
}

void Receiver(void)
{
    while(!quit)
    {
        std::string msgStr;
        sf::Packet packetReceive;

        socket.receive(packetReceive);
        if (packetReceive >> msgStr)
        {
            Message msg;
            msg.ParseFromString(msgStr);

            std::cout << msg.name() << ": " << msg.chat_message() << std::endl;
        }

    }
}

void Server(void)
{
    sf::TcpListener listener;
    listener.listen(PORT);
    listener.accept(socket);
    std::cout << "New client connected: " << socket.getRemoteAddress() << std::endl;
}

bool Client(void)
{
    if(socket.connect(IPADDRESS, PORT) == sf::Socket::Done)
    {
        std::cout << "Connected\n";
        return true;
    }
    return false;
}

void GetInput(void)
{
    std::string s;
    getline(std::cin,s);

    globalMutex.lock();
    msgSend = s;
    new_message = true;
    globalMutex.unlock();
}


int main(int argc, char* argv[])
{
    std::cout << "Enter name: ";
    std::cin >> name;

    sf::Thread* receiver_thread = 0,* sender_thread = 0;

    char who;
    std::cout << "Do you want to be a server (s) or a client (c) ? ";
    std::cin  >> who;

    if (who == 's')
        Server();
    else
        Client();

    receiver_thread = new sf::Thread(&Receiver);
    sender_thread = new sf::Thread(&Sender);
    receiver_thread->launch();
    sender_thread->launch();

    while(!quit)
    {
        GetInput();
    }

    if(sender_thread)
    {
        sender_thread->wait();
        delete sender_thread;
    }

    if(receiver_thread)
    {
        receiver_thread->wait();
        delete receiver_thread;
    }
    return 0;
}