cmake_minimum_required(VERSION 3.16)
project(mal VERSION 0.1 LANGUAGES CXX ASM)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

#add_subdirectory(src/cpp)
#add_subdirectory(src/llvm)
add_subdirectory(src/llvm-ir)
