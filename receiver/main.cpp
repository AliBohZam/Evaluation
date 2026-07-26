#include "ipc.hpp"
#include "logger.hpp"

#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <thread>

int main()
{
    try
    {
        SharedMemory sharedMemory;

        //
        // Wait until the initiator creates the shared memory.
        //
        while (true)
        {
            try
            {
                sharedMemory.open();
                break;
            }
            catch (...)
            {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(100));
            }
        }

        NamedSemaphore initiatorSemaphore;
        NamedSemaphore receiverSemaphore;

        while (true)
        {
            try
            {
                initiatorSemaphore.open(
                    ipc::InitiatorSemaphore);

                receiverSemaphore.open(
                    ipc::ReceiverSemaphore);

                break;
            }
            catch (...)
            {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(100));
            }
        }

        auto& state = sharedMemory.data();

        LOG("Receiver", "Receiver ready.");

        while (true)
        {
            receiverSemaphore.wait();

            LOG("Receiver",
                "Received value: ",
                state.value);

            if (state.value < 10) {
                state.value++;

                LOG("Receiver",
                    "Incremented to: ",
                    state.value);

                LOG("Receiver", "Sending value: ", state.value);

                initiatorSemaphore.post();
            } else {
                LOG("Receiver",
                    "Termination value reached.");

                break;
            }
        }

        receiverSemaphore.close();
        initiatorSemaphore.close();
        sharedMemory.close();

        LOG("Receiver", "Receiver exiting.");

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Receiver error: "
                  << e.what()
                  << '\n';

        return EXIT_FAILURE;
    }
}
