cmake_minimum_required(VERSION 3.10.0)

find_package(LLVM REQUIRED CONFIG)

find_package(Clang REQUIRED CONFIG)

set(Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/classes/CFGVisitor.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/classes/MyASTConsumer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/classes/MyFrontendAction.hpp
)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${Headers})

set(This Tests)

set(Sources
    Tests.cpp
)

add_executable(${This} ${Sources})

target_link_libraries(${This} PUBLIC
    gtest_main
    Program
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

add_test(
    NAME ${This}
    COMMAND ${This}
)