module Map = Map.Make(String)

exception Invalid_binding
exception Unknown_symbol of string

type t = {
  outer: t ref option;
  map: Types.t Map.t
}

let extend o = { outer = Some o; map = Map.empty }

let rec find env key = 
  try Map.find key env.map
  with Not_found ->
    match env.outer with
    | None -> raise (Unknown_symbol key)
    | Some(e) -> find !e key

let set key value env =
  let mapper = fun _ -> Some(value) in
  { outer = env.outer; map = Map.update key mapper env.map }

let rec bind o binds exprs =
  match (binds, exprs) with
  | ([], []) -> o 
  | ("&"::b::[], e) -> set b (Types.List e) o
  | (hb::tb, he::te) -> set hb he (bind o tb te) 
  | (_, _) -> raise Invalid_binding

let step2 =
  { outer = None; map = Map.empty }
    |> set "+" Core.sum
    |> set "-" Core.sub
    |> set "*" Core.mult
    |> set "/" Core.div
    |> set "prn" Core.prn
    |> set "list" Core.list_
    |> set "list?" Core.is_list
    |> set "empty?" Core.is_empty
    |> set "count" Core.count
    |> set "=" Core.equal
    |> set "<" Core.lt
    |> set "<=" Core.lte
    |> set ">" Core.gt
    |> set ">=" Core.gte
