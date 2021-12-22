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

function(mal_step LANG NAME FILE)
  set(TARGET "${LANG}.${NAME}")

  add_executable(${TARGET} src/${LANG}/${NAME}.cpp)
  target_compile_features(${TARGET} PRIVATE cxx_std_20)
  target_include_directories(${TARGET} PRIVATE
    ext/m4c0-stl/parser/inc
    inc)

  add_test(
    NAME ${TARGET}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND runtest.py --deferrable --optional tests/${FILE}.mal -- $<TARGET_FILE:${TARGET}>
    )
endfunction()

function(mal_cpp_step NAME FILE)
  mal_step("cpp" ${NAME} ${FILE})
endfunction()

function(mal_llvm_step NAME FILE)
  set(TARGET "llvm.${NAME}")
  mal_step("llvm" ${NAME} ${FILE})
  target_include_directories(${TARGET} PRIVATE ${LLVM_INCLUDE_DIR})
  target_link_libraries(${TARGET} PRIVATE ${CLANG} ${CLANGPP} ${LLVM})
endfunction()

mal_cpp_step(step0 step0_repl)
mal_cpp_step(step1 step1_read_print)
mal_cpp_step(step2 step2_eval)
mal_cpp_step(step3 step3_env)

mal_llvm_step(step0 step0_repl)
mal_llvm_step(step1 step1_read_print)
mal_llvm_step(step2 step2_eval)

