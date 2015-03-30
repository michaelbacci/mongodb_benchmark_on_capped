cmake_minimum_required(VERSION 2.8)

macro(compile_benchmark_test name)
  add_executable(${name} "${name}.cpp")
  target_link_libraries(${name} benchmark ${CMAKE_THREAD_LIBS_INIT})
endmacro(compile_benchmark_test)

# Demonstration executable
compile_benchmark_test(benchmark_test)
add_test(benchmark benchmark_test --benchmark_min_time=0.1)



