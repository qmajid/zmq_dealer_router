#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <cstring>
#include <set>
#include <ctime>

#include <zmq.h>

using namespace std;

typedef struct
{
    uint16_t event; // id of the event as bitfield
    int32_t value;  // value is either error code, fd or reconnect interval
} zmq_event_t;

std::set<int> socket_set;

static int read_msg(void* s, zmq_event_t* event, char* ep)
{
    int rc ;
    zmq_msg_t msg1;  // binary part
    zmq_msg_init (&msg1);
    zmq_msg_t msg2;  //  address part
    zmq_msg_init (&msg2);
    rc = zmq_msg_recv (&msg1, s, 0);
    if (rc == -1 && zmq_errno() == ETERM)
        return 1 ;
    assert (rc != -1);
    assert (zmq_msg_more(&msg1) != 0);
    rc = zmq_msg_recv (&msg2, s, 0);
    if (rc == -1 && zmq_errno() == ETERM)
        return 1;
    assert (rc != -1);
    assert (zmq_msg_more(&msg2) == 0);
    // copy binary data to event struct
    const char* data = (char*)zmq_msg_data(&msg1);
    memcpy(&(event->event), data, sizeof(event->event));
    memcpy(&(event->value), data+sizeof(event->event), sizeof(event->value));
    // copy address part
    const size_t len = zmq_msg_size(&msg2) ;
    ep = (char*)memcpy(ep, zmq_msg_data(&msg2), len);
    *(ep + len) = 0 ;
    return 0 ;
}

// REP socket monitor thread
static void *rep_socket_monitor (void *ctx)
{
    zmq_event_t event;
    static char addr[1025] ;
    int rc;

    printf("starting monitor...\n");

    void *s = zmq_socket (ctx, ZMQ_PAIR);
    assert (s);

    rc = zmq_connect (s, "inproc://monitor.rep");
    assert (rc == 0);
    while (!read_msg(s, &event, addr)) {
        switch (event.event) {
        case ZMQ_EVENT_LISTENING:
            printf ("listening socket descriptor %d\n", event.value);
            printf ("listening socket address %s\n", addr);
            break;
        case ZMQ_EVENT_ACCEPTED:
            printf ("accepted socket descriptor %d\n", event.value);
            printf ("accepted socket address %s\n", addr);
            socket_set.insert(event.value);
            break;
        case ZMQ_EVENT_CLOSE_FAILED:
            printf ("socket close failure error code %d\n", event.value);
            printf ("socket address %s\n", addr);
            break;
        case ZMQ_EVENT_CLOSED:
            printf ("closed socket descriptor %d\n", event.value);
            printf ("closed socket address %s\n", addr);
            socket_set.erase(event.value);
            break;
        case ZMQ_EVENT_DISCONNECTED:
            printf ("disconnected socket descriptor %d\n", event.value);
            printf ("disconnected socket address %s\n", addr);
            break;
        }
    }
    zmq_close (s);
    return NULL;
}

void wait_for_socket_and_fd(void* sock, int fd, long timeout)
{
   printf("wait for two objects\n");
  // wait for two objects
  zmq_pollitem_t items[2];

  // wait for ZeroMQ socket...
  items[0].socket = sock;
  items[0].fd = 0;
  items[0].events = ZMQ_POLLIN;

  // ...and wait for fd
  items[1].socket = NULL;
  items[1].fd = fd;
  items[1].events = ZMQ_POLLIN;

  // block until event occurs or timeout expires
  int ret =  zmq_poll(items, sizeof(items) / sizeof(items[0]), timeout);
  if (ret > 0)
  {
    // event occurred
    if (items[0].revents != 0)
    {
      // event occurred on sock, process it
    }
    if (items[1].revents != 0)
    {
      // event occurred on fd, process it
    }
  }
  else if (ret == 0)
  {
    // timeout expired, do something
  }
  else
  {
    // error occurred, handle it
  }
}

inline int send_reply(void* socket, const char* key, const char* data)
{
    zmq_send(socket, key, strlen(key), ZMQ_SNDMORE);
    //ZMQ_DONTWAIT
    int rv = zmq_send(socket, data, strlen(data), ZMQ_DONTWAIT);
    printf("send_reply with data[%s] rv:[%d] ...\n", data, rv);
    printf("-----------------------\n");
    return rv;
}

int main(int argc, char* argv[])
{
    int listen_port = 5555;
    string listen_address = "tcp://*:5555";

    void *context = zmq_ctx_new ();
    void *server_socket = zmq_socket (context, ZMQ_ROUTER);

    // Monitor all events on client and server sockets
    int rc = zmq_socket_monitor (server_socket, "inproc://monitor.rep", ZMQ_EVENT_ALL);
    assert (rc == 0);
    pthread_t thread ;
    rc = pthread_create (&thread, NULL, rep_socket_monitor, context);
    assert (rc == 0);

    rc = zmq_bind (server_socket, "tcp://*:5555");
    assert (rc == 0);        

    while (1) {
        char id [10+1] = {0};
        zmq_recv (server_socket, id, 10, 0);

        char data[10+1] = {0};
        zmq_recv (server_socket, data, 10, 0);
        printf ("rcv data[%s] with len[%lu] from id[%s]\n", data, strlen(data), id);

        string response = string(data);
        string new_key = "";
        time_t now = time(0);
        if (strncmp(id, "A", 1) == 0)
        {
            new_key = "B";
            response += "XXXX-" + string(ctime(&now));
        }
        else 
        {
            new_key = "A";
            response += "YYYY-" + string(ctime(&now));
        }
        printf("before send reply id[%s] new_key[%s] data[%s]\n", id, new_key.c_str(), response.c_str());
        if (socket_set.find(12) != socket_set.end())
        {
            send_reply(server_socket, new_key.c_str(), response.c_str());
        }
        else
        {
            printf("not found...\n");
            //send_reply(server_socket, id, "not found target port.");
        }        
    }

    return EXIT_SUCCESS; 
}