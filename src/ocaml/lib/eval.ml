exception Invalid_form
exception Invalid_callable
exception Mismatched_function_args

let rec eval env ast =
  let open Types in
  let rec eval_ast = function
    | Symbol(x) -> Env.find !env x
    | List(l) -> List(List.map eval_form l)
    | Vector(v) -> Vector(List.map eval_form v)
    | Hashmap(m) -> Hashmap(TMap.map eval_form m)
    | x -> x
  and eval_form = function
    | List(Symbol("def!") :: Symbol(key) :: v :: []) ->
        let value = eval_form v in
        env := Env.set key value !env; value
    | List(Symbol("def!") :: _) -> raise Invalid_form

    | List(Symbol("let*") :: (List(l) | Vector(l)) :: form :: []) ->
        let new_env = env |> Env.extend |> ref in
        let rec binder ll = begin
          match ll with
          | [] -> eval new_env form
          | Symbol(k) :: v :: xs -> 
              let vv = eval new_env v in
              new_env := Env.set k vv !new_env;
              binder xs
          | _ -> raise Invalid_form
        end in
        binder l
    | List(Symbol("let*") :: _) -> raise Invalid_form

    | List(Symbol("do") :: []) -> raise Invalid_form
    | List(Symbol("do") :: l) -> 
        l |> List.map eval_form |> List.rev |> List.hd

    | List(Symbol("if") :: cond :: tr :: []) -> if_form cond tr Nil
    | List(Symbol("if") :: cond :: tr :: fl :: []) -> if_form cond tr fl
    | List(Symbol("if") :: _) -> raise Invalid_form

    | List(Symbol("fn*") :: (List(sign) | Vector(sign)) :: body :: []) -> fn_form sign body
    | List(Symbol("fn*") :: _) -> raise Invalid_form

    | List(Symbol("quote") :: x :: []) -> x

    | List(Symbol("quasiquoteexpand") :: x :: []) -> quasiquote x
    | List(Symbol("quasiquote") :: x :: []) -> x |> quasiquote |> eval_form

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
    | _ -> raise Invalid_callable
  in eval_form ast
