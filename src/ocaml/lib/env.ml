module Map = Map.Make(String)

exception Unknown_symbol of string

type t = {
  outer: t option;
  map: Types.t Map.t
}

let create outer map = { outer; map }

let of_list x =
  x
  |> List.to_seq
  |> Map.of_seq
  |> create None

let find env key = 
  try Map.find key env.map
  with Not_found -> raise (Unknown_symbol key)

let step2 =
  [ ("+", Core.sum);
    ("-", Core.sub);
    ("*", Core.mult);
    ("/", Core.div);
  ] |> of_list
