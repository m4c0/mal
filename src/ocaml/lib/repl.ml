let rep env str = 
  try str |> Reader.read_str |> Eval.eval env |> Printer.pr_str true
  with
  | Core.Invalid_args -> "Invalid arguments"
  | Core.Unreadable_file -> "File not found or not accessible"
  | Eval.Invalid_callable -> "Tryed to call something that is not a function"
  | Eval.Invalid_form -> "Mismatched parameters in core call"
  | Eval.Mismatched_function_args -> "Mismatched parameters in function call"
  | Env.Invalid_binding -> "Invalid binding"
  | Env.Unknown_symbol(s) -> s ^ " not found"
  | Tokeniser.Eof -> "EOF"
  | Types.Expecting_symbol -> "Expecting symbol"

let env = 
  Env.empty
    |> Env.set "+" Core.sum
    |> Env.set "-" Core.sub
    |> Env.set "*" Core.mult
    |> Env.set "/" Core.div
    |> Env.set "list" Core.list_
    |> Env.set "list?" Core.is_list
    |> Env.set "empty?" Core.is_empty
    |> Env.set "count" Core.count
    |> Env.set "=" Core.equal
    |> Env.set "<" Core.lt
    |> Env.set "<=" Core.lte
    |> Env.set ">" Core.gt
    |> Env.set ">=" Core.gte
    |> Env.set "pr-str" Core.pr_str
    |> Env.set "str" Core.str
    |> Env.set "prn" Core.prn
    |> Env.set "println" Core.println
    |> Env.set "read-string" Core.read_string
    |> Env.set "slurp" Core.slurp
    |> Env.set "atom" Core.atom
    |> Env.set "atom?" Core.is_atom
    |> Env.set "deref" Core.deref
    |> Env.set "reset!" Core.reset
    |> Env.set "swap!" Core.swap
    |> Env.set "cons" Core.cons
    |> Env.set "concat" Core.concat
    |> ref

let eval args = 
  match args with
  | [ast] -> Eval.eval env ast
  | _ -> raise Core.Invalid_args

let argv = Sys.argv |> Array.to_list |> List.tl |> List.map Types.of_string

let repl =
  env := Env.set "eval" (Types.Lambda eval) !env;
  env := Env.set "*ARGV*" (Types.List argv) !env;
  "(def! not (fn* (a) (if a false true)))" |> rep env |> ignore;
  "(def! load-file (fn* (f) (eval (read-string (str \"(do \" (slurp f) \"\nnil)\")))))" |> rep env |> ignore;
  try
    while true do
      print_string "user> ";
    read_line () |> rep env |> print_endline
    done;
  with End_of_file -> ()
