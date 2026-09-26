# Adding content

Reference guide for extending this repository. Two operations are covered:

- [Adding an example](#adding-an-example) — a new program inside an existing topic
- [Adding a topic](#adding-a-topic) — a new section that groups one or more examples

Both are cheap because the build is **data-driven**. The shell is already wired
(`meson.build` → `topics/meson.build` → each topic's `meson.build`), so you only
ever add data — directories and dictionary entries — never build plumbing.

## Branching and pull requests

`main` is the integration branch: it always builds, always passes its tests, and
matches `origin/main`. Nothing is committed to it directly — every change arrives
through a pull request.

A **topic is one unit of review**, so each topic gets its own branch, cut from the
current `main` and named after its directory (`topic/NNN_snake_case_topic`):

```sh
git switch main
git pull --ff-only
git switch -c topic/002_thread_management
git push -u origin topic/002_thread_management
```

Keep the branch focused on that topic: its examples and READMEs, its
`meson.build`, and the index updates they require. Unrelated fixes belong on
their own branch.

### Finishing a topic

1. Rebase onto the latest `main` and confirm the topic still builds clean:

   ```sh
   git fetch origin
   git rebase origin/main
   meson compile -C builddir
   meson test -C builddir --print-errorlogs
   ```

2. Push the branch and open a pull request into `main`:

   ```sh
   git push
   ```

   Use the GitHub CLI (`gh pr create --base main --fill`) or follow the link
   GitHub prints when a branch is first pushed:

   ```text
   https://github.com/ShubhamKumar1996/Concurrency/compare/main...topic/002_thread_management
   ```

3. Merge once the CI workflow (build + tests + ThreadSanitizer) is green, then
   delete the branch — `git branch -d <branch>` locally and
   `git push origin --delete <branch>` on the remote.

Non-topic changes (docs, tooling, typo fixes) take the same branch-and-PR route,
with a prefix such as `docs/` or `chore/` instead of `topic/`. `main` is the only
long-lived branch.

## Layout recap

```
topics/
└── NNN_snake_case_topic/            # a topic = one section of the book
    ├── meson.build                  # declares every example (executable + test)
    ├── README.md                    # topic overview + example index
    └── NNN_snake_case_example/      # an example = one standalone program
        ├── main.cpp                 # its single entry point
        └── README.md                # concept notes for the example
```

- The topic's `meson.build` is the **only** file that lists examples.
- Executable and test names combine both numbers: `topicNNN_<example>`.
- Binaries land flat in `builddir/topics/<topic>/` — Meson keys the output path
  off the `meson.build` location, not off the source subdirectory.

## Adding an example

Worked example: add `002_function_object` to topic `002_thread_management`.

### 1. Create the directory and entry point

```sh
mkdir -p topics/002_thread_management/002_function_object
$EDITOR topics/002_thread_management/002_function_object/main.cpp
```

Rules:

- One example = one program = exactly one `main()`, in `main.cpp`.
- **Self-contained:** an example must compile and run on its own, sharing no
  mutable state with other examples. If two examples need the same code, promote
  it to `include/concurrency/` instead of cross-including between examples.
- **Conventions:** C++23 standard library only (no third-party dependencies);
  camelCase functions and locals, CamelCase classes, lower-case namespaces;
  4-space indent, `std::endl` for output, lines ≤ 100 columns — all enforced by
  `.clang-format`.

### 2. Register it in the topic's `meson.build`

Open `topics/002_thread_management/meson.build` and add **one line** to the
dictionary. The `key` is the clean example name used in the target/test; the
`value` is the numbered directory:

```meson
examples = {
  'hello_concurrent_world' : '001_hello_concurrent_world',
  'function_object'        : '002_function_object',   # <-- added
}

foreach name, dir : examples
  exe = executable('topic002_' + name,
    dir / 'main.cpp',
    cpp_args : cxx_args,
    link_args : link_args,
    dependencies : threads,
  )
  test('topic002_' + name, exe)
endforeach
```

That single edit yields the executable `topic002_function_object` and a smoke
test of the same name. Do **not** add a second `executable()`/`test()` pair by
hand — the loop already covers every dictionary entry.

Note that `cxx_args`, `link_args`, and `threads` are defined once in the root
`meson.build` and inherited into every `subdir()` scope; a topic never
redefines them.

### 3. Document the example

Create `topics/002_thread_management/002_function_object/README.md`:

````markdown
# Example NNN — <Title>

Part of [topic NNN — <Topic title>](../README.md).

## Concept

What primitive or idea this isolates, in a sentence or two.

## Why it works

The mechanism, and what would go wrong without it.

## Build and run

```sh
./builddir/topics/NNN_snake_case_topic/topicNNN_<example>
```

## Expected output

```
<stable structure; call out any part that varies per run, e.g. thread ids>
```
````

Then add a row to the **Examples** table in
`topics/NNN_snake_case_topic/README.md`, and — if you keep the root index
current — a row to the **Topics** table in the root `README.md`.

### 4. Build, test, run

```sh
meson compile -C builddir                       # auto-regenerates on meson.build change
meson test -C builddir --print-errorlogs
./builddir/topics/002_thread_management/topic002_function_object
```

If the directory exists but the target does not appear, force a reconfigure:

```sh
meson setup --reconfigure builddir
```

### 5. Validate (recommended)

```sh
./tools/format.sh                               # clang-format in place (needs LLVM)
meson setup --reconfigure builddir-tsan -Dsanitizer=thread
meson compile -C builddir-tsan
meson test -C builddir-tsan --print-errorlogs
```

Any new example that introduces shared mutable state should pass clean under
ThreadSanitizer before you consider it finished.

## Adding a topic

Worked example: add a new topic `003_sharing_data`.

### 1. Create the topic and its first example

```sh
mkdir -p topics/003_sharing_data/001_protecting_with_mutex
$EDITOR topics/003_sharing_data/001_protecting_with_mutex/main.cpp
```

### 2. Add the topic's `meson.build`

Create `topics/003_sharing_data/meson.build` — the same dictionary loop, with the
topic number in the target prefix:

```meson
# Topic 003 — Sharing data between threads.
#
# key   -> clean example name, used in the executable and test names
# value -> numbered example directory; the prefix preserves study order
examples = {
  'protecting_with_mutex' : '001_protecting_with_mutex',
}

foreach name, dir : examples
  exe = executable('topic003_' + name,
    dir / 'main.cpp',
    cpp_args : cxx_args,
    link_args : link_args,
    dependencies : threads,
  )
  test('topic003_' + name, exe)
endforeach
```

The `topicNNN_` prefix must match the topic number so target names stay unique
across the repository.

### 3. Register the topic in `topics/meson.build`

```meson
subdir('002_thread_management')
subdir('003_sharing_data')      # <-- added, in study order
```

### 4. Add the topic `README.md`

Create `topics/003_sharing_data/README.md`:

````markdown
# Topic NNN — <Title>

One-paragraph overview of the section, and which chapter of the reference text
it follows.

## Examples

| # | Example | Concept |
|---|---------|---------|
| 001 | [`001_protecting_with_mutex`](001_protecting_with_mutex) | <one-line concept> |

## Build and run

Each example builds a separate executable. From the repository root:

```sh
meson setup builddir
meson compile -C builddir
meson test -C builddir --print-errorlogs
```

Binaries are emitted as
`builddir/topics/NNN_snake_case_topic/topicNNN_<example>`:

```sh
./builddir/topics/NNN_snake_case_topic/topicNNN_<example>
```
````

### 5. Index the topic in the root README

Add one row per example to the **Topics** table, and optionally a short
subsection per example. Tick the matching item in the **Roadmap** once the
topic's examples are in place.

### 6. Build, test, run

Same as for an example; `meson compile -C builddir` picks up the new `subdir()`:

```sh
meson compile -C builddir
meson test -C builddir --print-errorlogs
./builddir/topics/003_sharing_data/topic003_protecting_with_mutex
```

## Naming and numbering

| Thing | Pattern | Example |
|-------|---------|---------|
| Topic directory | `topics/NNN_snake_case_topic/` | `topics/002_thread_management/` |
| Example directory | `NNN_snake_case_example/` | `002_function_object/` |
| Entry point | `main.cpp` (one per example) | — |
| Executable / test | `topicNNN_<example>` | `topic002_function_object` |
| Meson dict key | `<example>` (no numeric prefix) | `function_object` |

`NNN` mirrors the chapter of the reference text, so a topic's number can exceed
its position in `topics/meson.build` (the first topic here is `002`, Chapter 2).
Within a topic, keep example numbers contiguous from `001`; to insert an example
in the middle, prefer taking the next free number over renumbering directories.

## Checklist

Adding an example:

- [ ] `topics/<topic>/NNN_snake_case_example/main.cpp` created (exactly one `main()`)
- [ ] one line added to the topic's `examples` dictionary
- [ ] example `README.md` written; topic README table updated
- [ ] root README table updated (if you keep it current)
- [ ] `meson compile` + `meson test` pass
- [ ] ThreadSanitizer clean, if the example adds shared state

Adding a topic:

- [ ] `topics/NNN_snake_case_topic/` created with at least one example
- [ ] topic `meson.build` added (dictionary loop)
- [ ] `subdir('NNN_snake_case_topic')` added to `topics/meson.build`
- [ ] topic `README.md` added with an Examples table
- [ ] root README Topics table and Roadmap updated
- [ ] `meson compile` + `meson test` pass

## Troubleshooting

| Symptom | Cause / fix |
|---------|-------------|
| New target missing after adding a directory | Meson did not regenerate: `meson setup --reconfigure builddir` |
| `target topicNNN_x not found` | The dict key and directory disagree, or a comma is missing between dictionary entries |
| Documented binary path "not found" | Executables land directly in `builddir/topics/<topic>/`, not under the example subdirectory |
| `meson test` reports a new failure | The example returned non-zero — the smoke test asserts exit status 0 |
| clangd cannot resolve the new file | Refresh the compile database: `meson setup --reconfigure builddir` |
| `meson setup` errors on the dictionary | Dictionary syntax: keys and values are strings separated by `:`, entries separated by commas |
