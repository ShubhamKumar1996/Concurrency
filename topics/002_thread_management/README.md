# Topic 002 — Thread management

Managing the lifecycle of `std::thread`: how threads are started, identified,
and torn down deterministically.

This topic follows *C++ Concurrency in Action* (2nd Edition), Chapter 2
("Managing threads").

## Examples

| # | Example | Concept |
|---|---------|---------|
| 001 | [`001_hello_concurrent_world`](001_hello_concurrent_world) | Creating a `std::thread`, thread identity (`std::this_thread::get_id`), and deterministic teardown via `std::thread::join` |
| 002 | [`002_function_object`](002_function_object) | Launching a thread with a class instance that overloads `operator()` (a function object), and the copy semantics of the callable |

## Build and run

Each example builds a separate executable. From the repository root:

```sh
meson setup builddir
meson compile -C builddir
meson test -C builddir --print-errorlogs
```

Binaries are emitted next to the topic's `meson.build`, one per example, as
`builddir/topics/002_thread_management/topic002_<example>`:

```sh
./builddir/topics/002_thread_management/topic002_hello_concurrent_world
```
