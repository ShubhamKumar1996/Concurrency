# Example 002 — Function object as the thread's entry point

Part of [topic 002 — Thread management](../README.md).

## Concept

`std::thread`'s constructor is a template that accepts any *callable* — a free
function, a lambda, a member function pointer, **or a class object that overloads
`operator()`**. Such an object is called a *function object* (or *functor*), and
an instance of it can be passed directly where a thread function is expected:

```cpp
class MessageTask {
public:
    void operator()() const { /* runs on the new thread */ }
};

MessageTask task;      // an instance of the class
std::thread worker(task);
```

## Why it works

- **Callable, not just a function.** The `std::thread` constructor is
  `template<class F, class... Args> explicit thread(F&& f, Args&&...);`. It stores
  a decay-copy of `f` and invokes it on the new thread, so anything with a
  suitable `operator()` qualifies.
- **The object is copied.** `worker` owns its own copy of `task`; the worker
  therefore cannot see later mutations of the original, and `task` remains
  untouched after `join()`. This is why functors should keep the state they
  operate on inside the object, not rely on external access.
- **`operator() const` is idiomatic.** A stateless functor used only for its
  behavior should be `const`-callable; that also makes it safe to share.

## Common pitfall

```cpp
std::thread worker(MessageTask());   // ⚠️ declares a *function*, not a thread
```

The compiler reads this as a function declaration returning `std::thread` (the
"most vexing parse"). Use a named object (`MessageTask task; std::thread
worker(task);`) or brace initialization (`std::thread worker{MessageTask{}};`).

## Build and run

```sh
meson setup builddir
meson compile -C builddir
./builddir/topics/002_thread_management/topic002_function_object
```

## Expected output

The exact thread ids vary per run, but the structure is stable:

```
main(): entry with thread id: <id>
MessageTask::operator(): entry with thread id: <different id>
Hello from a function object!
MessageTask::operator(): exit()
main(): exit
```
