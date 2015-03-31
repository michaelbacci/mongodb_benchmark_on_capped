cmake_minimum_required(VERSION 2.8)

add_definitions(-DMONGO_HOST="localhost:27017")
add_definitions(-DMONGO_DB="test")
add_definitions(-DMONGO_DB_AUTH="")
add_definitions(-DMONGO_DB_USER="")
add_definitions(-DMONGO_DB_PSWD="")

macro(compile_benchmark_test name)
  add_executable(${name} "${name}.cpp")
  target_link_libraries(${name} benchmark ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${MongoDB_LIBRARIES})
endmacro(compile_benchmark_test)

# Demonstration executable
compile_benchmark_test(capped_find_and_modify)
add_test(benchmark capped_find_and_modify --benchmark_min_time=0.1)



