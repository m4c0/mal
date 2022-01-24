module Env = Map.Make(String)

exception Invalid_callable

let rec eval env ast =
  let eval_ast ast =
    match ast with
    | Types.Symbol(x) -> Env.find x env
    | Types.List(l) -> Types.List(List.map (eval env) l)
    | _ -> ast
  in
  match ast with
  | Types.List([]) -> ast
  | Types.List(fn :: args) ->
      begin
        match eval_ast fn with
        | Types.Lambda(l) -> args |> List.map eval_ast |> l
        | _ -> raise Invalid_callable
      end
  | _ -> eval_ast ast
