cmake_minimum_required(VERSION 3.10.0)

project(cfg)

find_package(LLVM REQUIRED CONFIG)

find_package(Clang REQUIRED CONFIG)

set(SOURCE_FILES main.cpp)

enable_testing()

add_subdirectory(googletest)

set(Headers
  classes/CFGVisitor.hpp
  classes/MyASTConsumer.hpp
  classes/MyFrontendAction.hpp
)

add_library(Program INTERFACE)
target_include_directories(Program INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/classes)

add_subdirectory(test)

add_executable(cfg ${SOURCE_FILES})

set_target_properties(cfg PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

add_definitions(${LLVM_DEFINITIONS})
add_definitions(${CLANG_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
target_include_directories(cfg PRIVATE ${CLANG_INCLUDE_DIRS})
target_include_directories(cfg PRIVATE ${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

target_link_libraries(cfg 
  clangFrontend
  clangSerialization
  clangDriver
  clangParse
  clangSema
  clangAnalysis
  clangAST
  clangBasic
  clangEdit
  clangLex
  clangTooling
  clang
  LLVM
  libclang
  )
