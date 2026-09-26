#include <iostream>
#include <thread>

void printConcurrentMessage() {
    std::cout << "printConcurrentMessage(): entry with thread id: " << std::this_thread::get_id() << std::endl;
    std::cout << "Hello from a concurrent thread!" << std::endl;
    std::cout << "printConcurrentMessage(): exit()" << std::endl;
}

int main()  {
    std::cout << "\nmain(): entry with thread id: " << std::this_thread::get_id() << std::endl;
    std::thread concurrentThreadObject(printConcurrentMessage);
    concurrentThreadObject.join();
    std::cout << "main(): exit" << std::endl;
    return 0;
}