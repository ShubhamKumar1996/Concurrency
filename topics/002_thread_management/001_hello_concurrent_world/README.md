# Example 001 — Hello, Concurrent World

Part of [topic 002 — Thread management](../README.md).

## Concept

The thread lifecycle: creating a `std::thread`, observing thread identity with
`std::this_thread::get_id()`, and deterministic teardown via `std::thread::join`.

## Why it works

`join()` blocks the main thread until the worker completes. Because the worker
is always joined before the process exits, its resources are released safely and
the program has a well-defined ordering — no data races and no
`std::terminate` from destroying a still-joinable thread.

## Build and run

```sh
meson setup builddir
meson compile -C builddir
./builddir/topics/002_thread_management/topic002_hello_concurrent_world
```

## Expected output

The exact thread ids vary per run, but the structure is stable:

```
main(): entry with thread id: <id>
printConcurrentMessage(): entry with thread id: <different id>
Hello from a concurrent thread!
printConcurrentMessage(): exit()
main(): exit
```
