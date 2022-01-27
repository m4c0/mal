open Types

let macro sym args = of_list (Symbol(sym) :: args)

let rec macroexpand env ast = 
  let extract_macro args = function
    | Some(Macro(fn)) -> args |> fn |> macroexpand env
    | _ -> ast
  in
  match ast with
  | Iterable(List, Symbol(s) :: xs) -> Env.find_opt !env s |> extract_macro xs
  | _ -> ast

let rec quasiquote = function
    | Iterable(List, [Symbol("unquote"); x]) -> x
    | Iterable(List, l) -> quasiquote_rec l
    | Iterable(Vector, l) -> macro "vec" [quasiquote_rec l]
    | (Hashmap(_) | Symbol(_)) as x -> macro "quote" [x]
    | ast -> ast
  and quasiquote_rec = function
    | [] -> Types.empty_list
    | Iterable(List, [Symbol("splice-unquote"); e2]) :: rest -> macro "concat" [e2; quasiquote_rec rest]
    | elt :: rest -> macro "cons" [quasiquote elt; quasiquote_rec rest]

let def_form env key value = env := Env.set key value !env; value
let defmacro_form env key = function
  | Lambda(x) -> def_form env key (Macro x)
  | _ -> Exc.invalid_form ()

let rec eval env (ast : Types.t) =
  let rec eval_form form =
    match macroexpand env form with
    | Iterable(List, []) as ast -> ast
    | Iterable(List, l) -> eval_list l
    | ast -> eval_ast ast
  and eval_list = function
    | [Symbol("def!"); Symbol(key); v] -> def_form env key (eval_form v)
    | [Symbol("defmacro!"); Symbol(key); v] -> defmacro_form env key (eval_form v)
    | [Symbol("macroexpand"); ast] -> macroexpand env ast
    | [Symbol("let*"); Iterable(_, l); form] -> let_form l form
    | Symbol("do") :: l -> l |> List.map eval_form |> List.rev |> List.hd
    | [Symbol("if"); cond; tr] -> if_form cond tr Nil
    | [Symbol("if"); cond; tr; fl] -> if_form cond tr fl
    | [Symbol("fn*"); Iterable(_, sign); body] -> fn_form sign body
    | [Symbol("quote"); x] -> x
    | [Symbol("quasiquoteexpand"); x] -> quasiquote x
    | [Symbol("quasiquote"); x] -> x |> quasiquote |> eval_form
    | [Symbol("try*"); a] -> eval_form a 
    | [Symbol("try*"); a; Iterable(List, [Symbol("catch*"); Symbol(b); c])] -> try_form a b c

    | Symbol("catch*" | "def!" | "defmacro!" | "fn*" | "if" | "let*" | "macroexpand" | "quasiquote" | "quasiquoteexpand" | "try*") :: _ -> Exc.invalid_form ()

    | x ->
       match x |> List.map eval_form with
      | Lambda(fn) :: args -> fn args
      | _ -> Exc.invalid_callable ()
  and eval_ast = function
    | Iterable(_, []) as x -> x
    | Iterable(Vector, x) -> x |> List.map eval_form |> Types.of_vector
    | Iterable(List, x) -> x |> List.map eval_form |> Types.of_list
    | Symbol(x) -> Env.find !env x
    | Hashmap(m) -> Hashmap(TMap.map eval_form m)
    | x -> x
  and let_form l form =
    let new_env = env |> Env.extend |> ref in
    let rec binder ll = begin
      match ll with
          | [] -> eval new_env form
          | Symbol(k) :: v :: xs -> 
              let vv = eval new_env v in
              new_env := Env.set k vv !new_env;
              binder xs
          | _ -> Exc.invalid_form ()
    end in
    binder l
  and if_form cond tr fl =
    match eval_form cond with
    | Nil
    | Bool(false) -> eval_form fl
    | _ -> eval_form tr
  and fn_form sign body =
    let ssign = sign |> List.map Types.string_of_symbol in
    Lambda(fun args -> fn_closure ssign args body)
  and fn_closure params args body =
    let new_env = Env.bind !env params args |> ref in
    eval new_env body
  and try_form a b c =
    try eval_form a
    with e ->
      let exc =
        match e with
        | Application_exception e -> e
        | e -> e |> Exc.to_string |> Types.of_string
    in fn_closure [b] [exc] c
  in eval_form ast
