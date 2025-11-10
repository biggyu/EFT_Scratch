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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
# ---------- Tests ----------
if (EFT_BUILD_TESTS)
  # Collect all .cpp and .c files in tests/ (non-recursive)
  file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp tests/*.c)

  foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})

    # Send this target’s binary to build/tests/
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )

    if (test_src MATCHES "\\.cpp$")
        target_link_libraries(${test_name} PRIVATE eft)
    else()
        target_link_libraries(${test_name} PRIVATE m)
    endif()
  endforeach()
endif()
