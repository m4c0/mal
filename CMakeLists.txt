cmake_minimum_required(VERSION 3.16)
project(mal VERSION 0.1 LANGUAGES CXX)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LLVM_DIR "/usr" CACHE PATH "Path to LLVM installation")
set(LLVM_INCLUDE_DIR "${LLVM_DIR}/include" CACHE PATH "Path to LLVM includes")
set(LLVM_LIBRARY_DIR "${LLVM_DIR}/lib" CACHE PATH "Path to LLVM libraries")

find_library(CLANG clang HINTS ${LLVM_LIBRARY_DIR} REQUIRED)
find_library(CLANGPP clang-cpp HINTS ${LLVM_LIBRARY_DIR} REQUIRED)
find_library(LLVM LLVM HINTS ${LLVM_LIBRARY_DIR} REQUIRED)

function(mal_step NAME FILE)
  add_executable(${NAME} src/${NAME}.cpp)
  target_compile_features(${NAME} PRIVATE cxx_std_20)
  target_include_directories(${NAME} PRIVATE
    ext/m4c0-stl/parser/inc
    inc
    ${LLVM_INCLUDE_DIR}
    )
  target_link_libraries(${NAME} PRIVATE ${CLANG} ${CLANGPP} ${LLVM})

  add_test(
    NAME ${NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND runtest.py --deferrable --optional tests/${FILE}.mal -- $<TARGET_FILE:${NAME}>
    )
endfunction()

mal_step(step0 step0_repl)
mal_step(step1 step1_read_print)
