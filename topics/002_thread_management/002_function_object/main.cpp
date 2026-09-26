// Example 002 — Launching a thread with a function object.
//
// std::thread accepts *any* callable, not just a free function. A class that
// overloads operator() is itself callable, so an instance of it can be handed
// straight to the std::thread constructor.

#include <iostream>
#include <string>
#include <thread>
#include <utility>

// A function object (functor): an object that behaves like a function because it
// overloads operator(). Declaring operator() const lets the stored copy be
// invoked without mutable access.
class MessageTask {
public:
    explicit MessageTask(std::string text) : text_(std::move(text)) {}

    void operator()() const {
        std::cout << "MessageTask::operator(): entry with thread id: " << std::this_thread::get_id()
                  << std::endl;
        std::cout << text_ << std::endl;
        std::cout << "MessageTask::operator(): exit()" << std::endl;
    }

private:
    std::string text_;
};

int main() {
    std::cout << "\nmain(): entry with thread id: " << std::this_thread::get_id() << std::endl;

    MessageTask task("Hello from a function object!");
    // The functor is *copied* into the thread's own storage; `task` itself is
    // never touched by the worker. (Writing std::thread worker(MessageTask());
    // would instead declare a function — the "most vexing parse".)
    std::thread worker(task);

    worker.join();
    std::cout << "main(): exit" << std::endl;
    return 0;
}
