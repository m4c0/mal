add_executable(cpp-step0 step0.cpp)
target_link_libraries(cpp-step0 PUBLIC mal_defs)
mal_step_test(cpp-step0 step0_repl cpp-step0)

add_executable(cpp-step1 printer.cpp reader.cpp step1.cpp)
target_link_libraries(cpp-step1 PUBLIC mal_defs)
mal_step_test(cpp-step1 step1_read_print cpp-step1)

add_executable(cpp-stepX
  core.cpp
  env.cpp
  eval.fn.cpp
  eval.impl.cpp
  eval.macro.cpp
  eval.trycatch.cpp
  reader.cpp
  rep.cpp
  printer.cpp
  types.holders.cpp
  stepA.cpp)
target_link_libraries(cpp-stepX PUBLIC mal_defs)
mal_step_test(cpp-step2 step2_eval     cpp-stepX)
mal_step_test(cpp-step3 step3_env      cpp-stepX)
mal_step_test(cpp-step4 step4_if_fn_do cpp-stepX)
mal_step_test(cpp-step5 step5_tco      cpp-stepX)
mal_step_test(cpp-step6 step6_file     cpp-stepX)
mal_step_test(cpp-step7 step7_quote    cpp-stepX)
mal_step_test(cpp-step8 step8_macros   cpp-stepX)
mal_step_test(cpp-step9 step9_try      cpp-stepX)
mal_step_test(cpp-stepA stepA_mal      cpp-stepX)
mal_perf(cpp cpp-stepX)

