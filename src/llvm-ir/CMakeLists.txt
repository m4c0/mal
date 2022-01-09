set(LLVM_DIR "/usr" CACHE PATH "Path to LLVM installation")
set(LLVM_BINARY_DIR "${LLVM_DIR}/bin" CACHE PATH "Path to LLVM binaries")

find_program(LLC NAMES llc HINTS ${LLVM_BINARY_DIR} REQUIRED)
find_library(READLINE NAMES readline REQUIRED DOC "GNU readline")

add_executable(ir-step0 ${CMAKE_CURRENT_BINARY_DIR}/step0.s)
target_link_libraries(ir-step0 PRIVATE ${READLINE})

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/step0.s
  COMMAND ${LLC} ${CMAKE_CURRENT_SOURCE_DIR}/step0.ll -o ${CMAKE_CURRENT_BINARY_DIR}/step0.s
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/step0.ll)

mal_step_test(ir-step0 step0_repl ir-step0)
#mal_perf(ir ir-stepX)

