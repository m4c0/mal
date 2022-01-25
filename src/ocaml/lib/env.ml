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

let bind o binds exprs =
  let be = 
    try List.combine binds exprs
    with Invalid_argument _ -> raise Invalid_binding
  in
  let set_pair env (k, v) = set k v env in
  List.fold_left set_pair (extend o) be

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
