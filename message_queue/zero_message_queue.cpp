#include "zero_message_queue.h"
#include <assert.h>
#include <stdio.h>
#include <cstring>
#include <thread>
#include <unistd.h>

#define USLEEP 1000*1

ZeroMessageQueue::ZeroMessageQueue(/* args */)
    : context(zmq_ctx_new())
    , mutex_(new std::mutex())
{
}

ZeroMessageQueue::~ZeroMessageQueue()
{
}

bool ZeroMessageQueue::start_listen() 
{
    socket = zmq_socket (context, ZMQ_ROUTER);
    if (socket == NULL)
    {
        printf("zmq_socket error with err:[%s]\n", zmq_strerror(errno));
        return false;
    }

    int rc = zmq_bind (socket, "tcp://*:5555");
    if (rc == -1)
    {
        printf("zmq_bind error with err:[%s]\n", zmq_strerror(errno));
    }
    assert (rc == 0);
    return (rc == 0);

    // bool already_start_response_thread = false;
    // std::thread t1, t2, t3, read_thread;

    // read_thread = std::thread(&ZeroMessageQueue::read_message, *this);
    // t1 = std::thread(&ZeroMessageQueue::send_reply4, *this, "A", "0123654789");
    // t2 = std::thread(&ZeroMessageQueue::send_reply4, *this, "A", "abcdefghij");
    // read_thread.join();
    // t1.join();
    // t2.join();

    // while (1) {
    //     //printf("wait for new data in router...\n");
    //     char id [10+1] = {0};
    //     mutex_->lock();
    //     zmq_recv (socket, id, 10, ZMQ_DONTWAIT);

    //     char data[10+1] = {0};
    //     zmq_recv (socket, data, 10, ZMQ_DONTWAIT);
    //     if (strlen(data))
    //         printf ("rcv data[%s] with len[%lu] from id[%s]\n", data, strlen(data), id);
    //     mutex_->unlock();

    //     //send_reply(id, data);
    //     if (!already_start_response_thread)
    //     {
    //         //t1 = std::thread(&ZeroMessageQueue::send_reply2, *this, socket, std::string(id), "abcdefghij");
    //         //t2 = std::thread(&ZeroMessageQueue::send_reply3, *this, socket, std::string(id), "0123456789");
            
    //         t1 = std::thread(&ZeroMessageQueue::send_reply4, *this, "A", "abcdefghij");
    //         //t2 = std::thread(&ZeroMessageQueue::send_reply4, *this, std::string(id), "0123456789");
    //         //t3 = std::thread(&ZeroMessageQueue::send_reply4, *this, std::string(id), "QAZWSXEDCR");

    //         already_start_response_thread = true;
    //     }
    //     //printf("after run thread.\n");
    // }
    // t1.join();
    // t2.join(); 
    // t3.join();    
    return (rc == 0);
}

int ZeroMessageQueue::send_reply(std::string key, std::string data) 
{
    mutex_->lock();
    zmq_send(socket, key.c_str(), key.length(), ZMQ_SNDMORE);
    int rv = zmq_send(socket, data.c_str(), data.length(), 0);
    //message_vector.push_back(data);
    mutex_->unlock();
    printf("send_reply with data[%s] rv:[%d] ...\n", data.c_str(), rv);
    //printf("-----------------------\n");
    return rv;    
}

void ZeroMessageQueue::read_message()
{
    int nbytes = 0;
    while (true)
    {
        char id [10+1] = {0};
        char data[10+1] = {0};
        
        mutex_->lock();
        nbytes = zmq_recv (socket, id, 10, ZMQ_DONTWAIT);
        if (nbytes > 0)
            nbytes = zmq_recv (socket, data, 10, 0);
        mutex_->unlock();
        if (nbytes > 0)
            printf ("rcv data[%s] with len[%lu] from id[%s] nbytes[%d]\n", data, strlen(data), id, nbytes);
        usleep(USLEEP);
    }
    
}

void ZeroMessageQueue::send_reply2(void *socket, std::string key, std::string data)
{
    //printf("enter send_reply2 key[%s] data[%s]\n", key.c_str(), data.c_str());
    while (true)
    {
        int rv = send_reply(key, data);
        printf("send_reply2 with key[%s] data[%s] rv:[%d] ...\n", key.c_str(), data.c_str(), rv);
        printf("-----------------------\n");
        usleep(USLEEP);
    }
}

void ZeroMessageQueue::send_reply3(void *socket, std::string key, std::string data)
{
    while (true)
    {
        int rv = send_reply(key, data);
        printf("send_reply3 with key[%s] data[%s] rv:[%d] ...\n", key.c_str(), data.c_str(), rv);
        printf("-----------------------\n");
        usleep(USLEEP);
    }
}

void ZeroMessageQueue::send_reply4(std::string key, std::string data)
{
    while (true)
    {
        int rv = send_reply(key, data);
        //printf("send_reply4 with key[%s] data[%s] rv:[%d] ...\n", key.c_str(), data.c_str(), rv);
        //printf("-----------------------\n");
        usleep(USLEEP);
    }
}

void ZeroMessageQueue::send_reply5(std::string key, std::string data)
{
    while (true)
    {
        int rv = send_reply(key, data);
        printf("send_reply5 with key[%s] data[%s] rv:[%d] ...\n", key.c_str(), data.c_str(), rv);
        printf("-----------------------\n");
        usleep(USLEEP);
    }
}

void ZeroMessageQueue::connect(std::string key)
{
    void *socket = zmq_socket (context, ZMQ_DEALER);
    zmq_setsockopt(socket, ZMQ_IDENTITY, key.c_str(), key.length());
    zmq_connect (socket, "tcp://localhost:5555"); 
}