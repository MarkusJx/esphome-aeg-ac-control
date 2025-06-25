cmake_minimum_required(VERSION 3.15)
project(AEGIR CXX)

cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
function(CloneRepository repositoryURL branchName projectName sourceDir)
  #Commands are left empty so that we only checkout the source and no not perform any kind of build
  message("Starting to clone ${projectName} into ${sourceDir}")
  FetchContent_Declare(
    "${projectName}"
    GIT_REPOSITORY "${repositoryURL}"
    SOURCE_DIR "${sourceDir}"
    GIT_TAG "origin/${branchName}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
  FetchContent_MakeAvailable(${projectName})
endfunction(CloneRepository)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

CloneRepository(https://github.com/Arduino-IRremote/Arduino-IRremote master IRemote ${CMAKE_BINARY_DIR}/iremote)
include_directories(${CMAKE_BINARY_DIR}/iremote/src)

CloneRepository(https://github.com/esphome/esphome dev ESPHome ${CMAKE_BINARY_DIR}/esphome)
include_directories(${CMAKE_BINARY_DIR}/esphome)

option(CMake_RUN_CLANG_TIDY "Run clang-tidy with the compiler." OFF)
if (CMake_RUN_CLANG_TIDY)
    if (CMake_SOURCE_DIR STREQUAL CMake_BINARY_DIR)
        message(FATAL_ERROR "CMake_RUN_CLANG_TIDY requires an out-of-source build!")
    endif ()
    find_program(CLANG_TIDY_COMMAND NAMES clang-tidy)
    if (NOT CLANG_TIDY_COMMAND)
        message(WARNING "CMake_RUN_CLANG_TIDY is ON but clang-tidy is not found!")
        set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE)
    else ()
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND};-header-filter='${CMAKE_SOURCE_DIR}/include/*'")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    endif ()

    # Create a preprocessor definition that depends on .clang-tidy content so
    # the compile command will change when .clang-tidy changes.  This ensures
    # that a subsequent build re-runs clang-tidy on all sources even if they
    # do not otherwise need to be recompiled.  Nothing actually uses this
    # definition.  We add it to targets on which we run clang-tidy just to
    # get the build dependency on the .clang-tidy file.
    file(SHA1 ${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy clang_tidy_sha1)
    set(CLANG_TIDY_DEFINITIONS "CLANG_TIDY_SHA1=${clang_tidy_sha1}")
    unset(clang_tidy_sha1)
endif ()
configure_file(.clang-tidy .clang-tidy COPYONLY)

file(GLOB test_src test/*.cpp)
enable_testing()

set(additional_sources "components/aeg_ac/aeg_ir.cpp")

add_executable(
  ir_test
  ${test_src}
  ${additional_sources}
)

target_link_libraries(
  ir_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ir_test)
