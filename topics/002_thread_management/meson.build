# Topic 002 — Thread management.
#
# Each example is a self-contained subdirectory with its own main.cpp.
#   key   -> clean example name, used in the executable and test names
#   value -> numbered example directory; the prefix preserves study order
#
# Adding an example is a single line in the dictionary below.
examples = {
  'hello_concurrent_world' : '001_hello_concurrent_world',
  'function_object'        : '002_function_object',
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
