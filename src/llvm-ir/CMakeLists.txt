set(LLVM_DIR "/usr" CACHE PATH "Path to LLVM installation")
set(LLVM_BINARY_DIR "${LLVM_DIR}/bin" CACHE PATH "Path to LLVM binaries")

find_program(LLC NAMES llc HINTS ${LLVM_BINARY_DIR} REQUIRED)
find_library(READLINE NAMES readline REQUIRED DOC "GNU readline")

function(mal_ir_step NAME)
  set(TARGET "ir-${NAME}")
  set(IR "${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.ll")
  set(ASM "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.s")

  add_executable(${TARGET} ${ASM})
  target_link_libraries(${TARGET} PRIVATE ${READLINE})

  add_custom_command(OUTPUT ${ASM} COMMAND ${LLC} ${IR} -o ${ASM} MAIN_DEPENDENCY ${IR})
endfunction()

mal_ir_step(step0)
mal_step_test(ir-step0 step0_repl ir-step0)

mal_ir_step(step1)
mal_step_test(ir-step1 step1_read_print ir-step1)

#mal_perf(ir ir-stepX)

