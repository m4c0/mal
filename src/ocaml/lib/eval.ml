exception Invalid_form
exception Invalid_callable

let eval env ast =
  let rec eval_ast ast =
    match ast with
    | Types.Symbol(x) -> Env.find !env x
    | Types.List(l) -> Types.List(List.map eval_form l)
    | Types.Vector(v) -> Types.Vector(List.map eval_form v)
    | Types.Hashmap(m) -> Types.Hashmap(Types.TMap.map eval_form m)
    | _ -> ast
  and eval_form ast =
    match ast with
    | Types.List(Types.Symbol("def!") :: Types.Symbol(key) :: v :: []) ->
        let value = eval_form v in
        env := Env.set key value !env; value
    | Types.List(Types.Symbol("def!") :: _) -> raise Invalid_form
    | Types.List(_) -> ast |> eval_ast |> eval_list
    | x -> eval_ast x
  and eval_list l =
    match l with
    | Types.List([]) -> Types.List([])
    | Types.List(Types.Lambda(fn) :: args) -> fn args
    | _ -> raise Invalid_callable
  in eval_form ast
