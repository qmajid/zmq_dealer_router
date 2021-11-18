#ifndef __ZERO_MESSAGE_QUEUE_H__
#define __ZERO_MESSAGE_QUEUE_H__

#include <zmq.h>
#include <string>
#include <mutex>
#include <iostream>
#include <vector>

class ZeroMessageQueue
{
public:
    ZeroMessageQueue(/* args */);
    ~ZeroMessageQueue();
    bool start_listen();
    int send_reply(std::string key, std::string data);
    void send_reply2(void *socket, std::string key, std::string data);
    void send_reply3(void *socket, std::string key, std::string data);
    void send_reply4(std::string key, std::string data);
    void send_reply5(std::string key, std::string data);
    void read_message();

    void connect(std::string key);

private:
    void *context;
    void *socket;
    std::mutex* mutex_;
    std::vector<std::string> message_vector;
};

#endif // __ZERO_MESSAGE_QUEUE_H__