let rec pr_str t =
  let open Types in
  match t with
  | Integer(i) -> string_of_int i 
  | Symbol(s) -> s 
  | List(l) ->
      let ls = List.map pr_str l in
      "(" ^ (String.concat " " ls) ^ ")" 
