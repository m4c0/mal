cmake_minimum_required(VERSION 3.16)
project(mal VERSION 0.1 LANGUAGES CXX)

enable_testing()

add_executable(step0 src/step0.cpp)

add_test(
  NAME step0 
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND runtest.py --deferrable --optional tests/step0_repl.mal -- $<TARGET_FILE:step0>
  )
