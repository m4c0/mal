add_custom_target(ocaml-build ALL COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runner.sh build)

mal_step_test_script(ocaml-step0 step0_repl ${CMAKE_CURRENT_SOURCE_DIR}/runner.sh exe bin/step0.exe)
mal_step_test_script(ocaml-step1 step1_read_print ${CMAKE_CURRENT_SOURCE_DIR}/runner.sh exe bin/step1.exe)
mal_step_test_script(ocaml-step2 step2_eval ${CMAKE_CURRENT_SOURCE_DIR}/runner.sh exe bin/step2.exe)
mal_step_test_script(ocaml-step3 step3_env ${CMAKE_CURRENT_SOURCE_DIR}/runner.sh exe bin/step2.exe)
mal_step_test_script(ocaml-step4 step4_if_fn_do ${CMAKE_CURRENT_SOURCE_DIR}/runner.sh exe bin/step2.exe)
