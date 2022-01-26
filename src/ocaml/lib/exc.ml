exception Invalid_form
exception Invalid_callable
exception Mismatched_function_args

let to_string = function
  | Core.Invalid_args -> "Invalid arguments"
  | Core.Out_of_bounds -> "Out of bounds"
  | Core.Unreadable_file -> "File not found or not accessible"
  | Invalid_callable -> "Tryed to call something that is not a function"
  | Invalid_form -> "Mismatched parameters in core call"
  | Mismatched_function_args -> "Mismatched parameters in function call"
  | Env.Invalid_binding -> "Invalid binding"
  | Env.Unknown_symbol(s) -> Printf.sprintf "'%s' not found" s
  | Tokeniser.Eof -> "EOF"
  | Types.Expecting_symbol -> "Expecting symbol"
  | Types.Application_exception(t) -> "Error: " ^ (Printer.pr_str_r t)
  | _ -> "Generic exception"

let invalid_form () = raise Invalid_form
let invalid_callable () = raise Invalid_callable
let mismatched_function_args () = raise Mismatched_function_args
