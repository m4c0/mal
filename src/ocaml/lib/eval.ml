let rec eval env ast =
  let open Types in
  let rec eval_ast = function
    | Symbol(x) -> Env.find !env x
    | List(l) -> List(List.map eval_form l)
    | Vector(v) -> Vector(List.map eval_form v)
    | Hashmap(m) -> Hashmap(TMap.map eval_form m)
    | x -> x
  and eval_form form = 
    match macroexpand form with
    | List(Symbol("def!") :: Symbol(key) :: v :: []) ->
        let value = eval_form v in
        env := Env.set key value !env; value
    | List(Symbol("def!") :: _) -> Exc.invalid_form ()

    | List(Symbol("defmacro!") :: Symbol(key) :: v :: []) ->
        begin
          match eval_form v with
          | Lambda(x) -> 
              let m = Macro x in
              env := Env.set key m !env; m
          | _ -> Exc.invalid_form ()
        end
    | List(Symbol("defmacro!") :: _) -> Exc.invalid_form ()

    | List(Symbol("macroexpand") :: ast :: []) -> macroexpand ast

    | List(Symbol("let*") :: (List(l) | Vector(l)) :: form :: []) ->
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
    | List(Symbol("let*") :: _) -> Exc.invalid_form ()

    | List(Symbol("do") :: []) -> Exc.invalid_form ()
    | List(Symbol("do") :: l) -> 
        l |> List.map eval_form |> List.rev |> List.hd

    | List(Symbol("if") :: cond :: tr :: []) -> if_form cond tr Nil
    | List(Symbol("if") :: cond :: tr :: fl :: []) -> if_form cond tr fl
    | List(Symbol("if") :: _) -> Exc.invalid_form ()

    | List(Symbol("fn*") :: (List(sign) | Vector(sign)) :: body :: []) -> fn_form sign body
    | List(Symbol("fn*") :: _) -> Exc.invalid_form ()

    | List(Symbol("quote") :: x :: []) -> x

    | List(Symbol("quasiquoteexpand") :: x :: []) -> quasiquote x
    | List(Symbol("quasiquote") :: x :: []) -> x |> quasiquote |> eval_form

    | List([Symbol("try*"); a; List([Symbol("catch*"); Symbol(b); c])]) -> (
        try eval_form a
        with e ->
          let exc =
            match e with
            | Application_exception e -> e
            | e -> e |> Exc.to_string |> Types.of_string
          in fn_closure [b] [exc] c
    )
    | List(Symbol("try*") :: _) -> Exc.invalid_form ()
    | List(Symbol("catch*") :: _) -> Exc.invalid_form ()

    | List(_) as x -> x |> eval_ast |> eval_list
    | x -> eval_ast x
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
  and macroexpand ast =
    match ast with
    | List(Symbol(s) :: xs) ->
        begin
          match Env.find_opt !env s with
          | Some(Macro(fn)) -> macroexpand (fn xs)
          | _ -> ast
        end
    | _ -> ast
  and quasiquote_list elt rest =
    match elt with
    | List([Symbol("splice-unquote"); x]) -> List(Symbol("concat") :: x :: [(quasiquote (List rest))])
    | _ -> List(Symbol("cons") :: (quasiquote elt) :: [(quasiquote (List rest))])
  and quasiquote = function
    | List([Symbol("unquote"); x]) -> x
    | List([]) -> List([])
    | Vector([]) -> List([Symbol("vec"); List([])])
    | Vector(elt :: rest) -> List([Symbol("vec"); quasiquote_list elt rest])
    | List(elt :: rest) -> quasiquote_list elt rest
    | (Hashmap(_) | Symbol(_)) as x -> List([Symbol("quote"); x]) 
    | x -> x
  and eval_list = function
    | List([]) -> List([])
    | List(Lambda(fn) :: args) -> fn args
    | _ -> Exc.invalid_callable ()
  in eval_form ast
