cmake_minimum_required(VERSION 3.10)

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
endforeach()
