# Cross-topic / integration tests live here.
#
# Per-topic tests are registered next to their executable in
# topics/*/meson.build. When shared test code becomes necessary, pull in a
# framework via a Meson wrap (see subprojects/) and register executables here:
#
#   doctest_dep = dependency('doctest', fallback : ['doctest', 'doctest_dep'])
#   test_exe = executable('integration_tests', 'main.cpp', dependencies : doctest_dep)
#   test('integration', test_exe)
