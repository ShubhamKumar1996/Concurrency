project('Concurrency',
  'cpp',
  version : '1.0.0',
  license : 'MIT',
  meson_version : '>= 1.3.0',
  default_options : [
    'cpp_std=c++23',
    'warning_level=3',
    'werror=false',
    'buildtype=debug',
  ])

# ---------------------------------------------------------------------------
# Toolchain probing
# ---------------------------------------------------------------------------
cpp = meson.get_compiler('cpp')

# std::thread / std::mutex require an explicit threads dependency.
threads = dependency('threads')

# Extra diagnostics beyond warning_level=3 that are useful for concurrency work.
cxx_args = []
if cpp.get_id() in ['clang', 'gcc']
  cxx_args += [
    '-Wconversion',
    '-Wshadow',
    '-Wnon-virtual-dtor',
    '-Wold-style-cast',
  ]
endif

# ---------------------------------------------------------------------------
# Optional sanitizers (-Dsanitizer=thread|address)
# ---------------------------------------------------------------------------
sanitizer = get_option('sanitizer')
link_args = []
if sanitizer == 'thread'
  cxx_args += ['-fsanitize=thread', '-fno-omit-frame-pointer', '-g']
  link_args += ['-fsanitize=thread']
  message('ThreadSanitizer enabled')
elif sanitizer == 'address'
  cxx_args += ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-g']
  link_args += ['-fsanitize=address,undefined']
  message('AddressSanitizer + UndefinedBehaviorSanitizer enabled')
elif sanitizer != 'none'
  error('Unknown sanitizer option: ' + sanitizer)
endif

# ---------------------------------------------------------------------------
# Topics
# ---------------------------------------------------------------------------
subdir('topics')
subdir('tests')
