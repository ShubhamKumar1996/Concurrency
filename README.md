# Concurrency — C++23 Concurrency in Action

Hands-on implementations and experiments in multithreaded and concurrent
programming with modern C++. This repository is a structured, incremental study
of concurrency primitives, memory model semantics, and lock-free techniques,
following Anthony Williams' *C++ Concurrency in Action* (2nd Edition) and
targeting the **C++23** standard.

Each example is a small, self-contained program that isolates a single concept so
it can be compiled, executed, and reasoned about in isolation. Related examples
are grouped into topics, one per section of the reference text.

## Objectives

- Build a working intuition for `std::thread`, synchronization primitives, and
  the C++ memory model through executable examples.
- Contrast correct and incorrect concurrent designs (data races, deadlocks,
  false sharing, missed wakeups) with observable behavior.
- Progress from high-level abstractions (`std::async`, futures, parallel STL)
  toward low-level atomics and lock-free data structures.
- Validate behavior empirically with sanitizers and disciplined testing.

## Prerequisites

- A C++23-capable compiler (Apple Clang 16+, GCC 13+, or MSVC 19.36+).
- [Meson](https://mesonbuild.com/) >= 1.3 and [Ninja](https://ninja-build.org/).
- Optional: a sanitizer-capable toolchain for race and memory-error detection.

```sh
# macOS (Homebrew)
brew install meson ninja
```

> On macOS the system `g++` is an alias for Apple Clang. The project pins the
> C++23 standard level via `cpp_std=c++23` in `meson.build`, so no manual
> `--std` flag is required.

## Repository Layout

```
.
├── README.md
├── meson.build                         # Root build definition (project + flags)
├── meson.options                       # Build options (e.g. -Dsanitizer=thread)
├── run.sh                              # Convenience wrapper: configure/build/test
├── .clang-format / .clang-tidy         # Formatting and static-analysis config
├── compile_commands.json -> builddir/  # Symlink for clangd (git-ignored)
├── include/
│   └── concurrency/                    # Headers shared across topics
├── topics/
│   ├── meson.build                     # Registers each topic subdirectory
│   └── 002_thread_management/          # A topic = one section of the text
│       ├── meson.build                 # Declares every example (exe + test)
│       ├── README.md                   # Topic overview and example index
│       └── 001_hello_concurrent_world/ # An example = one standalone program
│           ├── main.cpp                # std::thread lifecycle: spawn + join
│           └── README.md               # Concept notes for the example
├── tests/                              # Cross-topic / integration tests
├── docs/
│   ├── README.md                       # Documentation index
│   └── adding-content.md               # How to add examples and topics
├── tools/                              # format.sh, tidy.sh
├── subprojects/                        # Meson wrap dependencies
└── builddir*/                          # Meson build trees (git-ignored)
```

Topics are directories named `NNN_snake_case_topic`, where `NNN` mirrors the
chapter of the reference text and hence the order in which topics are studied. A
topic holds one or more examples in subdirectories named
`NNN_snake_case_example`, each with its own `main.cpp`. Target and test names
combine both numbers, so a topic section stays together while every example
builds and runs on its own. Because the first topic implemented here is Chapter
2 (*Managing threads*), the numbers begin at `002`.


## Build and Run

Meson is the build system of record and Ninja is the backend. Configure once,
then build, test, and run:

```sh
meson setup builddir          # configure (idempotent; re-run to reconfigure)
meson compile -C builddir     # compile all topics
meson test -C builddir --print-errorlogs
./builddir/topics/002_thread_management/topic002_hello_concurrent_world
```

For convenience, `run.sh` wraps the configure/build/test sequence:

```sh
./run.sh
```

Meson exports `compile_commands.json` into the build directory. A root symlink
(`compile_commands.json -> builddir/compile_commands.json`) lets clangd and
clang-tidy resolve it without knowing about the build directory:

```sh
ln -sf builddir/compile_commands.json compile_commands.json
```

### Sanitizer builds

The `sanitizer` option selects runtime instrumentation. Use a separate build
directory per configuration so the binary artifacts do not collide:

```sh
# ThreadSanitizer: data races and lock-order inversions
meson setup builddir-tsan -Dsanitizer=thread
meson compile -C builddir-tsan
meson test -C builddir-tsan --print-errorlogs

# AddressSanitizer + UndefinedBehaviorSanitizer: memory and UB errors
meson setup builddir-asan -Dsanitizer=address
meson compile -C builddir-asan
meson test -C builddir-asan --print-errorlogs
```

### Adding a topic or an example

The build is data-driven, so extending it means adding directories and
dictionary entries — never plumbing. The full walkthrough, with `meson.build`
and README templates, a naming table, a validation checklist, and
troubleshooting, lives in
[`docs/adding-content.md`](docs/adding-content.md).

In short:

- **New example** in an existing topic — create
  `topics/<topic>/NNN_snake_case_example/main.cpp` and add one line to that
  topic's `examples` dictionary.
- **New topic** — create `topics/NNN_snake_case_topic/` with its own
  `meson.build` (the same dictionary loop) and register it with
  `subdir('NNN_snake_case_topic')` in `topics/meson.build`.

## Tooling

```sh
./tools/format.sh          # clang-format all sources in place
./tools/format.sh check    # verify formatting (non-zero on diff)
./tools/tidy.sh            # clang-tidy using builddir/compile_commands.json
```

Both tools expect `clang-format` and `clang-tidy` on `PATH`
(`brew install llvm`). Continuous integration
([`.github/workflows/ci.yml`](.github/workflows/ci.yml)) runs a build + test job
and a ThreadSanitizer job on every push and pull request.

## Topics

| Topic | Example | Concept |
|-------|---------|---------|
| [`002_thread_management`](topics/002_thread_management) | [`001_hello_concurrent_world`](topics/002_thread_management/001_hello_concurrent_world) | Thread creation, thread identity (`std::this_thread::get_id`), and deterministic teardown via `std::thread::join` |
| [`002_thread_management`](topics/002_thread_management) | [`002_function_object`](topics/002_thread_management/002_function_object) | Launching a thread with a class instance that overloads `operator()` (a function object), and the copy semantics of the callable |

### 002_thread_management — 001_hello_concurrent_world

Demonstrates the fundamental thread lifecycle:

- The main thread reports its ID on entry.
- A secondary `std::thread` is spawned to run a free function.
- `join()` blocks the main thread until the worker completes, guaranteeing
  ordering and safe object destruction.

Because `join()` is called before the worker's resources are released, the
program exhibits no data races and produces output in a well-defined order.

### 002_thread_management — 002_function_object

`std::thread`'s constructor accepts any callable, not just a free function. This
example passes an instance of a class that overloads `operator()`, showing that a
function object is invoked just like an ordinary function. The callable is copied
into the thread's own storage, so the worker operates on its own copy and the
original object is left untouched.

## Conventions

- **Standard:** C++23, using the standard library exclusively; no third-party
  dependencies.
- **Naming:** topics follow `topics/NNN_snake_case_topic/`; examples follow
  `NNN_snake_case_example/main.cpp` inside a topic. Each example builds a target
  named `topicNNN_snake_case_example` and owns its `main.cpp`.
- **Structure:** build configuration lives at the repository root; exercises
  live under `topics/`; shared headers under `include/concurrency/`; cross-topic
  tests under `tests/`.
- **Style:** camelCase for functions and local variables, lower-case for
  namespaces, 4-space indentation — enforced by `.clang-format`.
- **Scope:** each example should compile and run independently, with no shared
  state between examples.
- **Instrumentation:** new examples that introduce shared mutable state should
  be verified under ThreadSanitizer before being considered complete.

## Roadmap

The progression below tracks the structure of the reference text; the checklist
reflects topics as they are implemented in this repository.

- [x] Thread management and the thread lifecycle
- [ ] Sharing data between threads and synchronization with mutexes
- [ ] Synchronizing concurrent operations (condition variables, futures, latches, barriers)
- [ ] The C++ memory model and atomic types
- [ ] Designing lock-based concurrent data structures
- [ ] Designing lock-free concurrent data structures
- [ ] Concurrency in the standard library (parallel algorithms, task-based parallelism)

## References

- Anthony Williams, *C++ Concurrency in Action*, 2nd Edition (Manning, 2019).
- ISO/IEC 14882:2024 — Programming Languages — C++.
- [`cppreference.com` — Concurrency support library](https://en.cppreference.com/w/cpp/thread).

## License

No license is currently declared. Unless otherwise noted, all code in this
repository is provided for educational purposes.