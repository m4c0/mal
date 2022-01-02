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

add_library(mal_defs INTERFACE)
target_compile_features(mal_defs INTERFACE cxx_std_20)
target_include_directories(mal_defs INTERFACE ext/m4c0-stl/parser/inc)

function(mal_step_test TEST_NAME MAL_FILE TARGET)
  set(CMD ../../runtest.py --deferrable --optional ${MAL_FILE}.mal -- $<TARGET_FILE:${TARGET}>)
  set(DIR ${CMAKE_SOURCE_DIR}/impls/tests)

  add_test(NAME ${TEST_NAME} WORKING_DIRECTORY ${DIR} COMMAND ${CMD})

  if(NOT MAL_FILE STREQUAL "step5_tco")
    # There is not step5 in mal's own implementation
    add_test(NAME mal.${TEST_NAME} WORKING_DIRECTORY ${DIR} COMMAND ${CMD} ../mal/${MAL_FILE}.mal)
  endif()
endfunction()

function(mal_perf_step LANG TARGET NAME)
  add_custom_target("${LANG}-${NAME}"
    COMMAND ${TARGET} ${NAME}.mal
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/impls/tests)
endfunction()
function(mal_perf LANG TARGET)
  mal_perf_step(${LANG} ${TARGET} perf1)
  mal_perf_step(${LANG} ${TARGET} perf2)
  mal_perf_step(${LANG} ${TARGET} perf3)
  add_custom_target(${LANG}-perf
    DEPENDS ${LANG}-perf1
    DEPENDS ${LANG}-perf2
    DEPENDS ${LANG}-perf3)
endfunction()

function(mal_llvm_step NAME FILE)
  set(TARGET "llvm.${NAME}")
  mal_step("llvm" ${NAME} ${FILE})
  target_include_directories(${TARGET} PRIVATE ${LLVM_INCLUDE_DIR})
  target_link_libraries(${TARGET} PRIVATE ${CLANG} ${CLANGPP} ${LLVM})
endfunction()

#mal_llvm_step(step0 repl)
#mal_llvm_step(step1 read_print)
#mal_llvm_step(step2 eval)

add_subdirectory(src/cpp)
