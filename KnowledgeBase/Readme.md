
### `std::mutex`
std::mutex bookMutex is a synchronization primitive from the C++ Standard Library used to ensure thread-safe access to shared resources. It prevents race conditions by allowing only one thread to lock and access the critical section at a time.

### `std::lock_guard<std::mutex> lock(bookMutex);`
std::lock_guard<std::mutex> lock(bookMutex); is a C++ mechanism for managing mutex locks in a thread-safe and exception-safe way. It automatically locks the bookMutex when created and ensures the mutex is automatically unlocked when the lock object goes out of scope, preventing potential deadlocks or resource leaks. This simplifies mutex management by adhering to the RAII (Resource Acquisition Is Initialization) principle.