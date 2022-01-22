add_custom_target(ocaml-build ALL COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runner.sh build)

mal_step_test_script(ocaml-step0 step0_repl ${CMAKE_CURRENT_SOURCE_DIR}/runner.sh exe ./step0.exe)
mal_step_test_script(ocaml-step1 step1_read_print ${CMAKE_CURRENT_SOURCE_DIR}/runner.sh exe ./step1.exe)
