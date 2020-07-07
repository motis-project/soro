cmake_minimum_required(VERSION 3.17)
project(rapid)

option(RAPID_LINT "Run clang-tidy with the compiler." OFF)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
if (MSVC)
  set(Boost_USE_STATIC_RUNTIME ON)
endif()
find_package(Boost 1.70.0 REQUIRED COMPONENTS filesystem)
find_package(Threads)

include(cmake/buildcache.cmake)
#include(cmake/pkg.cmake)
add_subdirectory(deps)
if(RAPID_LINT)
  include(cmake/clang-tidy.cmake)
endif()

file(GLOB_RECURSE rapid-lib-files src/*.cc)
add_library(rapid-lib ${rapid-lib-files})
target_link_libraries(rapid-lib utl cista)
target_compile_features(rapid-lib PUBLIC cxx_std_20)
target_include_directories(rapid-lib PUBLIC include)

file(GLOB_RECURSE rapid-test-files test/*.cc)
add_executable(rapid-test ${rapid-test-files})
target_link_libraries(rapid-test doctest rapid-lib)