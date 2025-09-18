cmake_minimum_required(VERSION 3.16)
project(eft_runtime LANGUAGES C CXX)

option(EFT_BUILD_TESTS "Build tests" ON)
option(EFT_BUILD_SHARED "Build EFT as shared library" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library target
set(EFT_SOURCES
    src/eft/shadow_memory.cpp
    src/eft/tmp_mta_space.cpp
    src/eft/shadow_stack.cpp
)

if (EFT_BUILD_SHARED)
  add_library(eft SHARED ${EFT_SOURCES})
else()
  add_library(eft STATIC ${EFT_SOURCES})
endif()

target_include_directories(eft
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ---------- Tests ----------
if (EFT_BUILD_TESTS)
    add_executable(gausselm tests/gausselm.c)

    add_executable(gausselm_eft tests/gausselm_eft.cpp)
    target_link_libraries(gausselm_eft PRIVATE eft)

#   add_executable(benchmark_plain tests/fpcore/benchmark.c)
#   target_link_libraries(benchmark_plain PRIVATE m)

#   add_executable(benchmark_eft tests/fpcore/benchmark_eft.cpp)
#   target_link_libraries(benchmark_eft PRIVATE eft)
endif()
