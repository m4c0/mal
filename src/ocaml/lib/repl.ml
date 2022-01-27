let rep env str = 
  try str |> Reader.read_str |> Eval.eval env |> Printer.pr_str true
  with e -> Exc.to_string e

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
    |> Env.set "vec" Core.vec
    |> Env.set "nth" Core.nth
    |> Env.set "first" Core.first
    |> Env.set "rest" Core.rest
    |> Env.set "throw" Core.throw
    |> Env.set "apply" Core.apply
    |> Env.set "map" Core.map
    |> Env.set "nil?" Core.is_nil
    |> Env.set "true?" Core.is_true
    |> Env.set "false?" Core.is_false
    |> Env.set "symbol?" Core.is_symbol
    |> Env.set "symbol" Core.symbol
    |> Env.set "keyword" Core.keyword
    |> Env.set "keyword?" Core.is_keyword
    |> Env.set "vector" Core.vector
    |> Env.set "vector?" Core.is_vector
    |> Env.set "sequential?" Core.is_sequential
    |> Env.set "map?" Core.is_map
    |> Env.set "hash-map" Core.hashmap
    |> Env.set "assoc" Core.assoc
    |> Env.set "dissoc" Core.dissoc
    |> Env.set "get" Core.get
    |> Env.set "contains?" Core.contains
    |> Env.set "keys" Core.keys
    |> Env.set "vals" Core.vals
    |> Env.set "readline" Core.readline
    |> Env.set "*host-language*" (String "ocaml")
    |> Env.set "time-ms" Core.timems
    |> Env.set "conj" Core.conj
    |> Env.set "fn?" Core.is_fn
    |> Env.set "string?" Core.is_string
    |> Env.set "number?" Core.is_number
    |> Env.set "macro?" Core.is_macro
    |> Env.set "seq" Core.seq
    |> Env.set "meta" Nil
    |> Env.set "with-meta" Nil
    |> ref

let eval args = 
  match args with
  | [ast] -> Eval.eval env ast
  | _ -> raise Core.Invalid_args

let argv = Sys.argv |> Array.to_list |> List.tl |> List.map Types.of_string

let repl =
  env := Env.set "eval" (Types.Lambda eval) !env;
  env := Env.set "*ARGV*" (Types.of_list argv) !env;
  [ "(def! not (fn* (a) (if a false true)))";
    "(def! load-file (fn* (f) (eval (read-string (str \"(do \" (slurp f) \"\nnil)\")))))";
    "(defmacro! cond (fn* (& xs) (if (> (count xs) 0) (list 'if (first xs) (if (> (count xs) 1) (nth xs 1) (throw \"odd number of forms to cond\")) (cons 'cond (rest (rest xs)))))))";
    "(println (str \"Mal [\" *host-language* \"]\"))"
  ] |> List.map (rep env) |> ignore;
  try
    while true do
      print_string "user> ";
    read_line () |> rep env |> print_endline
    done;
  with End_of_file -> ()
