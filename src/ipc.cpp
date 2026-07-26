#include "ipc.hpp"

#include <cerrno>
#include <cstring>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace
{

[[noreturn]]
void throwSystemError(const char* message)
{
    throw std::system_error(
        errno,
        std::generic_category(),
        message);
}

}

//
// SharedMemory
//

SharedMemory::SharedMemory()
    : fd_(-1),
      state_(nullptr)
{
}

SharedMemory::~SharedMemory()
{
    close();
}

void SharedMemory::create()
{
    fd_ = shm_open(
        ipc::SharedMemoryName,
        O_CREAT | O_RDWR,
        0666);

    if (fd_ == -1)
    {
        throwSystemError("shm_open(create)");
    }

    if (ftruncate(fd_, sizeof(SharedState)) == -1)
    {
        throwSystemError("ftruncate");
    }

    void* address =
        mmap(nullptr,
             sizeof(SharedState),
             PROT_READ | PROT_WRITE,
             MAP_SHARED,
             fd_,
             0);

    if (address == MAP_FAILED)
    {
        throwSystemError("mmap(create)");
    }

    state_ = static_cast<SharedState*>(address);
    state_->value = 0;
}

void SharedMemory::open()
{
    fd_ = shm_open(
        ipc::SharedMemoryName,
        O_RDWR,
        0666);

    if (fd_ == -1)
    {
        throwSystemError("shm_open(open)");
    }

    void* address =
        mmap(nullptr,
             sizeof(SharedState),
             PROT_READ | PROT_WRITE,
             MAP_SHARED,
             fd_,
             0);

    if (address == MAP_FAILED)
    {
        throwSystemError("mmap(open)");
    }

    state_ = static_cast<SharedState*>(address);
}

void SharedMemory::close()
{
    if (state_ != nullptr)
    {
        munmap(state_, sizeof(SharedState));
        state_ = nullptr;
    }

    if (fd_ != -1)
    {
        ::close(fd_);
        fd_ = -1;
    }
}

void SharedMemory::destroy()
{
    shm_unlink(ipc::SharedMemoryName);
}

SharedState& SharedMemory::data()
{
    return *state_;
}

//
// NamedSemaphore
//

NamedSemaphore::NamedSemaphore()
    : semaphore_(SEM_FAILED)
{
}

NamedSemaphore::~NamedSemaphore()
{
    close();
}

void NamedSemaphore::create(
    const std::string& name,
    unsigned int initialValue)
{
    name_ = name;

    semaphore_ =
        sem_open(
            name.c_str(),
            O_CREAT,
            0666,
            initialValue);

    if (semaphore_ == SEM_FAILED)
    {
        throwSystemError("sem_open(create)");
    }
}

void NamedSemaphore::open(
    const std::string& name)
{
    name_ = name;

    semaphore_ =
        sem_open(
            name.c_str(),
            0);

    if (semaphore_ == SEM_FAILED)
    {
        throwSystemError("sem_open(open)");
    }
}

void NamedSemaphore::wait()
{
    while (sem_wait(semaphore_) == -1)
    {
        if (errno != EINTR)
        {
            throwSystemError("sem_wait");
        }
    }
}

void NamedSemaphore::post()
{
    if (sem_post(semaphore_) == -1)
    {
        throwSystemError("sem_post");
    }
}

void NamedSemaphore::close()
{
    if (semaphore_ != SEM_FAILED)
    {
        sem_close(semaphore_);
        semaphore_ = SEM_FAILED;
    }
}

void NamedSemaphore::destroy()
{
    if (!name_.empty())
    {
        sem_unlink(name_.c_str());
    }
}
