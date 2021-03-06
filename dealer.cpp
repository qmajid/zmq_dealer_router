#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <assert.h>
#include <mutex>
#include <zmq.h>
#include "message_queue/dealer_node.h"

#define USLEEP 1000*1
using namespace std;
std::mutex g_i_mutex; 

void receive_response(string id, void* socket)
{
    while (true)
    {
        //printf("wait for receive in dealer...\n");
        char buffer [100] = {0};
        zmq_recv (socket, buffer, 100, 0);
        printf ("Received reply in id[%s]: [%s]\n", id.c_str(), buffer);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Usage is => ./dealer id message\n");
        return EXIT_FAILURE;
    }

    // string id = string(argv[1]);
    // string message = string(argv[2]);
    // printf("id: [%s], messsage: [%s]\n", id.c_str(), message.c_str());

    // void *context = zmq_ctx_new ();
    // void *socket = zmq_socket (context, ZMQ_DEALER);
    // zmq_setsockopt(socket, ZMQ_IDENTITY, id.c_str(), id.length());
    // zmq_connect (socket, "tcp://localhost:5555"); 

    std::thread t1, t2;
    DealerNode dealer;
    dealer.connect();
    t1 = std::thread(&DealerNode::send_message, dealer, "HELLO");
    t2 = std::thread(&DealerNode::receive_response, dealer);
    t1.join();
    t2.join();    

    // while (1)
    // {
    //     //printf("wait for send in delar...\n");
    //     int rv = zmq_send (socket, message.c_str(), message.length(), 0);
    //     printf("zmq send message [%s] with rv:[%d]\n", message.c_str(), rv);
    //     // char buffer [100] = {0};
    //     // zmq_recv (socket, buffer, 100, 0);
    //     // printf ("Received reply in id[%s]: [%s]\n", id.c_str(), buffer);
    //     usleep (USLEEP);
    // }
    // t1.join();
    return EXIT_SUCCESS;        
}