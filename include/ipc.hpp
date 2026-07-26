#pragma once

#include <semaphore.h>

#include <string>
#include <system_error>

struct SharedState
{
    int value;
};

class SharedMemory
{
public:
    SharedMemory();
    ~SharedMemory();

    SharedMemory(const SharedMemory&) = delete;
    SharedMemory& operator=(const SharedMemory&) = delete;

    void create();
    void open();
    void close();
    void destroy();

    SharedState& data();

private:
    int fd_;
    SharedState* state_;
};

class NamedSemaphore
{
public:
    NamedSemaphore();
    ~NamedSemaphore();

    NamedSemaphore(const NamedSemaphore&) = delete;
    NamedSemaphore& operator=(const NamedSemaphore&) = delete;

    void create(const std::string& name,
                unsigned int initialValue);

    void open(const std::string& name);

    void wait();

    void post();

    void close();

    void destroy();

private:
    sem_t* semaphore_;
    std::string name_;
};

namespace ipc
{
constexpr const char* SharedMemoryName = "/ping_pong_shared_memory";
constexpr const char* InitiatorSemaphore = "/ping_pong_initiator_sem";
constexpr const char* ReceiverSemaphore = "/ping_pong_receiver_sem";
}
