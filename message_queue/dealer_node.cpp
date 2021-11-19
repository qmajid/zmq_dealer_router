#include "dealer_node.h"
#include <unistd.h>
#include <thread>

#define USLEEP 1000*1

DealerNode::DealerNode(/* args */)
    : context(zmq_ctx_new())
    , mutex_(new std::mutex())
    , id("A")
{
}

DealerNode::~DealerNode()
{
}

bool DealerNode::connect() 
{
    socket = zmq_socket (context, ZMQ_DEALER);
    zmq_setsockopt(socket, ZMQ_IDENTITY, id.c_str(), id.length());
    int rv = zmq_connect (socket, "tcp://localhost:5555");
    assert(rv == 0);
    return (rv == 0);
}

void DealerNode::send_message(std::string message) 
{
    while (true)
    {
        mutex_->lock();
        int rv = zmq_send (socket, message.c_str(), message.length(), 0);
        mutex_->unlock();
        printf("zmq send message [%s] with rv:[%d]\n", message.c_str(), rv);
        if (rv == -1)
        {
            printf("can not send because [%s]\n", zmq_strerror(errno));
        }
        usleep (USLEEP);        
    }
    
}

void DealerNode::receive_response() 
{
    while (true)
    {
        char buffer [100] = {0};
        mutex_->lock();
        int nbytes = zmq_recv (socket, buffer, 100, ZMQ_DONTWAIT);
        mutex_->unlock();
        if (nbytes > 0)
            printf ("Received reply in id[%s]: [%s]\n", id.c_str(), buffer);
    }    
}
