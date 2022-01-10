#include <iostream>
#include "zmq.hpp"
#include <string>
#include <sstream>
#include <zconf.h>
#include <exception>
#include <fstream>
#include <vector>
#include <signal.h>
#include "server_functions.h"

int main(int argc, char **argv)
{
    int id = std::stoi(argv[1]);
    int parent_port = std::stoi(argv[2]);
    zmq::context_t context;
    zmq::socket_t parent_socket(context, ZMQ_REP);
    parent_socket.connect(get_port_name(parent_port));
    int son_pid = 0;
    int son_id = 0;
    int linger = 0;
    int son_counter = 0;
    std::string answ;
    zmq::socket_t son_socket(context, ZMQ_REQ);
    son_socket.setsockopt(ZMQ_SNDTIMEO, 2000);
    son_socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
    int son_port = bind_socket(son_socket);
    while (true)
    {
        std::string request_string;

        request_string = recieve_message(parent_socket);
        std::istringstream command_stream(request_string);
        std::string command;
        command_stream >> command;

        if (command == "id")
        {
            std::string parent_string = "Ok:" + std::to_string(id);
            send_message(parent_socket, parent_string);
        }
        else if (command == "pid")
        {
            std::string parent_string = "Ok:" + std::to_string(getpid());
            send_message(parent_socket, parent_string);
        }
        else if (command == "create")
        {
            int id_to_create;
            int parent_id;
            command_stream >> parent_id;
            if (parent_id == id)
            {
                son_counter++;
                command_stream >> id_to_create;
                son_pid = fork();
                if (son_pid == -1)
                {
                    send_message(parent_socket, "Error: Cannot fork");
                    son_pid = 0;
                }
                else if (son_pid == 0)
                {
                    create_node(id_to_create, son_port);
                }
                else
                {
                    son_id = id_to_create;
                    for (int i = 0; i < son_counter; i++)
                    {
                        send_message(son_socket, " pid");
                        answ = recieve_message(son_socket);
                        if (answ.substr(0, 2) == "Ok")
                            break;
                    }
                    send_message(parent_socket, answ);
                }
            }
            else
            {
                send_message(son_socket, request_string);
                send_message(parent_socket, recieve_message(son_socket));
            }
        }
        else if (command == "exec")
        {
            int exec_id;
            command_stream >> exec_id;
            if (exec_id == id)
            {
                std::string example;
                std::string search;
                std::vector<int> res;
                command_stream >> example >> search;
                for (int i = example.find(search, i++); i != std::string::npos; i = example.find(search, i + 1))
                    res.push_back(i);
                std::string recieve_message = "Ok:" + std::to_string(id) + ":";
                for (int i = 0; i < res.size(); i++)
                {
                    recieve_message += std::to_string(res[i]) + ";";
                }
                send_message(parent_socket, recieve_message);
            }
            else
            {
                for (int i = 0; i < son_counter; i++)
                {
                    send_message(son_socket, request_string);
                    answ = recieve_message(son_socket);
                    if (answ.substr(0, 2) == "Ok")
                        break;
                }
                send_message(parent_socket, answ);
            }
        }
        else if (command == "ping")
        {
            int pingid;
            command_stream >> pingid;
            if (pingid == id)
            {
                send_message(parent_socket, "Ok:" + std::to_string(id));
            }
            else
            {
                for (int i = 0; i < son_counter; i++)
                {
                    send_message(son_socket, request_string);
                    answ = recieve_message(son_socket);
                    if (answ.substr(0, 2) == "Ok")
                        break;
                }
                send_message(parent_socket, answ);
            }
        }
        else if (command == "kill")
        { // УБИТЬ ВСЕХ ДЕТЕЙ
            int killid;
            command_stream >> killid;
            if (!killid)
            {
                send_message(parent_socket, "OK");
                for (int i = 0; i < son_counter; i++)
                {
                    send_message(son_socket, "kill 0");
                    recieve_message(son_socket);
                }
                sleep(2);
                zmq_close(son_socket);
                close(son_port);
                exit(1);
            }
            else
            {
                if (killid == id)
                {
                    answ = "Ok:" + std::to_string(id);
                    send_message(parent_socket, answ);
                    for (int i = 0; i < son_counter; i++)
                    {
                        send_message(son_socket, "kill 0");
                        recieve_message(son_socket);
                    }
                    sleep(2);
                    zmq_close(son_socket);
                    close(son_port);
                    exit(1);
                }
                else
                {
                    for (int i = 0; i < son_counter; i++)
                    {
                        send_message(son_socket, request_string);
                        answ = recieve_message(son_socket);
                        if (answ.substr(0, 2) == "Ok")
                            break;
                    }
                    send_message(parent_socket, answ);
                }
            }
        }
        if (parent_port == 0)
        {
            break;
        }
    }
}