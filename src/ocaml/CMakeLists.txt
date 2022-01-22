add_custom_target(ocaml-build ALL COMMAND dune build)

mal_step_test_script(ocaml-step0 step0_repl ${CMAKE_CURRENT_SOURCE_DIR}/runner.sh step0)
mal_step_test_script(ocaml-step1 step1_read_print ${CMAKE_CURRENT_SOURCE_DIR}/runner.sh step1)
