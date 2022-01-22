cmake_minimum_required(VERSION 3.16)
project(mal VERSION 0.1 LANGUAGES CXX ASM)

enable_testing()

find_package(m4c0 REQUIRED CONFIG)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(mal_defs INTERFACE)
target_compile_features(mal_defs INTERFACE cxx_std_20)
target_link_libraries(mal_defs INTERFACE m4c0-parser)

function(mal_step_test_script TEST_NAME MAL_FILE)
  set(CMD ../../runtest.py --deferrable --optional ${MAL_FILE}.mal -- ${ARGN})
  set(DIR ${CMAKE_SOURCE_DIR}/impls/tests)

  add_test(NAME ${TEST_NAME} WORKING_DIRECTORY ${DIR} COMMAND ${CMD})

endfunction()

function(mal_step_test TEST_NAME MAL_FILE TARGET)
  mal_step_test_script(${TEST_NAME} ${MAL_FILE} $<TARGET_FILE:${TARGET}>)
endfunction()
function(mal_step_selfhost_test TEST_NAME MAL_FILE TARGET)
  mal_step_test_script(mal-${TEST_NAME} ${MAL_FILE} $<TARGET_FILE:${TARGET}> ../mal/${MAL_FILE}.mal)
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

#add_subdirectory(src/cpp)
#add_subdirectory(src/llvm)
#add_subdirectory(src/llvm-ir)
add_subdirectory(src/ocaml)
