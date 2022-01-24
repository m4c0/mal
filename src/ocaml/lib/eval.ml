exception Invalid_form
exception Invalid_callable

let rec eval env ast =
  let open Types in
  let rec eval_ast ast =
    match ast with
    | Symbol(x) -> Env.find !env x
    | List(l) -> List(List.map eval_form l)
    | Vector(v) -> Vector(List.map eval_form v)
    | Hashmap(m) -> Hashmap(TMap.map eval_form m)
    | _ -> ast
  and eval_form ast =
    match ast with
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
    | List(Symbol("if") :: []) -> raise Invalid_form
    | List(_) -> ast |> eval_ast |> eval_list
    | x -> eval_ast x
  and if_form cond tr fl =
    let c = begin
      match eval_form cond with
      | Nil -> false
      | Bool(x) -> x
      | _ -> true
    end in
    if c then eval_form tr else eval_form fl
  and eval_list l =
    match l with
    | List([]) -> List([])
    | List(Lambda(fn) :: args) -> fn args
    | _ -> raise Invalid_callable
  in eval_form ast
