# Notes
Although this implementation works, I would not ship it in production. The receiver repeatedly retries opening the shared memory and semaphores with sleep_for(100ms). It's simple, but not ideal.

A more robust architecture would have the initiator create and initialize the IPC resources, fork() and exec() the receiver (or launch it with posix_spawn() on macOS), eliminate all retry loops, use O_CREAT | O_EXCL to avoid stale IPC objects, employ stronger RAII wrappers with move semantics and automatic cleanup.

That design is cleaner, race-free, and closer to production-quality C++ systems programming.

# To build
```
mkdir build
cd build

cmake ..
make -j

Or

cmake -DENABLE_LOGGING=OFF ..
make -j
```

# To Run in terminal 1
```
./receiver
```

# To Run in terminal 2
```
./initiator
```
