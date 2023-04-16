cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)

project(syringe VERSION 1.0)

add_library(syringe INTERFACE)

target_include_directories(syringe INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(DIRECTORY include/ DESTINATION include)

file(GLOB EXAMPLE_SOURCES "examples/*.cc")

# Create an executable target for each example file
foreach(E ${EXAMPLE_SOURCES})
  get_filename_component(EXAMPLE_NAME ${E} NAME_WE)
  add_executable(${EXAMPLE_NAME} ${E})
  target_link_libraries(${EXAMPLE_NAME} PRIVATE syringe)
endforeach()

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests tests/test.cc)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE syringe)