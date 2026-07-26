#include "ipc.hpp"
#include "logger.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
    try
    {
        SharedMemory sharedMemory;
        sharedMemory.create();

        NamedSemaphore initiatorSemaphore;
        NamedSemaphore receiverSemaphore;

        initiatorSemaphore.create(ipc::InitiatorSemaphore, 0);
        receiverSemaphore.create(ipc::ReceiverSemaphore, 0);

        auto& state = sharedMemory.data();

        state.value = 0;

        LOG("Initiator", "Created shared memory and semaphores.");
        LOG("Initiator", "Sending initial value: ", state.value);

        receiverSemaphore.post();

        while (true)
        {
            initiatorSemaphore.wait();

            LOG("Initiator", "Received value: ", state.value);

            state.value++;

            LOG("Initiator",
                "Incremented to: ",
                state.value);

            LOG("Initiator", "Sending value: ", state.value);

            receiverSemaphore.post();

            if (state.value >= 10)
            {
                LOG("Initiator", "Reached termination value.");

                break;
            }
        }

        receiverSemaphore.close();
        initiatorSemaphore.close();

        receiverSemaphore.destroy();
        initiatorSemaphore.destroy();

        sharedMemory.close();
        sharedMemory.destroy();

        LOG("Initiator", "Cleanup complete.");

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Initiator error: "
                  << e.what()
                  << '\n';

        return EXIT_FAILURE;
    }
}
