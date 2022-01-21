add_custom_command(
  OUTPUT ocaml-step0
  COMMAND ocamlopt ${CMAKE_CURRENT_SOURCE_DIR}/step0.ml -o step0
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/step0.ml)
mal_step_test_script(ocaml-step0 step0_repl ${CMAKE_CURRENT_BINARY_DIR}/step0)

add_custom_target(ocaml-steps ALL DEPENDS ocaml-step0)
