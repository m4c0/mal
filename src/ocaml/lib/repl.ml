let rep env str = 
  try str |> Reader.read_str |> Eval.eval env |> Printer.pr_str
  with
  | Core.Invalid_args -> "Invalid arguments"
  | Eval.Invalid_callable -> "Tryed to call something that is not a function"
  | Eval.Unknown_symbol(s) -> "Unknown symbol: " ^ s
  | Tokeniser.Eof -> "EOF"

let env = Env.step2

let repl =
  try
    while true do
      print_string "user> ";
    read_line () |> rep env |> print_string;
    print_newline ();
    done;
  with End_of_file -> ()
