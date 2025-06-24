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

file(GLOB test_src test/*.cpp)
enable_testing()

add_executable(
  ir_test
  ${test_src}
)

target_link_libraries(
  ir_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ir_test)
