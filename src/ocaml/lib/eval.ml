module Env = Map.Make(String)

exception Unknown_symbol of string
exception Invalid_callable

let eval env ast =
  let rec eval_ast ast =
    match ast with
    | Types.Symbol(x) ->
        begin
          try Env.find x env 
          with Not_found -> raise (Unknown_symbol x)
        end
    | Types.List(l) -> Types.List(List.map eval_form l)
    | Types.Vector(v) -> Types.Vector(List.map eval_form v)
    | Types.Hashmap(m) -> Types.Hashmap(Types.TMap.map eval_form m)
    | _ -> ast
  and eval_form ast =
    match eval_ast ast with
    | Types.List(l) -> eval_list l
    | x -> x
  and eval_list l =
    match l with
    | [] -> Types.List([])
    | Types.Lambda(fn) :: args -> fn args
    | _ -> raise Invalid_callable
  in eval_form ast
