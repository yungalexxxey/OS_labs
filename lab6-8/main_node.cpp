#include <iostream>
#include "zmq.hpp"
#include <string>
#include <zconf.h>
#include <vector>
#include <signal.h>
#include <sstream>
#include <set>
#include <algorithm>
#include "server_functions.h"

struct treenode
{
    int data;
    treenode *parent;
    treenode *son;
    treenode *brother;
};
class IdTree
{
public:
    std::vector<int> get_nodes()
    {
        std::vector<int> result;
        get_nodes(root, result);
        return result;
    }
    bool contains(int id)
    {
        treenode *res = nullptr;
        res = find(id);
        return res != nullptr;
    }
    void erase(int id)
    {
        if (root->data == id)
        {
            destroy();
        }
        treenode *finds = nullptr;
        finds = find(id);
        if (finds == nullptr)
            return;
        deleteunder(finds);
        deletenode(root, id);
    }
    void createtree(int id)
    {
        root = new treenode;
        root->data = id;
        root->brother = NULL;
        root->son = NULL;
        root->parent = NULL;
    }
    void Print_Tree()
    {
        print_tree(root, 0);
    }
    treenode *find(int id)
    {
        return find(root, id);
    }
    void insert(int parent, int id)
    {
        treenode *node = nullptr;
        node = find(parent);
        if (node == nullptr)
            return;
        treenode *par = node;
        if (node->son == NULL)
        {
            node->son = createnode(id);
            node->son->parent = node;
            return;
        }
        node = node->son;
        while (node->brother != NULL)
        {
            node = node->brother;
        }
        node->brother = createnode(id);
        node->brother->parent = par;
    }

private:
    treenode *root;
    void get_nodes(treenode *node, std::vector<int> &v) const
    {
        if (node == nullptr)
        {
            return;
        }
        get_nodes(node->brother, v);
        v.push_back(node->data);
        get_nodes(node->son, v);
    }
    void destroy()
    {
        destroy(root);
    }
    void destroy(treenode *node)
    {
        if (node->son)
        {
            destroy(node->son);
        }
        if (node->brother)
        {
            destroy(node->brother);
        }
        free(node);
        node = NULL;
    }
    void deleteunder(treenode *node)
    {
        if (node == NULL)
        {
            return;
        }
        else
        {
            if (node->son)
            {
                node = node->son;
            }
            else
            {
                return;
            }
            if (node->son)
            {
                deleteunder(node->son);
            }
            treenode *next = node;
            treenode *prev = NULL;
            while (next->brother)
            {
                prev = next;
                next = next->brother;
                if (next->son)
                {
                    deleteunder(next->son);
                }
                free(prev);
            }
            delete next;
        }
    }
    void deletenode(treenode *node, int id)
    {
        if (node != NULL)
        {
            if (node->son != NULL)
            {
                if (node->son->data == id)
                {
                    treenode *prev = root->son;
                    node->son = node->son->brother;
                    free(prev);
                    prev = NULL;
                }
                else
                {
                    deletenode(node->son, id);
                }
            }
            if (node->brother != NULL)
            {
                if (node->brother->data == id)
                {
                    treenode *prev = node->brother;
                    node->brother = node->brother->brother;
                    free(prev);
                    prev = NULL;
                }
                else
                {
                    deletenode(node->brother, id);
                }
            }
        }
    }
    void print_tree(treenode *tree, int count)
    {
        if (tree != NULL)
        {
            for (int i = 0; i < count; i++)
            {
                printf(" ");
            }
            printf("%d", tree->data);
            printf("\n");
        }
        if (tree->son != NULL)
        {
            print_tree(tree->son, count + 2);
        }
        if (tree->brother != NULL)
        {
            print_tree(tree->brother, count);
        }
    }
    treenode *createnode(int id)
    {
        treenode *node = new treenode;
        node->data = id;
        node->son = NULL;
        node->brother = NULL;
        node->parent = NULL;
        return node;
    }
    treenode *find(treenode *node, int id)
    {
        if (node == NULL)
            return NULL;
        if (node->data == id)
        {
            return node;
        }
        treenode *findnode;
        if (node->son != NULL)
        {
            findnode = find(node->son, id);
            if (findnode != NULL)
                return findnode;
        }
        if (node->brother != NULL)
        {
            findnode = find(node->brother, id);
            if (findnode != NULL)
                return node;
        }
    }
};

int main()
{
    std::string command;
    IdTree ids;
    ids.createtree(-1);
    size_t child_pid = 0;
    int child_id = 0;
    zmq::context_t context(1);
    zmq::socket_t main_socket(context, ZMQ_REQ);
    int linger = 0;
    main_socket.setsockopt(ZMQ_SNDTIMEO, 2000);
    main_socket.setsockopt(ZMQ_RCVTIMEO, 2000);
    //main_socket.setsockopt(ZMQ_RCVTIMEO, 2000);
    main_socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
    //main_socket.connect(get_connect_name(30000));
    int port = bind_socket(main_socket);
    std::vector<zmq::socket_t> sons;

    while (true)
    {
        std::cin >> command;
        if (command == "create")
        {
            size_t node_id;
            int parent_id;
            std::string result;
            std::cin >> node_id;
            std::cin >> parent_id;
            std::cout << node_id << " " << parent_id << std::endl;
            if (ids.find(parent_id) != nullptr && ids.find(node_id) == nullptr)
            {
                if (child_pid == 0)
                {
                    child_pid = fork();
                    if (child_pid == -1)
                    {
                        std::cout << "Unable to create first worker node\n";
                        child_pid = 0;
                        exit(1);
                    }
                    else if (child_pid == 0)
                    {
                        create_node(node_id, port);
                    }
                    else
                    {
                        child_id = node_id;
                        send_message(main_socket, "pid");
                        result = recieve_message(main_socket);
                    }
                }
                else
                {
                    std::ostringstream msg_stream;
                    msg_stream << "create " << parent_id << " " << node_id;
                    send_message(main_socket, msg_stream.str());
                    result = recieve_message(main_socket);
                }

                if (result.substr(0, 2) == "Ok")
                {
                    ids.insert(parent_id, node_id);
                }
                std::cout << result << "\n";
            }
            else
            {
                std::cout << "Can't create : already exist or doesnt exist\n";
            }
        }
        else if (command == "remove")
        {
            size_t node_id;
            std::cin >> node_id;
            std::string message_string = "kill " + std::to_string(node_id);
            send_message(main_socket, message_string);
            std::string recieved_message = recieve_message(main_socket);
            if (recieved_message.substr(0, 2) == "Ok")
            {
                ids.erase(node_id);
            }
            else{
                std::cout<<recieved_message.substr(0,2)<<std::endl;
            }
            std::cout << recieved_message << "\n";
        }
        else if (command == "exec")
        {
            int id;
            std::string example;
            std::string search;
            std::cin >> id>>example>>search;

            std::string message_string = "exec " + std::to_string(id)+" "+ example + " " + search;
            send_message(main_socket, message_string);
            std::string recieved_message = recieve_message(main_socket);
            std::cout << recieved_message << "\n";
        }
         else if (command == "ping")
        {
            int pingid;
            std::cin>>pingid;
            send_message(main_socket, "ping " +std::to_string(pingid));
            std::string recieved = recieve_message(main_socket);
            std::cout<<recieved<<std::endl;
        }
        else if (command == "exit")
        {
            send_message(main_socket,"kill 0");
            recieve_message(main_socket);
            //sleep(2);
            zmq_close(main_socket);
            close(port);
            exit(1);
            break;
        }
        else
        {
            std::cout << "Try again\n";
            continue;
        }
    }

    return 0;
}