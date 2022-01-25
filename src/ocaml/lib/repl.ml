let rep env str = 
  try str |> Reader.read_str |> Eval.eval env |> Printer.pr_str
  with
  | Core.Invalid_args -> "Invalid arguments"
  | Eval.Invalid_callable -> "Tryed to call something that is not a function"
  | Eval.Invalid_form -> "Mismatched parameters in core call"
  | Eval.Mismatched_function_args -> "Mismatched parameters in function call"
  | Env.Unknown_symbol(s) -> s ^ " not found"
  | Tokeniser.Eof -> "EOF"
  | Types.Expecting_symbol -> "Expecting symbol"

let env = ref Env.step2

let repl =
  try
    while true do
      print_string "user> ";
    read_line () |> rep env |> print_string;
    print_newline ();
    done;
  with End_of_file -> ()
