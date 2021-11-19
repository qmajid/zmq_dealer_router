#ifndef __DEALER_NODE_H__
#define __DEALER_NODE_H__

#include <zmq.h>
#include <string>
#include <mutex>
#include <iostream>
#include <assert.h>

class DealerNode
{
public:
    DealerNode(/* args */);
    ~DealerNode();
    bool connect();

    void send_message(std::string message);
    void receive_response();

private:
    void *context;
    void *socket;
    std::mutex* mutex_;
    std::string id;
};

#endif // __DEALER_NODE_H__